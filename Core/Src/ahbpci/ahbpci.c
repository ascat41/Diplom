#include "ahbpci/ahbpci.h"
#include "grpci2.h"

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

// private macro
#define HOST      (0)
#define HOST_FUNC (0)

#define AHBPCI_MEM_OFFSET    (0x00000000)
#define AHBPCI_IO_OFFSET     (0x1FF80000)
#define AHBPCI_CFG_OFFSET    (AHBPCI_IO_OFFSET + 0x10000)

#define CAP_CTRL_OFS     (0x00)
#define CAP_BAR_OFS      (0x04)
#define CAP_IOMAP_OFS    (0x20)
#define CAP_BARSIZE_OFS  (0x24)
#define CAP_AHBPREF_OFS  (0x3C)

#define	PCI_VENDOR	     (0x00)
#define	PCI_DEVICE	     (0x02)
#define	PCI_COMMAND	     (0x04)
#define	PCI_STATUS	     (0x06)
#define	PCI_REVID	     (0x08)
#define	PCI_PROGIF	     (0x09)
#define	PCI_SUBCLASS	 (0x0a)
#define	PCI_CLASS	     (0x0b)
#define	PCI_CACHELNSZ	 (0x0c)
#define	PCI_LATTIMER	 (0x0d)
#define	PCI_HDRTYPE	     (0x0e)
#define	PCI_BIST	     (0x0f)

#define	PCI_BARS	     (0x10)
#define	PCI_BAR(x)		 (PCI_BARS + (x) * 4)
#define	PCI_CIS	         (0x28)
#define	PCI_SUBVEND_0	 (0x2c)
#define	PCI_SUBDEV_0	 (0x2e)
#define	PCI_BIOS	     (0x30)
#define	PCI_CAP_PTR	     (0x34)
#define	PCI_INTLINE	     (0x3c)
#define	PCI_INTPIN	     (0x3d)
#define	PCI_MINGNT	     (0x3e)
#define	PCI_MAXLAT	     (0x3f)

// private variables
static uint32_t const* ahbpci_mem_ptr = (uint32_t*)0xE0000000;
static uint32_t const* ahbpci_io_ptr  = (uint32_t*)0xFFF80000;
static uint32_t const* ahbpci_cfg_ptr = (uint32_t*)0xFFF90000;
//static struct ahbpci_pcibar_cfg* _pcibar_cfg = { 0 };

// public functions
void ahbpci_memspace_init(uint32_t const* ahbpci) {
	ahbpci_mem_ptr = ahbpci + AHBPCI_MEM_OFFSET;
	ahbpci_io_ptr  = ahbpci + AHBPCI_IO_OFFSET;
	ahbpci_cfg_ptr = ahbpci + AHBPCI_CFG_OFFSET;
}

