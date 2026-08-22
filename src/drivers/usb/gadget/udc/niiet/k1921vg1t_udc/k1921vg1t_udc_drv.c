/**
 * @file
 * @brief
 *
 * @author  Anton Bondarev
 * @date    03.07.2026
 */

#include <util/log.h>

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <drivers/usb/gadget/udc.h>
#include <drivers/usb/usb_defines.h>
#include <drivers/usb/gadget/gadget.h>

#include <kernel/printk.h>
#include <kernel/irq.h>
#include <lib/libds/array.h>

#include <util/macro.h>

#include <embox/unit.h>

#include <util/math.h>

#include <drivers/clk/niiet_rcu.h>

#include <config/board_config.h>

#include "k1921vg1t_udc_priv.h"

EMBOX_UNIT_INIT(niiet_udc_init);


#define USB_NUM           OPTION_GET(NUMBER, usb_num)

#define CONF_PREF              MACRO_CONCAT(CONF_USB,USB_NUM)

#define CONF_USB_IRQ              MACRO_CONCAT(CONF_PREF,_IRQ)
#define CONF_USB_REGION_BASE      MACRO_CONCAT(CONF_PREF,_REGION_USBDC_BASE)
#define CONF_USBCTP_REGION_BASE   MACRO_CONCAT(CONF_PREF,_REGION_USBCTR_BASE)
#define CONF_USB_CLK_DEF_USB      MACRO_CONCAT(CONF_PREF,_CLK_DEF_USB)
#define CONF_USB_MISC_EP_MAX_SIZE MACRO_CONCAT(CONF_PREF,_MISC_EP_MAX_SIZE)

#define USB_IRQ_NUM           CONF_USB_IRQ

#define USBD_BASE      ((struct niiet_usbd_regs *)(uintptr_t)CONF_USB_REGION_BASE)
#define USBCTP_BASE    ((struct niiet_usbphy_regs *)(uintptr_t)CONF_USBCTP_REGION_BASE)

#define NIIET_UDC_IN_EP_MASK  ((1 << 1) | (1 << 2) | (1 << 3))
#define NIIET_UDC_OUT_EP_MASK ((1 << 1) | (1 << 2) | (1 << 3))


static inline void usb_control_header_show(struct usb_control_header *ctrl) {
	if (log_level_self() >= LOG_DEBUG) {
		printk("usb_control_header:\n");
		printk("\tbm_req_type: 0x%02x\n", ctrl->bm_request_type);
		printk("\tb_request:   0x%02x\n", ctrl->b_request);
		printk("\tw_value:   0x%04x\n", ctrl->w_value);
		printk("\tw_index:   0x%04x\n", ctrl->w_index);
		printk("\tw_length:  0x%04x\n", ctrl->w_length);
	}
}

static inline int niiet_usb_idx(int ep_nr, int dir) {
	if (dir == USB_DIR_IN) {
		return ep_nr;
	} else {
		return (ep_nr + NIIET_UDC_EPS_COUNT);
	}
}

static inline struct ep_req_desc *
niiet_usb_desc_prepare(struct niiet_udc *niiet_udc, int ep_nr, int dir) {
	int num;
	struct ep_req_desc *desc;

	num = niiet_usb_idx(ep_nr, dir);

	desc = &niiet_udc->ep_req_desc[num];
	niiet_usb_desc_set_buf(desc, niiet_udc->dma_buf[num]);

	niiet_usb_desc_set_next(desc, NULL);
	niiet_usb_desc_init(desc, 0, 0);

	return desc;
}

static inline struct ep_req_desc *
niiet_usbd_init_ep(struct niiet_udc *niiet_udc, int num, int dir) {
	struct ep_req_desc *desc;
	struct niiet_usbd_regs *regs;
	struct niiet_usbd_ep_regs *ep_regs;

	regs = niiet_udc->regs;

	if (dir == USB_DIR_OUT) {
		ep_regs = &regs->ep_out[num];
	}
	else {
		ep_regs = &regs->ep_in[num];
	}

	desc = niiet_usb_desc_prepare(niiet_udc, num, dir);

	ep_regs->DMADESC = (uintptr_t)desc;
	ep_regs->DMACTRL = 0;

	return desc;
}

