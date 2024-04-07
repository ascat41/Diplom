#include <plug_and_play.h>
#include <stdio.h>
#include <stdint.h>

// Private functions prototypes ----------------------------------------------/
//static uint32_t callback(void *info, uint32_t vendor, uint32_t device, uint32_t type, uint32_t depth, void *arg);

// Public functions ----------------------------------------------------------/
uint32_t get_GPTIMER() {
	const uint32_t ioarea = 0xFFFFF000;
	const uint32_t maxdepth = 4;
	uint32_t ret = 0;
	ret = ambapp_visit(
			ioarea,
			VENDOR_GAISLER,
			GAISLER_GPTIMER,
			AMBAPP_VISIT_APBSLAVE,
			maxdepth,
			ambapp_findfirst_fn,
			NULL
			);
	return ret;
}

uint32_t get_GRGPIO() {
	const uint32_t ioarea = 0xFFFFF000;
	const uint32_t maxdepth = 4;
	uint32_t ret = 0;
	ret = ambapp_visit(
			ioarea,
			VENDOR_GAISLER,
			GAISLER_GPIO,
			AMBAPP_VISIT_APBSLAVE,
			maxdepth,
			ambapp_findfirst_fn,
			NULL
			);
	return ret;
}

uint32_t get_IRQMP() {
	const uint32_t ioarea = 0xFFFFF000;
	const uint32_t maxdepth = 4;
	uint32_t ret = 0;
	ret = ambapp_visit(
			ioarea,
			VENDOR_GAISLER,
			GAISLER_IRQMP,
			AMBAPP_VISIT_APBSLAVE,
			maxdepth,
			ambapp_findfirst_fn,
			NULL
			);
	return ret;
}

uint32_t get_GRPCI2() {
	const uint32_t ioarea = 0xFFFFF000;
	const uint32_t maxdepth = 4;
	uint32_t ret = 0;
	ret = ambapp_visit(
			ioarea,
			VENDOR_GAISLER,
			GAISLER_GRPCI2,
			AMBAPP_VISIT_APBSLAVE,
			maxdepth,
			ambapp_findfirst_fn,
			NULL
			);
	return ret;
}

uint32_t get_GRPCI2_AHB() {
	const uint32_t ioarea = 0xFFFFF000;
	const uint32_t maxdepth = 4;
	uint32_t ret = 0;
	ret = ambapp_visit(
			ioarea,
			VENDOR_GAISLER,
			GAISLER_GRPCI2,
			AMBAPP_VISIT_AHBSLAVE,
			maxdepth,
			ambapp_findfirst_fn,
			NULL
			);
	return ret;
}

// Private functions ---------------------------------------------------------/
//static uint32_t callback(void *info, uint32_t vendor, uint32_t device, uint32_t type, uint32_t depth, void *arg)
//{
//	struct amba_apb_info* apbi = info;
//	if (type != AMBAPP_VISIT_APBSLAVE) {
//		printf("Unexpected type=%u\n", type);
//		return 0;
//	}
//	printf("vendor=%x, device=%x, type=%x, depth=%u\n", vendor, device, type, depth);
//	printf("ver=%u, irq=%u, start=%08x, mask=%08x\n", apbi->ver, apbi->irq, apbi->start, apbi->mask);
//	return apbi->start;
//}
