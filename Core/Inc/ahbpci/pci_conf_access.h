#ifndef INC_AHBPCI_PCI_CONF_ACCESS_H_
#define INC_AHBPCI_PCI_CONF_ACCESS_H_

#include "grpci2/grpci2api.h"

#include <stdint.h>

void pci_conf_add_dev(uint32_t dev, uint32_t func_num);
void pci_conf_map(uint32_t dev, uint32_t func, void* addr);
struct grpci2_pci_conf_space_regs* get_pci_conf(uint32_t dev, uint32_t func);



#endif /* INC_AHBPCI_PCI_CONF_ACCESS_H_ */