static int niiet_usbd_init_ep_ctrl_desc(struct usb_udc *udc, int num) {
	struct ep_req_desc *desc;
	struct niiet_udc *niiet_udc;
	struct niiet_usbd_regs *regs;

	niiet_udc = member_cast_out(udc, struct niiet_udc, udc);
	regs = niiet_udc->regs;

	desc = niiet_usbd_init_ep(niiet_udc, num, USB_DIR_OUT);
	niiet_usb_desc_init(desc, EP_DESCR_CTRL_EN | EP_DESCR_CTRL_IE, 0);
	niiet_udc_ep_irq_en(regs, USB_DIR_OUT, num);
	niiet_udc_ep_activate(regs, USB_DIR_OUT, num);

	log_debug("init out[%d].CTRL(%x)", num, regs->ep_out[num].CTRL);
	log_debug("init out[%d].DMADESC(%x)", num, regs->ep_out[num].DMADESC );
	log_debug("init out[%d].DMACTRL (%x)", num, regs->ep_out[num].DMACTRL);

	desc = niiet_usbd_init_ep(niiet_udc, num, USB_DIR_IN);
	niiet_usb_desc_init(desc, 0, 0);
	niiet_udc_ep_activate(regs, USB_DIR_IN, num);

	log_debug("init in[%d].CTRL(%x)", num, regs->ep_in[num].CTRL);
	log_debug("init in[%d].DMADESC(%x)", num, regs->ep_in[num].DMADESC );
	log_debug("init in[%d].DMACTRL (%x)", num, regs->ep_in[num].DMACTRL);

	return 0;
}

static int niiet_usbd_irq_ep_rx(struct niiet_udc *niiet_udc, int num) {
	struct ep_req_desc *desc;
	struct niiet_usbd_regs *regs;
	const void *dma_buf;
	uint32_t ctlr_reg;

	regs = niiet_udc->regs;
	desc = (struct ep_req_desc *)(uintptr_t)regs->ep_out[num].DMADESC;

	ctlr_reg = desc->ctrl;
	if ((ctlr_reg & EP_DESCR_CTRL_EN)) {
		return 0;
	}

	dma_buf = (void *)(uintptr_t)desc->addr;
	log_debug("rx ep (%d) desc (%x) ctrl(%x)", num, desc, ctlr_reg);

	if ((num == 0)) {
		if ((ctlr_reg & EP_DESCR_CTRL_SE)) {
			static struct usb_control_header ctrl = {0};
			memcpy(&ctrl, dma_buf, sizeof(ctrl));
			usb_control_header_show(&ctrl);
			usb_gadget_setup(niiet_udc->udc.udc_composite, &ctrl,
			    (uint8_t *)niiet_udc->buf);
		}
		else {
			int len = ctlr_reg & EP_DESCR_CTRL_LEN_MASK;
			log_debug("rx ep (%d) desc (%x) len(%d)", num, desc, len);
		}
	}
	else {
		struct usb_gadget_request *req;
		int idx = niiet_usb_idx(num, USB_DIR_OUT);
		int len = ctlr_reg & EP_DESCR_CTRL_LEN_MASK;

		log_debug("rx ep (%d) desc (%x) len(%d)", num, desc, len);

		niiet_udc->ep_info[idx].is_used = 0;

		req = niiet_udc->requests[idx];
		niiet_udc->requests[idx] = NULL;

		if (req != NULL && req->complete != NULL) {
			memcpy(req->buf, dma_buf, len);
			req->actual_len = len;
			req->complete(niiet_udc->eps[idx], req);
		}
	}

	niiet_usb_desc_init(desc, EP_DESCR_CTRL_EN | EP_DESCR_CTRL_IE, 0);

	niiet_udc_ep_activate(regs, USB_DIR_OUT, num);

	return 0;
}

