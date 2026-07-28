/**
 * @file
 * @brief
 *
 * @author  Anton Bondarev
 * @date    03.07.2026
 */

#ifndef _DRIVERS_USB_GADGET_UDC_NIIET_PRIV_H_
#define _DRIVERS_USB_GADGET_UDC_NIIET_PRIV_H_

#include <stdint.h>

#include <drivers/usb/gadget/udc.h>
#include <drivers/usb/usb_defines.h>

#define USB_MAX_EP0_SIZE 64U



#define NIIET_UDC_EPS_COUNT   4 /* 4 IN and  4 OUT */

#define NIIET_UDC_EPS_MAX    16


struct niiet_usbd_ep_regs {
	volatile uint32_t CTRL;
	volatile uint32_t DMACTRL;
	volatile uint32_t DMADESC;
	volatile uint32_t STAT;
};

#define USBDC_EP_CTRL_EV_Pos         0
#define USBDC_EP_CTRL_ED_Pos         1
#define USBDC_EP_CTRL_EH_Pos         2
#define USBDC_EP_CTRL_TT_Pos         3
#define USBDC_EP_CTRL_NT_Pos         5
#define USBDC_EP_CTRL_MAXPL_Pos      7
#define USBDC_EP_CTRL_CS_Pos         18
#define USBDC_EP_CTRL_CB_Pos         19
#define USBDC_EP_CTRL_PI_Pos         20
#define USBDC_EP_CTRL_BUFSZ_Pos      21

#define EP_TYPE_CTRL (0x0)
#define EP_TYPE_ISO  (0x1)
#define EP_TYPE_BULK (0x2)
#define EP_TYPE_INT  (0x3)

#define USBDC_EP_CTRL_EV         (1 << USBDC_EP_CTRL_EV_Pos)
#define USBDC_EP_CTRL_ED         (1 << USBDC_EP_CTRL_ED_Pos)
#define USBDC_EP_CTRL_EH         (1 << USBDC_EP_CTRL_EH_Pos)
#define USBDC_EP_CTRL_TT(type)   ((type & 0x3) << USBDC_EP_CTRL_TT_Pos)
#define USBDC_EP_CTRL_NT(n)      ((n & 0x3) << USBDC_EP_CTRL_NT_Pos)
#define USBDC_EP_CTRL_MAXPL(len) ((len & 0x7FF) << USBDC_EP_CTRL_MAXPL_Pos)
#define USBDC_EP_CTRL_CS         (1 << USBDC_EP_CTRL_CS_Pos)
#define USBDC_EP_CTRL_CB         (1 << USBDC_EP_CTRL_CB_Pos)
#define USBDC_EP_CTRL_PI         (1 << USBDC_EP_CTRL_PI_Pos)
#define USBDC_EP_CTRL_BUFSZ(len) ((len & 0x7FF) << USBDC_EP_CTRL_BUFSZ_Pos)

#define USBDC_EP_DMACTRL_DA_Pos    0
#define USBDC_EP_DMACTRL_IE_Pos    1
#define USBDC_EP_DMACTRL_AI_Pos    2
#define USBDC_EP_DMACTRL_AD_Pos    3
#define USBDC_EP_DMACTRL_AE_Pos    10

#define USBDC_EP_DMACTRL_DA       (1 << USBDC_EP_DMACTRL_DA_Pos)
#define USBDC_EP_DMACTRL_IE       (1 << USBDC_EP_DMACTRL_IE_Pos)
#define USBDC_EP_DMACTRL_AI       (1 << USBDC_EP_DMACTRL_AI_Pos)
#define USBDC_EP_DMACTRL_AD       (1 << USBDC_EP_DMACTRL_AD_Pos)
#define USBDC_EP_DMACTRL_AE       (1 << USBDC_EP_DMACTRL_AE_Pos)

#define USBDC_EP_STAT_BS_Pos                 0 
#define USBDC_EP_STAT_B0_Pos                 1 
#define USBDC_EP_STAT_B1_Pos                 2 
#define USBDC_EP_STAT_B0CNT_Pos              3 
#define USBDC_EP_STAT_B1CNT_Pos              16
#define USBDC_EP_STAT_PT_Pos                 29

#define USBDC_EP_STAT_BS                 (1 << USBDC_EP_STAT_BS_Pos     )
#define USBDC_EP_STAT_B0                 (1 << USBDC_EP_STAT_B0_Pos     )
#define USBDC_EP_STAT_B1                 (1 << USBDC_EP_STAT_B1_Pos     )
#define USBDC_EP_STAT_B0CNT              (1 << USBDC_EP_STAT_B0CNT_Pos  )
#define USBDC_EP_STAT_B1CNT              (1 << USBDC_EP_STAT_B1CNT_Pos  )
#define USBDC_EP_STAT_PT                 (1 << USBDC_EP_STAT_PT_Pos     )

