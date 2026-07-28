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

#include <util/macro.h>

#include <util/math.h>

#include "k1921vg1t_udc_priv.h"

void niiet_usb_desc_init(struct ep_req_desc *desc, uint32_t flags, int len) {
	desc->ctrl = 0;
	desc->ctrl = flags | EP_DESCR_CTRL_LEN(len);
}

void niiet_usb_desc_set_next(struct ep_req_desc *desc, struct ep_req_desc *next) {
	desc->next = (uintptr_t)next;
}

struct ep_req_desc *niiet_usb_desc_get_next(struct ep_req_desc *desc) {
	return (void *)(uintptr_t)desc->next;
}


int niiet_usb_desc_avail(struct ep_req_desc *desc) {
	return desc->ctrl & EP_DESCR_CTRL_EN;
}

void niiet_usb_desc_set_buf(struct ep_req_desc *desc, void *buf) {
	desc->addr = (uintptr_t)buf;
}

void *niiet_usb_desc_get_buf(struct ep_req_desc *desc) {
	return (void *)(uintptr_t)desc->addr;
}