static int niiet_usbd_irq_ep_tx(struct niiet_udc *niiet_udc, int num) {
	struct ep_req_desc *desc;
	struct niiet_usbd_regs *regs;
	uint32_t ctlr_reg;

	regs = niiet_udc->regs;
	desc = (struct ep_req_desc *)(uintptr_t)regs->ep_in[num].DMADESC;

	ctlr_reg = desc->ctrl;
	if ((ctlr_reg & EP_DESCR_CTRL_EN)) {
		return 0;
	}

	log_debug("tx ep (%d) desc (%x) ctrl(%x)", num, desc, ctlr_reg);

	if ((num != 0)) {
		struct usb_gadget_request *req;
		int idx = niiet_usb_idx(num, USB_DIR_IN);
		int len = ctlr_reg & EP_DESCR_CTRL_LEN_MASK;

		log_debug("tx ep (%d) desc (%x) len(%d)", num, desc, len);

		niiet_udc->ep_info[idx].is_used = 0;

		req = niiet_udc->requests[idx];
		niiet_udc->requests[idx] = NULL;

		if (req != NULL && req->complete != NULL) {
			req->complete(niiet_udc->eps[idx], req);
		}
	}

	return 0;
}

static int niiet_usbd_irq_ep_parse(struct niiet_udc *niiet_udc) {
	struct niiet_usbd_regs *regs = niiet_udc->regs;
	//struct ep_req_desc *desc;

	int i;

	if (niiet_udc->status == 0) {
		return 0;
	}
	for (i = 0; i < NIIET_UDC_EPS_COUNT; i++) {
		if ((regs->ep_out[i].STAT & USBDC_EP_STAT_PT) && niiet_udc->eps[niiet_usb_idx(i, USB_DIR_OUT)]) {
			niiet_usbd_irq_ep_rx(niiet_udc, i);
		}

		if (/* (regs->ep_in[i].STAT & USBDC_EP_STAT_PT) && */ niiet_udc->requests[niiet_usb_idx(i, USB_DIR_IN)]) {
			//regs->ep_in[i].STAT |= USBDC_EP_STAT_PT;
			niiet_usbd_irq_ep_tx(niiet_udc, i);
		}

	}
	
	return 0;
}

static irq_return_t niiet_usbd_irq_handler(unsigned int irq_nr, void *data) {
	struct niiet_udc *niiet_udc = member_cast_out(data, struct niiet_udc, udc);
	struct niiet_usbd_regs *regs = niiet_udc->regs;
	//int handled = 0;

	//niiet_usbd_irq_ep_parse(niiet_udc);
	//log_debug("regs->GSTAT (%x)",regs->GSTAT);

	if (regs->GSTAT & USBDC_GSTAT_URES) {
		regs->GSTAT |= (USBDC_GSTAT_URES);
		niiet_usbd_init_ep_ctrl_desc(data, 0);
		niiet_udc->status = 1;

		//handled = 1;
	}

	if (regs->GSTAT & USBDC_GSTAT_VBUS) {
		regs->GSTAT |= (USBDC_GSTAT_VBUS);
		//handled = 1;
	}

	if (regs->GSTAT & USBDC_GSTAT_SPEED) {
		regs->GSTAT |= (USBDC_GSTAT_SPEED);
		//handled = 1;
	}
	//if (!handled) {
		niiet_usbd_irq_ep_parse(niiet_udc);
	//}

	return IRQ_HANDLED;
}