struct niiet_usbd_regs {
	struct niiet_usbd_ep_regs ep_out[NIIET_UDC_EPS_MAX];
	//uint32_t reserved0[48];
	struct niiet_usbd_ep_regs ep_in[NIIET_UDC_EPS_MAX];  /* 0x100 */
	//uint32_t reserved1[48];
	volatile uint32_t GCTRL;              /* 0x200 */
	volatile uint32_t GSTAT;              /* 0x204 */
};

/* Bit field positions: */
#define USBDC_GCTRL_SU_Pos 0  /*!< Set USB address */
#define USBDC_GCTRL_UA_Pos 1  /*!< USB address */
#define USBDC_GCTRL_TM_Pos 8  /*!< Enable test mode */
#define USBDC_GCTRL_TS_Pos 9  /*!< Testmode selector */
#define USBDC_GCTRL_RW_Pos 12 /*!< Remote wakeup */
#define USBDC_GCTRL_DH_Pos 13 /*!< Disable High-speed */
#define USBDC_GCTRL_EP_Pos 14 /*!< Enable Pull-Up */
#define USBDC_GCTRL_FT_Pos 15 /*!< Functional test mode */
#define USBDC_GCTRL_FI_Pos 27 /*!< Frame number received interrupt */
#define USBDC_GCTRL_SP_Pos 28 /*!< Speed mode interrupt */
#define USBDC_GCTRL_VI_Pos 29 /*!< VBUS valid interrupt */
#define USBDC_GCTRL_UI_Pos 30 /*!< USB reset */
#define USBDC_GCTRL_SI_Pos 31 /*!< Suspend interrupt */

#define GCTRL_SU        (1 << USBDC_GCTRL_SU_Pos)
#define GCTRL_UA(addr)  ((addr & 0x7F) << USBDC_GCTRL_UA_Pos)
#define GCTRL_TM        (1 << USBDC_GCTRL_TM_Pos)
#define GCTRL_TS        (1 << USBDC_GCTRL_TS_Pos)
#define GCTRL_RW        (1 << USBDC_GCTRL_RW_Pos)
#define GCTRL_DH        (1 << USBDC_GCTRL_DH_Pos)
#define GCTRL_EP        (1 << USBDC_GCTRL_EP_Pos)
#define GCTRL_FT        (1 << USBDC_GCTRL_FT_Pos)
#define GCTRL_FI        (1 << USBDC_GCTRL_FI_Pos)
#define GCTRL_SP        (1 << USBDC_GCTRL_SP_Pos)
#define GCTRL_VI        (1 << USBDC_GCTRL_VI_Pos)
#define GCTRL_UI        (1 << USBDC_GCTRL_UI_Pos)
#define GCTRL_SI        (1 << USBDC_GCTRL_SI_Pos)

#define USBDC_GSTAT_FN_Pos    0
#define USBDC_GSTAT_AF_Pos    11
#define USBDC_GSTAT_SPEED_Pos 14
#define USBDC_GSTAT_VBUS_Pos  15
#define USBDC_GSTAT_URES_Pos  16
#define USBDC_GSTAT_SU_Pos    17
#define USBDC_GSTAT_DM_Pos    23
#define USBDC_GSTAT_NEPO_Pos  24
#define USBDC_GSTAT_NEPI_Pos  28

#define USBDC_GSTAT_FN    (1 << USBDC_GSTAT_FN_Pos)
#define USBDC_GSTAT_AF    (0x7 << USBDC_GSTAT_AF_Pos)
#define USBDC_GSTAT_SPEED (1 << USBDC_GSTAT_SPEED_Pos)
#define USBDC_GSTAT_VBUS  (1 << USBDC_GSTAT_VBUS_Pos)
#define USBDC_GSTAT_URES  (1 << USBDC_GSTAT_URES_Pos)
#define USBDC_GSTAT_SU    (1 << USBDC_GSTAT_SU_Pos)
#define USBDC_GSTAT_DM    (1 << USBDC_GSTAT_DM_Pos)
#define USBDC_GSTAT_NEPO  (0xF << USBDC_GSTAT_NEPO_Pos)
#define USBDC_GSTAT_NEPI  (0xF << USBDC_GSTAT_NEPI_Pos)

