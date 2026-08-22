
/**
 * @file
 * @brief
 *
 * @author  Anton Bondarev
 * @date    03.07.2026
 */

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <drivers/usb/gadget/gadget.h>
#include <drivers/usb/gadget/udc.h>
#include <drivers/usb/usb_defines.h>
#include <util/log.h>
#include <util/macro.h>
#include <util/math.h>

#include "k1921vg1t_udc_priv.h"

int niiet_udc_ep_type_to(int type) {
	switch (type) {
	case USB_DESC_ENDP_TYPE_CTRL:
		return EP_TYPE_CTRL;
	case USB_DESC_ENDP_TYPE_ISOCHR:
		return EP_TYPE_ISO;
	case USB_DESC_ENDP_TYPE_BULK:
		return EP_TYPE_BULK;
	case USB_DESC_ENDP_TYPE_INTR:
		return EP_TYPE_INT;
	default:
		return -1;
	}
}

void niiet_udc_ep_prepare(struct niiet_usbd_regs *regs, int is_out, int num,
    int type, int max_len) {
	struct niiet_usbd_ep_regs *ep_regs;
	uint32_t ctrl_reg;

	if (is_out == USB_DIR_OUT) {
		ep_regs = &regs->ep_out[num];
	}
	else {
		ep_regs = &regs->ep_in[num];
	}
	ctrl_reg = USBDC_EP_CTRL_MAXPL(max_len);
	ctrl_reg |= USBDC_EP_CTRL_TT(niiet_udc_ep_type_to(type));
	ctrl_reg |= USBDC_EP_CTRL_EV /* | USBDC_EP_CTRL_ED */;

	ep_regs->CTRL |= ctrl_reg;
}

void niiet_usbd_ep_init(struct niiet_usbd_regs *regs, struct usb_gadget_ep *ep) {
	niiet_udc_ep_prepare(regs, ep->dir, ep->nr, ep->desc->b_desc_type,
	    ep->desc->w_max_packet_size);
}

void niiet_udc_ep_reset(struct niiet_usbd_regs *regs, int is_out, int num) {
	struct niiet_usbd_ep_regs *ep_regs;

	if (is_out == USB_DIR_OUT) {
		ep_regs = &regs->ep_out[num];
	}
	else {
		ep_regs = &regs->ep_in[num];
	}

	ep_regs->CTRL = 0;
	ep_regs->DMACTRL = 0;
	ep_regs->DMADESC = 0;
}

void niiet_udc_ep_activate(struct niiet_usbd_regs *regs, int is_out, int num) {
	struct niiet_usbd_ep_regs *ep_regs;

	if (is_out == USB_DIR_OUT) {
		ep_regs = &regs->ep_out[num];
	}
	else {
		ep_regs = &regs->ep_in[num];
	}

	ep_regs->DMACTRL |= USBDC_EP_DMACTRL_DA;
}

void niiet_udc_ep_irq_en(struct niiet_usbd_regs *regs, int is_out, int num) {
	struct niiet_usbd_ep_regs *ep_regs;

	if (is_out == USB_DIR_OUT) {
		ep_regs = &regs->ep_out[num];
	}
	else {
		ep_regs = &regs->ep_in[num];
	}

	ep_regs->DMACTRL |= USBDC_EP_DMACTRL_IE;
}

void *niiet_udc_ep_get_desc(struct niiet_usbd_regs *regs, int is_out, int num) {
	struct niiet_usbd_ep_regs *ep_regs;

	if (is_out == USB_DIR_OUT) {
		ep_regs = &regs->ep_out[num];
	}
	else {
		ep_regs = &regs->ep_in[num];
	}

	return (void *)(uintptr_t)ep_regs->DMADESC;
}