static int niiet_udc_start(struct usb_udc *udc) {
	struct niiet_udc *niiet_udc = member_cast_out(udc, struct niiet_udc, udc);
	struct niiet_usbd_regs *regs = niiet_udc->regs;
	int i ;


	regs->GSTAT |= (USBDC_GSTAT_URES);

	regs->GCTRL |= GCTRL_SI | GCTRL_UI | GCTRL_VI | GCTRL_FI;

	regs->GCTRL |= GCTRL_EP | GCTRL_DH;

	for (i = 0; i < (sizeof(niiet_udc->eps)/sizeof(niiet_udc->eps[0])); i++) {
		niiet_udc->eps[i] = NULL;
	}

	niiet_udc->eps[niiet_usb_idx(0, USB_DIR_IN)] =  &udc->udc_composite->ep0;
	niiet_udc->eps[niiet_usb_idx(0, USB_DIR_OUT)] = &udc->udc_composite->ep0;

	niiet_udc_ep_prepare(regs, USB_DIR_IN, 0, USB_DESC_ENDP_TYPE_CTRL, USB_MAX_EP0_SIZE);
	niiet_udc_ep_prepare(regs, USB_DIR_OUT, 0, USB_DESC_ENDP_TYPE_CTRL, USB_MAX_EP0_SIZE);

	niiet_usbd_init_ep_ctrl_desc(udc, 0);

	log_debug("regs->GSTAT (%x)",regs->GSTAT);

	return 0;
}

static int niiet_udc_ep_queue(struct usb_gadget_ep *ep, struct usb_gadget_request *req) {
	struct ep_req_desc *desc;
	struct niiet_udc *niiet_udc = member_cast_out(ep->udc, struct niiet_udc, udc);
	struct niiet_usbd_regs *regs = niiet_udc->regs;

	log_debug("ep_queue: ep=%u dir=%s len=%u",
	    ep->nr, ep->dir == USB_DIR_IN ? "IN" : "OUT", (unsigned)req->len);

	if (ep->dir == USB_DIR_IN) {
		int idx = niiet_usb_idx(ep->nr, ep->dir);

		niiet_udc->ep_info[idx].is_used = 1;
		niiet_udc->ep_info[idx].total_length = req->len;
		niiet_udc->ep_info[idx].rem_length = req->len;

		niiet_udc->requests[idx] = req;
		niiet_udc->eps[idx] = ep;

		desc = (struct ep_req_desc *)(uintptr_t)regs->ep_in[ep->nr].DMADESC;
		if (niiet_usb_desc_avail(desc)) {
			log_debug("niiet_usb_desc_avail");
			assert(0);
		}

		if (req->len) {
			memcpy(niiet_usb_desc_get_buf(desc), req->buf, req->len);
		}

		niiet_usb_desc_init(desc, EP_DESCR_CTRL_EN | EP_DESCR_CTRL_IE,req->len);
		//niiet_udc_ep_irq_en(regs, USB_DIR_IN, ep->nr);
		niiet_udc_ep_activate(regs, USB_DIR_IN, ep->nr);
	} else {
		if (ep->nr != 0) {
			int idx = niiet_usb_idx(ep->nr, ep->dir);

			niiet_udc->ep_info[idx].is_used = 1;
			niiet_udc->ep_info[idx].total_length = req->len;
			niiet_udc->ep_info[idx].rem_length = req->len;

			niiet_udc->requests[idx] = req;
			niiet_udc->eps[idx] = ep;

			desc = (struct ep_req_desc *)(uintptr_t)regs->ep_out[ep->nr].DMADESC;
			niiet_usb_desc_init(desc, EP_DESCR_CTRL_EN | EP_DESCR_CTRL_IE, 0);
			//niiet_udc_ep_irq_en(regs, USB_DIR_OUT, ep->nr);
			niiet_udc_ep_activate(regs, USB_DIR_OUT, ep->nr);

			return 0;
		}

		desc = (struct ep_req_desc *)(uintptr_t)regs->ep_in[ep->nr].DMADESC;
		if (niiet_usb_desc_avail(desc)) {
			log_debug("niiet_usb_desc_avail");
			assert(0);
		}

		if (req->len) {
			memcpy(niiet_usb_desc_get_buf(desc), req->buf, req->len);
		}

		niiet_usb_desc_init(desc, EP_DESCR_CTRL_EN,req->len);

		niiet_udc_ep_activate(regs, USB_DIR_IN, ep->nr);
	}

	return 0;
}

