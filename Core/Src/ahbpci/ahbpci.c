#include "ahbpci/ahbpci.h"
#include "grpci2/grpci2api.h"
#include "ahbpci/pci_conf_access.h"

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#ifdef DEBUG
#define DBG(fmt, ...) \
		do { if (DEBUG) fprintf(stdout, fmt, __VA_ARGS__); } while (0)
#else
#define DBG(x...)
#endif


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

// private functions
static uint32_t* ahbpci_config_addr(uint8_t bus, uint8_t slot, uint8_t function);

// private variables
static uint32_t* const ahbpci_mem_ptr = (uint32_t*)0xE0000000;
static uint32_t* const ahbpci_io_ptr  = (uint32_t*)0xFFF80000;
static uint32_t* const ahbpci_cfg_ptr = (uint32_t*)0xFFF90000;

static uint32_t* const apb_regs_ptr   = (uint32_t*)0x80000400;


static inline int loadmem(int addr){
  int tmp;
  asm volatile (" lda [%1]1, %0 "
    : "=r"(tmp)
    : "r"(addr)
  );
  return tmp;
};

void ahbpci_host_init(void) {
	pci_conf_add_dev(0, 1);
	pci_conf_map(0, 0, ahbpci_cfg_ptr);

	struct grpci2_pci_conf_space_regs* conf0 = get_pci_conf(0, 0);

	grpci2_mst_enable(conf0);
	grpci2_mem_enable(conf0);
	grpci2_io_disable(conf0);
	grpci2_set_latency_timer(conf0, 0x20);
	grpci2_set_bar(conf0, 0, (unsigned int)ahbpci_mem_ptr); // PCI_ADDR
	grpci2_set_barmap(conf0, 0, 0);

	grpci2_set_mstmap((struct grpci2regs*)apb_regs_ptr, 0, (unsigned int)ahbpci_mem_ptr); // PCI_ADDR
}

void ahbpci_allocate_resources(void) {
	uint32_t* mem_addr;
	uint32_t* io_addr;
	uint32_t* conf_addr;
    uint32_t slot, numfuncs, func, id, pos, size, tmp, dev, fn;
    uint8_t header;
    int32_t bar;
    uint8_t space_flag;

    mem_addr = ahbpci_mem_ptr + 0x10000000;
    io_addr  = ahbpci_io_ptr;
    for(slot = 1; slot < PCI_MAX_DEVICES; slot++) {

    	ahbpci_config_read32(0, slot, 0, PCI_VENDOR, &id);

        if((id == 0xffffffff) || (id == 0) || ((id >> 16) == 0xFFFF)) {
            /*
             * This slot is empty
             */
            continue;
        }

        ahbpci_config_read8(0, slot, 0, PCI_HDRTYPE, &header);

        if(header & 0x80)	{
            numfuncs = PCI_MAX_FUNCTIONS;
        }
        else {
            numfuncs = 1;
        }

        pci_conf_add_dev(slot, numfuncs);

        for(func = 0; func < numfuncs; func++) {

        	ahbpci_config_read32(0, slot, func, PCI_VENDOR, &id);
            if(id == 0xffffffff || id == 0) {
                /*
                 * This slot is empty
                 */
                continue;
            }

            ahbpci_config_read32(0, slot, func, PCI_REVID, &tmp);
            tmp >>= 16;
            if (tmp == 0x0604) { // PCI_CLASS_BRIDGE_PCI
                continue;
            }

            for (pos = 0; pos < 6; pos++) {
            	ahbpci_config_write32(0, slot, func, PCI_BAR(pos), 0xffffffff);
            	ahbpci_config_read32(0, slot, func, PCI_BAR(pos), &size);

                if (size == 0 || size == 0xffffffff) {
                	continue;
                }

                space_flag = size & 0x01;

                if (space_flag) { // io space
                	size &= 0xfffffffc;
                } else {          // memory space
                	size &= 0xfffffff0;
                }

                size = ~size+1;
                bar = pos;
         	    dev = (slot * 8 + func) >> 3;
         	    fn  = (slot * 8 + func) & 7;

         	    ahbpci_config_write32(0, dev, fn, PCI_BAR(bar), space_flag ? (uint32_t)io_addr : (uint32_t)mem_addr);
         	    if (space_flag) io_addr  += size;
         	    else            mem_addr += size;

         	    DBG("Slot: %d, function: %d, bar%d size: %x\n", dev, fn, pos, size);
            }

			ahbpci_config_read32(0, slot, func, 0xC, &tmp);
			ahbpci_config_write32(0, slot, func, 0xC, tmp|0x4000);

			conf_addr = ahbpci_config_addr(0, slot, func);
			pci_conf_map(slot, func, conf_addr);
		}
	}
}

void ahbpci_init(void) {
	ahbpci_host_init();
	ahbpci_allocate_resources();
}

