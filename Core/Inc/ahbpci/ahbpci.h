#ifndef INC_AHBPCI_AHBPCI_H_
#define INC_AHBPCI_AHBPCI_H_

#include "grpci2/grpci2api.h"
#include "pci_conf_access.h"

#include <stdint.h>

#define PCI_MAX_DEVICES			(21)
#define PCI_MAX_FUNCTIONS		 (8)

// Error values that may be returned by the PCI bios.
typedef enum {
	en_pcibios_successful          = 0x00,
	en_pcibios_func_not_supported  = 0x81,
	en_pcibios_bad_vendor_id       = 0x83,
	en_pcibios_device_not_found    = 0x86,
	en_pcibios_bad_register_number = 0x87,
	en_pcibios_set_faile           = 0x88,
	en_pcibios_buffer_too_small    = 0x89
} en_err_value;


uint32_t* ahbpci_get_apb_ptr(void);
uint32_t* ahbpci_get_mem_ptr(void);
uint32_t* ahbpci_get_io_ptr(void);
uint32_t* ahbpci_get_cfg_ptr(void);

void ahbpci_host_init(void);
void ahbpci_allocate_resources(void);
void ahbpci_init(void);
en_err_value ahbpci_config_read32(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset, uint32_t* val);
en_err_value ahbpci_config_read16(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset, uint16_t* val);
en_err_value ahbpci_config_read8(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset, uint8_t* val);
en_err_value ahbpci_config_write32(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset, uint32_t val);
en_err_value ahbpci_config_write16(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset, uint16_t val);
en_err_value ahbpci_config_write8(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset, uint8_t val);

void ahbpci_loopback_test(uint8_t reset);

void ahbpci_enable_interrupts();
void ahbpci_disable_interrupts();
void __attribute__((weak)) ahbpci_serr_handler();
void __attribute__((weak)) ahbpci_dma_handler();
void __attribute__((weak)) ahbpci_dma_error_handler();
void __attribute__((weak)) ahbpci_undefined_handler();

#endif /* INC_AHBPCI_AHBPCI_H_ */