struct niiet_usbphy_regs {
	volatile uint32_t HOSTEN;  /*!< HOSTEN USB Host Enable register */
	volatile uint32_t PHYSTAT; /*!< PHYSTAT USB PHY Layer Status register  */
	volatile uint32_t PHYCFG0; /*!< PHYCFG0 USB Physical Layer Config 0 register  */
	volatile uint32_t PHYCFG1; /*!< PHYCFG1 USB Physical Layer Config 1 register */
};

#define USBCTR_PHYCFG0_EXTPHY_Pos 0 /*!< Use external PHY Layer */
#define USBCTR_PHYCFG0_POL_Pos    1 /*!< Polarity */
#define USBCTR_PHYCFG0_ATPSEL_Pos 2 /*!< ATP select */
#define USBCTR_PHYCFG0_DTPSEL_Pos 8 /*!< DTP select */

#define USBCTR_PHYCFG0_EXTPHY (1 << USBCTR_PHYCFG0_EXTPHY_Pos)
#define USBCTR_PHYCFG0_POL    (1 << USBCTR_PHYCFG0_POL_Pos)
#define USBCTR_PHYCFG0_ATPSEL (1 << USBCTR_PHYCFG0_ATPSEL_Pos)
#define USBCTR_PHYCFG0_DTPSEL (1 << USBCTR_PHYCFG0_DTPSEL_Pos)

#define USBCTR_PHYSTAT_ROUT_CODE_Pos   0
#define USBCTR_PHYSTAT_RSET_CODE_Pos   4
#define USBCTR_PHYSTAT_ROUT_LOCKED_Pos 11
#define USBCTR_PHYSTAT_RS_LOCKED_Pos   12
#define USBCTR_PHYSTAT_UTMI_CLK_EN_Pos 13
#define USBCTR_PHYSTAT_PLL_LOCK_Pos    15
#define USBCTR_PHYSTAT_PD_PLL_REQ_Pos  16
#define USBCTR_PHYSTAT_RESET_Pos       17
#define USBCTR_PHYSTAT_SUSPENDM_Pos    18
#define USBCTR_PHYSTAT_TERMSELECT_Pos  19
#define USBCTR_PHYSTAT_HST_DISC_Pos    20
#define USBCTR_PHYSTAT_XCVRSELECT_Pos  21
#define USBCTR_PHYSTAT_OPMODE_Pos      23
#define USBCTR_PHYSTAT_EN_RPU_Pos      25
#define USBCTR_PHYSTAT_TX_LSFS_EN_Pos  27
#define USBCTR_PHYSTAT_RX_SE_EN_Pos    28
#define USBCTR_PHYSTAT_RX_LSFS_EN_Pos  29
#define USBCTR_PHYSTAT_RECEIVE_EN_Pos  30
#define USBCTR_PHYSTAT_POL_Pos         31

#define USBCTR_PHYSTAT_ROUT_CODE   (1 << USBCTR_PHYSTAT_ROUT_CODE_Pos)
#define USBCTR_PHYSTAT_RSET_CODE   (1 << USBCTR_PHYSTAT_RSET_CODE_Pos)
#define USBCTR_PHYSTAT_ROUT_LOCKED (1 << USBCTR_PHYSTAT_ROUT_LOCKED_Pos)
#define USBCTR_PHYSTAT_RS_LOCKED   (1 << USBCTR_PHYSTAT_RS_LOCKED_Pos)
#define USBCTR_PHYSTAT_UTMI_CLK_EN (1 << USBCTR_PHYSTAT_UTMI_CLK_EN_Pos)
#define USBCTR_PHYSTAT_PLL_LOCK    (1 << USBCTR_PHYSTAT_PLL_LOCK_Pos)
#define USBCTR_PHYSTAT_PD_PLL_REQ  (1 << USBCTR_PHYSTAT_PD_PLL_REQ_Pos)
#define USBCTR_PHYSTAT_RESET       (1 << USBCTR_PHYSTAT_RESET_Pos)
#define USBCTR_PHYSTAT_SUSPENDM    (1 << USBCTR_PHYSTAT_SUSPENDM_Pos)
#define USBCTR_PHYSTAT_TERMSELECT  (1 << USBCTR_PHYSTAT_TERMSELECT_Pos)
#define USBCTR_PHYSTAT_HST_DISC    (1 << USBCTR_PHYSTAT_HST_DISC_Pos)
#define USBCTR_PHYSTAT_XCVRSELECT  (1 << USBCTR_PHYSTAT_XCVRSELECT_Pos)
#define USBCTR_PHYSTAT_OPMODE      (1 << USBCTR_PHYSTAT_OPMODE_Pos)
#define USBCTR_PHYSTAT_EN_RPU      (1 << USBCTR_PHYSTAT_EN_RPU_Pos)
#define USBCTR_PHYSTAT_TX_LSFS_EN  (1 << USBCTR_PHYSTAT_TX_LSFS_EN_Pos)
#define USBCTR_PHYSTAT_RX_SE_EN    (1 << USBCTR_PHYSTAT_RX_SE_EN_Pos)
#define USBCTR_PHYSTAT_RX_LSFS_EN  (1 << USBCTR_PHYSTAT_RX_LSFS_EN_Pos)
#define USBCTR_PHYSTAT_RECEIVE_EN  (1 << USBCTR_PHYSTAT_RECEIVE_EN_Pos)
#define USBCTR_PHYSTAT_POL         (1 << USBCTR_PHYSTAT_POL_Pos)