en_err_value ahbpci_config_read32(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset, uint32_t* val) {
    volatile uint32_t* pci_conf;

    if (offset & 0x03) return en_pcibios_bad_register_number;

    if (slot >= 21) {
        *val = 0xffffffff;
        return en_pcibios_successful;
    }

    pci_conf = (uint32_t*)((uint32_t)ahbpci_cfg_ptr | ((slot << 11) | (function << 8) | offset));

    *val = *pci_conf;
    *val = grpci2_tw(*val);

    // PCI config access error
    if (((struct grpci2regs*)apb_regs_ptr)->status & (1 << 19)) {
        *val = 0xffffffff;
    }

    DBG("pci_read - bus: %d, dev: %d, fn: %d, off: 0x%x => addr: %x, val: %x\n", bus, slot, function, offset,
    		(uint32_t)ahbpci_cfg_ptr | (slot << 11) | (function << 8) | (offset), *val);

    return en_pcibios_successful;
}

en_err_value ahbpci_config_read16(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset, uint16_t* val) {
    uint32_t v;

    if (offset & 0x01) return en_pcibios_bad_register_number;

    ahbpci_config_read32(bus, slot, function, offset & ~0x03, &v);
    *val = 0xffff & (v >> (8 * (offset & 0x03)));

    return en_pcibios_successful;
}

en_err_value ahbpci_config_read8(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset, uint8_t* val) {
	uint32_t v;

    ahbpci_config_read32(bus, slot, function, offset & ~0x03, &v);

    *val = 0xff & (v >> (8 * (offset & 0x03)));

    return en_pcibios_successful;
}

en_err_value ahbpci_config_write32(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset, uint32_t val) {
    volatile uint32_t* pci_conf;

    if ((offset & 0x03) || bus != 0) return en_pcibios_bad_register_number;


    pci_conf = (uint32_t*)((uint32_t)ahbpci_cfg_ptr | ((slot << 11) | (function << 8) | (offset & ~0x03)));

    val = grpci2_tw(val);
    *pci_conf = val;

    DBG("pci write - bus: %d, dev: %d, fn: %d, off: 0x%x => addr: %x, val: %x\n", bus, slot, function, offset,
    		(uint32_t)ahbpci_cfg_ptr | (slot << 11) | (function << 8) | (offset), val);

    return en_pcibios_successful;
}

en_err_value ahbpci_config_write16(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset, uint16_t val) {
    uint32_t v;

    if (offset & 0x01) return en_pcibios_bad_register_number;

    ahbpci_config_read32(bus, slot, function, offset & ~0x03, &v);

    v = (v & ~(0xffff << (8 * (offset & 0x03)))) | ((0xffff & val) << (8 * (offset & 0x03)));

    return ahbpci_config_write32(bus, slot, function, offset & ~0x03, v);
}

en_err_value ahbpci_config_write8(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset, uint8_t val) {
	uint32_t v;

	ahbpci_config_read32(bus, slot, function, offset & ~0x03, &v);

    v = (v & ~(0xff << (8 * (offset & 0x03)))) | ((0xff & val) << (8 * (offset & 0x03)));

    return ahbpci_config_write32(bus, slot, function, offset & ~0x03, v);
}

static uint32_t* ahbpci_config_addr(uint8_t bus, uint8_t slot, uint8_t function) {
	return (uint32_t*)((uint32_t)ahbpci_cfg_ptr | ((slot << 11) | (function << 8)));
}