void ahbpci_init(GRPCI2_REGS_TypeDef* regs/*, struct ahbpci_pcibar_cfg* pcibar_cfg*/) {
//	uint8_t capptr = 0;
//	uint32_t io_map = 0;
//	uint32_t size, pciadr, ahbadr = 0;
	uint32_t data = 0;
//	_pcibar_cfg = pcibar_cfg;

//	// Отображение доступов AHB в доступы PCI для области ввода-вывода PCI
//	regs->IO_MAP = (uint32_t)ahbpci_io_ptr;
//
//	// Отображение адреса доступа ведущего устройства AHB в адрес доступа к области памяти PCI
//	for (uint32_t i = 0; i < 16; i++) {
//		regs->AHBMST_MAP[i] = (uint32_t)ahbpci_mem_ptr;
//	}
//
//	// Получить указатель на capability
//	capptr = ahbpci_config_read8(HOST, HOST_FUNC, PCI_CAP_PTR);
//	printf("ahbpci_init: capptr = %" PRIx8 "\n", capptr);
//
//	// Разрешить перестановку байт
//	io_map = ahbpci_config_read32(HOST, HOST_FUNC, capptr + CAP_IOMAP_OFS);
//	printf("ahbpci_init: io_map = %" PRIx32 "\n", io_map);
//	io_map = (io_map & ~0x1) | 1;
//	ahbpci_config_write32(HOST, HOST_FUNC, capptr + CAP_IOMAP_OFS, io_map);

	// Установить BAR'ы для хоста, чтобы дать доступ (DMA) другим устройствам
//	for (uint32_t i = 0; i < 6; i++) {
//		size = ~(pcibar_cfg[i].barsize-1);
//		pcibar_cfg[i].pciadr &= size;
//		pcibar_cfg[i].ahbadr &= size;
//
//		pciadr = pcibar_cfg[i].pciadr;
//		ahbadr = pcibar_cfg[i].ahbadr;
//
//		ahbpci_config_write32(HOST, HOST_FUNC, capptr + CAP_BARSIZE_OFS + i * 4, size);
//		ahbpci_config_write32(HOST, HOST_FUNC, capptr + CAP_BAR_OFS + i * 4, ahbadr);
//		ahbpci_config_write32(HOST, HOST_FUNC, PCI_BAR(0) + i * 4, pciadr);
//	}

	// Установить как мастера на шине и разрешить запись в PCI память
	data = ahbpci_config_read32(HOST, HOST_FUNC, PCI_COMMAND);
	printf("ahbpci_init: data = %" PRIx32 "\n", data);
	data |= (0x02 | 0x04);
	ahbpci_config_write32(HOST, HOST_FUNC, PCI_COMMAND, data);
}

uint32_t ahbpci_config_read32(uint8_t dev, uint8_t func, uint8_t ofs) {
	uint32_t* address = NULL;
	uint32_t value = 0;
	uint8_t reg = ofs & ~0x03;

	address = (uint32_t*)( (uint32_t)ahbpci_cfg_ptr | (dev << 11) | (func << 8) | (reg << 2) );
	value = *address;
	return value;
}

uint16_t ahbpci_config_read16(uint8_t dev, uint8_t func, uint8_t ofs) {
	uint32_t value = ahbpci_config_read32(dev, func, ofs);
	return (uint16_t)((value >> ((ofs & ~0xfffC) * 8)) & 0x0000ffff);
}

uint8_t ahbpci_config_read8(uint8_t dev, uint8_t func, uint8_t ofs) {
	uint32_t value = ahbpci_config_read32(dev, func, ofs);
	return (uint8_t)((value >> ((ofs & ~0xfffC) * 8)) & 0x000000ff);
}

void ahbpci_config_write32(uint8_t dev, uint8_t func, uint8_t ofs, uint32_t value) {
	uint32_t* address = NULL;
	uint8_t reg = ofs & ~0x0003;
	printf("ahbpci_config_write32: ofs = %" PRIx8 ", reg = %" PRIx16 "\n", ofs, reg);

	address = (uint32_t*)( (uint32_t)ahbpci_cfg_ptr | (dev << 11) | (func << 8) | (reg << 2) );
	*address = value;
}

void ahbpci_config_write16(uint8_t dev, uint8_t func, uint8_t ofs, uint16_t value) {
	uint32_t tmp = value << ((ofs & 0x0002) * 8);
	ahbpci_config_write32(dev, func, ofs, tmp);
}

void ahbpci_config_write8(uint8_t dev, uint8_t func, uint8_t ofs, uint8_t value) {
	uint32_t tmp = value << ((ofs & 0x0003) * 8);
	ahbpci_config_write32(dev, func, ofs, tmp);
}

uint32_t ahbpci_read_config(uint8_t dev, uint8_t func, uint8_t reg) {
	uint32_t* address = NULL;
	uint32_t value = 0;

	address = (uint32_t*)( (uint32_t)ahbpci_cfg_ptr | (dev << 11) | (func << 8) | (reg << 2) );
	value = *address;
	return value;
}