struct ep_req_desc {
	volatile uint32_t ctrl;
	volatile uint32_t addr;
	volatile uint32_t next;
	volatile uint32_t reserved;
};

#define USBDC_EP_DESCR_CTRL_LENGTH_Pos 0
#define USBDC_EP_DESCR_CTRL_EN_Pos     13
#define USBDC_EP_DESCR_CTRL_NX_Pos     14
#define USBDC_EP_DESCR_CTRL_IE_Pos     15
#define USBDC_EP_DESCR_CTRL_SE_Pos     17

#define EP_DESCR_CTRL_LEN_MASK    (0x1FFF)

#define EP_DESCR_CTRL_LEN(len) \
			((len & EP_DESCR_CTRL_LEN_MASK) << USBDC_EP_DESCR_CTRL_LENGTH_Pos)
#define EP_DESCR_CTRL_EN       (1 << USBDC_EP_DESCR_CTRL_EN_Pos)
#define EP_DESCR_CTRL_NX       (1 << USBDC_EP_DESCR_CTRL_NX_Pos)
#define EP_DESCR_CTRL_IE       (1 << USBDC_EP_DESCR_CTRL_IE_Pos)
#define EP_DESCR_CTRL_SE       (1 << USBDC_EP_DESCR_CTRL_SE_Pos)

struct ep_status {
	uint32_t status;
	uint32_t total_length;
	uint32_t rem_length;
	uint32_t maxpacket;
	uint8_t is_used;
	uint16_t bInterval;
};

struct niiet_udc {
	struct usb_udc udc;

	struct usb_gadget_ep *eps[NIIET_UDC_EPS_COUNT * 2];
	struct usb_gadget_request *requests[NIIET_UDC_EPS_COUNT * 2];
	struct ep_status ep_info[NIIET_UDC_EPS_COUNT * 2];
	uint32_t ep0_data_len;

	struct ep_req_desc ep_req_desc[NIIET_UDC_EPS_COUNT * 2] __attribute__ ((aligned (4)));
	uint32_t dma_buf[NIIET_UDC_EPS_COUNT * 2][1024]; /* don't cached */

	uint32_t buf[1024 / 4];

	struct niiet_usbd_regs *regs;
	struct niiet_usbphy_regs *usbphy_regs;
	volatile uint32_t status;
	uint8_t addr;
};

extern void usb_phy_bconf_init(struct niiet_udc *niiet_udc) ;

extern void niiet_usb_desc_init(struct ep_req_desc *desc, uint32_t flags, int len);
extern void niiet_usb_desc_set_next(struct ep_req_desc *desc, struct ep_req_desc *next);
extern struct ep_req_desc *niiet_usb_desc_get_next(struct ep_req_desc *desc);
extern void niiet_usb_desc_set_buf(struct ep_req_desc *desc, void *buf);
extern void *niiet_usb_desc_get_buf(struct ep_req_desc *desc);
extern int niiet_usb_desc_avail(struct ep_req_desc *desc);

extern void niiet_udc_ep_prepare(struct niiet_usbd_regs *regs, int is_out, int num, int type, int max_len);
extern void niiet_usbd_ep_init(struct niiet_usbd_regs *regs, struct usb_gadget_ep *ep);
extern int niiet_udc_ep_type_to(int type);
extern void niiet_udc_ep_reset(struct niiet_usbd_regs *regs, int is_out, int num);

extern void niiet_udc_ep_activate(struct niiet_usbd_regs *regs, int is_out, int num);

extern void niiet_udc_ep_irq_en(struct niiet_usbd_regs *regs, int is_out, int num);
extern void *niiet_udc_ep_get_desc(struct niiet_usbd_regs *regs, int is_out, int num);

#endif /* _DRIVERS_USB_GADGET_UDC_NIIET_PRIV_H_ */
