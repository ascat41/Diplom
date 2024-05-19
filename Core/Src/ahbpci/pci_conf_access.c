#include "grpci2/grpci2api.h"

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>


#define PCI_MAX_DEVICES (21)

struct func {
    struct grpci2_pci_conf_space_regs* conf;
};

struct dev {
    struct func* funcs;
    int func_num;
};

static struct dev dev_conf[PCI_MAX_DEVICES] = { 0 };

void pci_conf_add_dev(uint32_t dev, uint32_t func_num) {
    dev_conf[dev].func_num = func_num;
    dev_conf[dev].funcs = malloc( func_num * sizeof(struct func) );
    for (int i = 0; i < func_num; i++) {
        dev_conf[dev].funcs[i].conf = malloc ( sizeof(struct grpci2_pci_conf_space_regs) );
    }
}

void pci_conf_map(uint32_t dev, uint32_t func, void* addr) {
    dev_conf[dev].funcs[func].conf->head = (struct grpci2_head_pci_conf_space_regs*)addr;
    dev_conf[dev].funcs[func].conf->ext = (struct grpci2_ext_pci_conf_space_regs*)(dev_conf[dev].funcs[func].conf->head + (grpci2_tw(dev_conf[dev].funcs[func].conf->head->cap_pointer) & 0xff));
}

struct grpci2_pci_conf_space_regs* get_pci_conf(uint32_t dev, uint32_t func) {
    if ((dev < PCI_MAX_DEVICES) && (dev_conf[dev].func_num > func)) {
        return dev_conf[dev].funcs[func].conf;
    }

    return NULL;
}
