#ifndef INC_AHBPCI_AHBPCI_H_
#define INC_AHBPCI_AHBPCI_H_

#include "grpci2.h"

#include <stdint.h>

struct ahbpci_pcibar_cfg {
	unsigned int pciadr;	// PCI адрес BAR'а (содержимое BAR'а)
	unsigned int ahbadr;	// pciadr переводится в этот AHB адрес
	unsigned int barsize;	// PCI BAR size, степень 2
};

void ahbpci_memspace_init(uint32_t const* ahbpci);
void ahbpci_init(GRPCI2_REGS_TypeDef* regs/*, struct ahbpci_pcibar_cfg* pcibar_cfg*/);
uint32_t ahbpci_config_read32(uint8_t dev, uint8_t func, uint8_t ofs);
uint16_t ahbpci_config_read16(uint8_t dev, uint8_t func, uint8_t ofs);
uint8_t ahbpci_config_read8(uint8_t dev, uint8_t func, uint8_t ofs);
void ahbpci_config_write32(uint8_t dev, uint8_t func, uint8_t ofs, uint32_t value);
void ahbpci_config_write16(uint8_t dev, uint8_t func, uint8_t ofs, uint16_t value);
void ahbpci_config_write8(uint8_t dev, uint8_t func, uint8_t ofs, uint8_t value);

uint32_t ahbpci_read_config(uint8_t dev, uint8_t func, uint8_t reg);


#endif /* INC_AHBPCI_AHBPCI_H_ */