static int niiet_udc_ep_stall(struct usb_gadget_ep *ep,
    const struct usb_control_header *ctrl)  {
	struct niiet_udc *niiet_udc;
	struct niiet_usbd_regs *regs;

	niiet_udc = member_cast_out(ep->udc, struct niiet_udc, udc);
	regs = niiet_udc->regs;

	if (ep->dir == USB_DIR_IN) {
		regs->ep_out[ep->nr].CTRL |= USBDC_EP_CTRL_CS;
	} else {
		regs->ep_in[ep->nr].CTRL |= USBDC_EP_CTRL_CS;
	}
	return 0;
}

static void niiet_udc_ep_enable(struct usb_gadget_ep *ep) {
	struct niiet_udc *niiet_udc;
	struct niiet_usbd_regs *regs;

	niiet_udc = member_cast_out(ep->udc, struct niiet_udc, udc);
	regs = niiet_udc->regs;

	log_debug("niiet_udc_ep_enable: ep=%u dir=%s type=%u",
	    ep->nr, ep->dir == USB_DIR_IN ? "IN" : "OUT", usb_gadget_ep_type(ep));

	niiet_usbd_ep_init(regs, ep);
	niiet_usbd_init_ep(niiet_udc, ep->nr, ep->dir);

	niiet_udc_ep_activate(regs, ep->dir, ep->nr);
}

static int niiet_udc_set_addr(struct usb_udc *udc, uint8_t addr) {
	struct niiet_udc *niiet_udc = member_cast_out(udc, struct niiet_udc, udc);

	log_debug("niiet_udc_set_addr: addr=%u", addr);
	niiet_udc->addr = 6;

	niiet_udc->regs->GCTRL |= GCTRL_UA(addr) | GCTRL_SU;

	return 0;
}

static struct usb_udc_ops niiet_usb_udc_ops = {
	.uuo_udc_start = niiet_udc_start,
	.uuo_ep_queue = niiet_udc_ep_queue,
	.uuo_ep_stall = niiet_udc_ep_stall,
	.uuo_ep_enable = niiet_udc_ep_enable,
	.uuo_set_addr = niiet_udc_set_addr,
};

struct niiet_udc niiet_udc = {
    .udc =
        {
            .udc_name = "udc",
            .udc_ops = &niiet_usb_udc_ops,

            .udc_in_ep_mask = NIIET_UDC_IN_EP_MASK,
            .udc_out_ep_mask = NIIET_UDC_OUT_EP_MASK,

			.udc_ep0_max_size = USB_MAX_EP0_SIZE,
			.udc_ep_max_size = CONF_USB_MISC_EP_MAX_SIZE,
        },
};

static int niiet_udc_init(void) {
	int ret;

	niiet_udc.status = 0;

	niiet_udc.addr= 0;

	niiet_udc.regs = USBD_BASE;
	niiet_udc.usbphy_regs = USBCTP_BASE;

	clk_enable(CONF_USB_CLK_DEF_USB);
	//sys_ctrl_set_pwr_mode("USBD1", 0x3);

	niiet_udc.usbphy_regs->HOSTEN = 0;
	usb_phy_bconf_init(&niiet_udc);

	niiet_udc.regs->GCTRL = 0;
	niiet_udc.regs->GSTAT |= (0xFFFFFFFF);

	for (int i = 0; i < NIIET_UDC_EPS_COUNT; i++) {
		niiet_udc_ep_reset(niiet_udc.regs, 0, i);
		niiet_udc_ep_reset(niiet_udc.regs, 1, i);

    }

	ret = irq_attach(USB_IRQ_NUM, niiet_usbd_irq_handler, 0, &niiet_udc, "udc");
	if (ret != 0) {
		log_error("USB irq attach failed");
		return ret;
	}

	usb_gadget_udc_register(&niiet_udc.udc);

	return 0;
}

STATIC_IRQ_ATTACH(USB_IRQ_NUM, niiet_usbd_irq_handler, &niiet_udc);