void ahbpci_loopback_test(uint8_t reset) {
  int i;
  struct grpci2regs *apb;

  uint32_t base_addr = (uint32_t)ahbpci_get_mem_ptr();
  uint32_t conf_addr = (uint32_t)ahbpci_get_cfg_ptr();
  uint32_t apb_addr = (uint32_t)ahbpci_get_apb_ptr();
  uint32_t pci_addr = (uint32_t)ahbpci_get_mem_ptr();

  volatile uint32_t* ahbmem;
  volatile uint32_t* pcimem;
  volatile unsigned int* chbase;
  volatile struct grpci2_dma_data_desc* ddesc1;
  volatile struct grpci2_dma_data_desc* ddesc2;
  volatile struct grpci2_dma_data_desc* ddesc3;

  /* Конфигурация хоста ************************************************************
   *  BAR[0] = PCI_ADDR
   *  BAR[0] => AHB AHBMEM
   * ***************************************************************************** */
  apb = (struct grpci2regs*)apb_addr;
  struct grpci2_pci_conf_space_regs* conf0 = get_pci_conf(0, 0);

  DBG("Loopback test run!\n");

  // Reset
  if (reset != 0) {
    apb->ctrl = GRPCI2_CTRL_RST;
    if ((apb->ctrl & GRPCI2_CTRL_RST) == 0) DBG("Loopback test error: pci reset fail\n");
    apb->ctrl = 0x00000000;
    if ((apb->ctrl & GRPCI2_CTRL_RST) != 0) DBG("Loopback test error: pci ctrl reg reset fail\n");
  }

  /* Get BASE_ADDR_MASK */
  grpci2_set_mstmap(apb, 0, 0xffffffff);
  uint32_t base_addr_mask = grpci2_get_mstmap(apb, 0);
  grpci2_set_bar(conf0, 0, 0xffffffff);
  uint32_t pci_addr_mask = grpci2_tw(grpci2_get_bar(conf0, 0));

  ahbmem = malloc(sizeof(int)*128);
  pcimem = (volatile uint32_t*)((base_addr & base_addr_mask) | (((pci_addr & pci_addr_mask) | ((uint32_t)(ahbmem) &~ pci_addr_mask)) &~ base_addr_mask));

  DBG("ahbmem = 0x%x\n", (uint32_t)ahbmem);
  DBG("pcimem = 0x%x\n", (uint32_t)pcimem);

  grpci2_mst_enable(conf0);
  grpci2_mem_enable(conf0);
  grpci2_io_disable(conf0);
  grpci2_set_latency_timer(conf0, 0x20);
  grpci2_set_bar(conf0, 0, pci_addr);
  grpci2_set_barmap(conf0, 0, ((unsigned int)ahbmem) & 0xfffffff0);

  grpci2_set_mstmap(apb, 0, pci_addr);

  /* PCI-AHB loopback test */

  /* Clear memory */
  for (i=0; i<32; i++){
    *(ahbmem + i) = 0;
  }

  /* Write to memory via PCI*/
  for (i=0; i<32; i++){
    *(pcimem + i) = i;
  }

  /* Check memory */
  for (i=0; i<32; i++){
    if (loadmem((unsigned int)(ahbmem + i)) != i) DBG("Loopback test error: ahb memory check fail\n");
  }

  /* Write to memory*/
  for (i=0; i<32; i++){
    *(ahbmem + i) = 31-i;
  }

  /* Check memory via PCI*/
  for (i=0; i<32; i++){
    if (loadmem((unsigned int)(pcimem + i)) != 31-i) DBG("Loopback test error: pci memory check fail\n");
  }

  if (grpci2_get_dma(apb)){

    /* PCI-DMA loopback test */

    for (i=0; i<32; i++){
      *(ahbmem + i) = i;
    }

    for (i=32; i<128; i++){
      *(ahbmem + i) = 0xafafafaf;
    }

    /* DMA descriptor table setup */
    grpci2_dma_desc_init(apb, &chbase, 0, 2, 2);

    ddesc1 = (struct grpci2_dma_data_desc*)*(chbase + 2);
    ddesc2 = (struct grpci2_dma_data_desc*)*(chbase + 6);
    ddesc3 = (struct grpci2_dma_data_desc*)ddesc2->next;

    /* Setup AHB to PCI transfer 32 words */
    grpci2_dma_add(apb, (volatile unsigned int**)&ddesc1, (pci_addr & pci_addr_mask) | ((unsigned int)(ahbmem + 32) &~ pci_addr_mask),
    		                                              (unsigned int)ahbmem,
								                          1, 0, 32);

    /* Setup PCI to AHB transfer 16 words */
    grpci2_dma_add(apb, (volatile unsigned int**)&ddesc2, (pci_addr & pci_addr_mask) | ((unsigned int)(ahbmem + 32) &~ pci_addr_mask),
    		                                              (unsigned int)(ahbmem + 64),
								                          0, 0, 16);

    /* Setup AHB to PCI transfer 1 word */
    grpci2_dma_add(apb, (volatile unsigned int**)&ddesc3, (pci_addr & pci_addr_mask) | ((unsigned int)(ahbmem + 96) &~ pci_addr_mask),
    		                                              (unsigned int)(ahbmem + 64),
								                          1, 0, 1);

    /* Wait on desc1 done*/
    while((loadmem((unsigned int)&ddesc1->ctrl) & GRPCI2_DMA_DESC_EN) != 0);
    for (i=0; i<32; i++){
      if (loadmem((unsigned int)(ahbmem + 32 + i)) != i) DBG("Loopback test error: dma test 1 failed\n");
    }

    /* Wait on desc2 done*/
    while((loadmem((unsigned int)&ddesc2->ctrl) & GRPCI2_DMA_DESC_EN) != 0);
    for (i=0; i<16; i++){
      if (loadmem((unsigned int)(ahbmem + 64 + i)) != i) DBG("Loopback test error: dma test 2 failed\n");
    }

    /* Wait on desc3 done*/
    while((loadmem((unsigned int)&ddesc3->ctrl) & GRPCI2_DMA_DESC_EN) != 0);
    for (i=0; i<1; i++){
      if (loadmem((unsigned int)(ahbmem + 96 + i)) != i) DBG("Loopback test error: dma test 3 failed\n");
    }

  }

  DBG("Loopback test passed!");
}

uint32_t* ahbpci_get_apb_ptr(void) {
	return apb_regs_ptr;
}

uint32_t* ahbpci_get_mem_ptr(void) {
	return ahbpci_mem_ptr;
}

uint32_t* ahbpci_get_io_ptr(void) {
	return ahbpci_io_ptr;
}

uint32_t* ahbpci_get_cfg_ptr(void) {
	return ahbpci_cfg_ptr;
}
