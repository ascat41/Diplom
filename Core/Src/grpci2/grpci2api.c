#include "grpci2/grpci2api.h"
#include <stdlib.h>

static inline int loadmem(int addr){
  int tmp;        
  asm volatile (" lda [%1]1, %0 "
    : "=r"(tmp)
    : "r"(addr)
  );
  return tmp;
};
  
unsigned int grpci2_tw(unsigned int data){
  return ((data & 0xff) << 24) | (((data >> 8) & 0xff) << 16) | (((data >> 16) & 0xff) << 8) | (((data >> 24) & 0xff)); 
}

/* GRPCI2 Get Master/Target/DMA enabled ****************************************** */
  int grpci2_get_master(volatile struct grpci2regs* apb){
    return ((apb->status & GRPCI2_STA_MASTER) != 0);
  };

  int grpci2_get_target(volatile struct grpci2regs* apb){
    return ((apb->status & GRPCI2_STA_TARGET) != 0);
  };

  int grpci2_get_dma(volatile struct grpci2regs* apb){
    return ((apb->status & GRPCI2_STA_DMA) != 0);
  };

/* GRPCI2 Set/Get AHB Master-to-PCI map ****************************************** */
  void grpci2_set_mstmap(volatile struct grpci2regs* apb, int mst, unsigned int addr){
    apb->mst_to_pci[mst] = addr;
  };
  
  unsigned int grpci2_get_mstmap(volatile struct grpci2regs* apb, int mst){
    return apb->mst_to_pci[mst];
  };

/* GRPCI2 Set/Get Bus-endiannes ************************************************** */
  void grpci2_set_bus_litle_endian(volatile struct grpci2_pci_conf_space_regs* conf){
    conf->ext->ahbio_endiannes |= grpci2_tw(1);
  };
  
  void grpci2_set_bus_big_endian(volatile struct grpci2_pci_conf_space_regs* conf){
    conf->ext->ahbio_endiannes &= grpci2_tw(0xfffffffe);
  };
  
  int grpci2_get_endian(volatile struct grpci2_pci_conf_space_regs* conf){
    return (grpci2_tw(conf->ext->ahbio_endiannes) & 1);
  };

/* GRPCI2 Set/Get BARMAP ********************************************************* */
  void grpci2_set_barmap(volatile struct grpci2_pci_conf_space_regs* conf, int bar, unsigned int addr){
    conf->ext->bar_to_ahb[bar] = grpci2_tw(addr);
  };
  
  unsigned int grpci2_get_barmap(volatile struct grpci2_pci_conf_space_regs* conf, int bar){
    return conf->ext->bar_to_ahb[bar];
  };

/* GRPCI2 Set/Get BAR ************************************************************ */
  void grpci2_set_bar(volatile struct grpci2_pci_conf_space_regs* conf, int bar, unsigned int addr){
    conf->head->bar[bar] = grpci2_tw(addr);
  };
  
  unsigned int grpci2_get_bar(volatile struct grpci2_pci_conf_space_regs* conf, int bar){
    return conf->head->bar[bar];
  };

/* GRPCI2 Set Latency Timer ****************************************************** */
  void grpci2_set_latency_timer(volatile struct grpci2_pci_conf_space_regs* conf, int timer){
    conf->head->lat_timer = grpci2_tw((grpci2_tw(conf->head->lat_timer) & 0xffff00ff) | (timer & 0xff) << 8);
  };

/* GRPCI2 Master Enable/Disable ************************************************** */
  void grpci2_mst_enable(volatile struct grpci2_pci_conf_space_regs* conf){
    conf->head->sta_cmd |= grpci2_tw(0x4);
  };
  
  void grpci2_mst_disable(volatile struct grpci2_pci_conf_space_regs* conf){
    conf->head->sta_cmd &= ~grpci2_tw(0x4);
  };
  
/* GRPCI2 Memory target Enable/Disable ******************************************* */
  void grpci2_mem_enable(volatile struct grpci2_pci_conf_space_regs* conf){
    conf->head->sta_cmd |= grpci2_tw(0x2);
  };
  
  void grpci2_mem_disable(volatile struct grpci2_pci_conf_space_regs* conf){
    conf->head->sta_cmd &= ~grpci2_tw(0x2);
  };
  
/* GRPCI2 IO target Enable/Disable *********************************************** */
  void grpci2_io_enable(volatile struct grpci2_pci_conf_space_regs* conf){
    conf->head->sta_cmd |= grpci2_tw(0x1);
  };
  
  void grpci2_io_disable(volatile struct grpci2_pci_conf_space_regs* conf){
    conf->head->sta_cmd &= ~grpci2_tw(0x1);
  };
  
/* GRPCI2 DMA Descriptors init *************************************************** */
  int grpci2_dma_desc_init(struct grpci2regs* apb, volatile unsigned int **chdesc, int irqen, 
                           int num_ch, int ch_entry){
    int i,j;
    struct grpci2_dma_ch_desc *ch = malloc(sizeof(struct grpci2_dma_ch_desc)*(num_ch+1));
    struct grpci2_dma_data_desc *ddesc = malloc(sizeof(struct grpci2_dma_data_desc)*(ch_entry*num_ch+1));
    /*printf("ch: %p, desc: %p\n", ch, ddesc);*/
    if (((int)ch & 0xf) != 0) ch = (struct grpci2_dma_ch_desc*)((int)ch + (0x10 - (int)ch & 0xf));
    if (((int)ddesc & 0xf) != 0) ddesc = (struct grpci2_dma_data_desc*)((int)ddesc + (0x10 - (int)ddesc & 0xf));
    /*printf("ch: %p, desc: %p\n", ch, ddesc);*/

    *chdesc = (int*)ch;
    /*printf("ch base: %08p\n", ch);*/

    for(i=0; i<num_ch; i++){
      for(j=0; j<ch_entry; j++){
        (ddesc + i*ch_entry + j)->ctrl = 0;
        (ddesc + i*ch_entry + j)->paddr = 0;
        (ddesc + i*ch_entry + j)->aaddr = 0;
        if (j == ch_entry-1) (ddesc + i*ch_entry + j)->next = (int)(ddesc + i*ch_entry);
        else (ddesc + i*ch_entry + j)->next = (int)(ddesc + i*ch_entry + j + 1);
      }
      
      (ch + i)->ctrl = GRPCI2_DMA_CHDESC_EN | 
                       ((i << GRPCI2_DMA_CHDESC_ID)& GRPCI2_DMA_CHDESC_ID_MASK) | 
                       GRPCI2_DMA_DESC_TYPE_CH;
      if (i == num_ch-1) (ch + i)->next = (int)ch;
      else (ch + i)->next = (int)(ch + i + 1);
      (ch + i)->desc = (int)(ddesc + i*ch_entry);
      (ch + i)->res = 0;

      //desc[i] = (int*)(ddesc + i*CH_ENTRY);
      //printf("datadesc[%d] base: %08p\n", i, desc[i]);
    };

    apb->dma_ctrl = GRPCI2_DMACTRL_GUARD | GRPCI2_DMACTRL_MABORT | GRPCI2_DMACTRL_TABORT | 
                    GRPCI2_DMACTRL_PERR | GRPCI2_DMACTRL_AHBDATA_ERR | GRPCI2_DMACTRL_AHBDESC_ERR |
                    (((num_ch-1) << GRPCI2_DMACTRL_NUMCH) & GRPCI2_DMACTRL_NUMCH_MASK) |
                    (irqen*GRPCI2_DMACTRL_IRQEN);
    apb->dma_desc = (int)ch;

    apb->status = GRPCI2_STA_IRQ_DMAINT | GRPCI2_STA_IRQ_DMAERR;
    
  };
/* ******************************************************************************* */

/* GRPCI2 DMA add transfer ******************************************************* */
  int grpci2_dma_add(struct grpci2regs* apb, volatile unsigned int **ddesc, unsigned int paddr, 
                     unsigned int aaddr, int dir, int ien, int length){
    struct grpci2_dma_data_desc *desc = (struct grpci2_dma_data_desc*)*ddesc;
    unsigned int tmp = loadmem((int)desc); 
        
    /*printf("desc: %08X, ctrl: %08X, paddr: %08X, aaddr: %08X, next: %08X\n", 
           (desc), (desc)->ctrl, (desc)->paddr, (desc)->aaddr, (desc)->next); /**/
  
    //if ((desc->ctrl & GRPCI2_DMA_DESC_EN) == 0){
    //printf("desc-ctrl: 0x%08X 0x%08X\n", tmp, desc->ctrl);
    if ((tmp & GRPCI2_DMA_DESC_EN) == 0){
      desc->paddr = paddr;
      desc->aaddr = aaddr;
      desc->ctrl = GRPCI2_DMA_DESC_EN | GRPCI2_DMA_DESC_IRQEN*ien | GRPCI2_DMA_DESC_DIR*dir |
                   GRPCI2_DMA_DESC_TYPE_DATA | 
                   (length & GRPCI2_DMA_DESC_LENGTH_MASK) << GRPCI2_DMA_DESC_LENGTH;
      *ddesc = (int*)desc->next;
      apb->dma_ctrl = (apb->dma_ctrl & GRPCI2_DMACTRL_NUMCH_MASK) | 
                      (apb->dma_ctrl & GRPCI2_DMACTRL_IRQEN) | GRPCI2_DMACTRL_EN;
    
    /*printf("desc: %08X, ctrl: %08X, paddr: %08X, aaddr: %08X, next: %08X\n", 
           (desc), (desc)->ctrl, (desc)->paddr, (desc)->aaddr, (desc)->next); /**/
      return 0;
    }else{
      return -1;
    };

  };
/* ******************************************************************************* */

/* GRPCI2 DMA check transfer ******************************************************* */
  unsigned int grpci2_dma_check(volatile unsigned int **ddesc){
    struct grpci2_dma_data_desc *desc = (struct grpci2_dma_data_desc*)*ddesc;
    unsigned int res;

    res = loadmem((int)desc);
    
    if (!(res & GRPCI2_DMA_DESC_EN)) {
      *ddesc = (int*)desc->next;
    };
    return res;
  };
  /* ******************************************************************************* */

  int grpci2_loopback_test(unsigned int base_addr, unsigned int conf_addr, unsigned int apb_addr,
                           unsigned int pci_addr, int reset) {
    int i;
    struct grpci2regs *apb;
    struct grpci2_pci_conf_space_regs conf_host;
    struct grpci2_pci_conf_space_regs *conf[1];

    volatile unsigned int* ahbmem;
    volatile unsigned int* pcimem;
    volatile unsigned int* chbase;
    volatile struct grpci2_dma_data_desc* ddesc1;
    volatile struct grpci2_dma_data_desc* ddesc2;
    volatile struct grpci2_dma_data_desc* ddesc3;

//    report_device(0x0107C000);

    /* PCI core configuration ******************************************************
     *  BAR[0] = PCI_ADDR
     *  BAR[0] => AHB AHBMEM
     * ***************************************************************************** */
    apb = (struct grpci2regs*)apb_addr;
    conf[0] = (struct grpci2_pci_conf_space_regs*)&conf_host;
    conf[0]->head = (struct grpci2_head_pci_conf_space_regs*)conf_addr;
    conf[0]->ext = (struct grpci2_ext_pci_conf_space_regs*)((unsigned int)conf[0]->head +
                   (grpci2_tw(conf[0]->head->cap_pointer) & 0xff));

    // Software reset
//    report_subtest(7);
    if (reset != 0) {
      apb->ctrl = GRPCI2_CTRL_RST;
      if ((apb->ctrl & GRPCI2_CTRL_RST) == 0) /*fail(1);*/ return 1;
      apb->ctrl = 0x00000000;
      if ((apb->ctrl & GRPCI2_CTRL_RST) != 0) /*fail(2);*/ return 2;
    };

    /* Get BASE_ADDR_MASK */
    grpci2_set_mstmap(apb, 0, 0xffffffff);
    unsigned int base_addr_mask = grpci2_get_mstmap(apb, 0);
    grpci2_set_bar(conf[0], 0, 0xffffffff);
    unsigned int pci_addr_mask = grpci2_tw(grpci2_get_bar(conf[0], 0));

    ahbmem = malloc(sizeof(int)*128);
    pcimem = (volatile unsigned int*)((base_addr & base_addr_mask) | (((pci_addr & pci_addr_mask) | ((unsigned int)(ahbmem) &~ pci_addr_mask)) &~ base_addr_mask));

    grpci2_mst_enable(conf[0]);
    grpci2_mem_enable(conf[0]);
    grpci2_io_disable(conf[0]);
    grpci2_set_latency_timer(conf[0], 0x20);
    grpci2_set_bar(conf[0], 0, pci_addr);
    grpci2_set_barmap(conf[0], 0, ((unsigned int)ahbmem) & 0xfffffff0);
    grpci2_set_bus_big_endian(conf[0]);

    grpci2_set_mstmap(apb, 0, pci_addr);
    /* PCI-AHB loopback test */

//    report_subtest(8);
    /* Clear memory */
    for (i=0; i<32; i++){
      *(ahbmem + i) = 0;
    };

    /* Write to memory via PCI*/
    for (i=0; i<32; i++){
      *(pcimem + i) = i;
    };

    /* Check memory */
    for (i=0; i<32; i++){
      if (loadmem((unsigned int)(ahbmem + i)) != i) /* fail(1);*/ return 3;
    };

    /* Write to memory*/
    for (i=0; i<32; i++){
      *(ahbmem + i) = 31-i;
    };

    /* Check memory via PCI*/
    for (i=0; i<32; i++){
      if (loadmem((unsigned int)(pcimem + i)) != 31-i) /*fail(2);*/ return 4;
    };

    /* Change PCI bus endianess */
    grpci2_set_bus_litle_endian(conf[0]);

    /* Check memory via PCI*/
    for (i=0; i<32; i++){
      if (loadmem((unsigned int)(pcimem + i)) != 31-i) /*fail(3);*/ return 5;
    };

    /* Change PCI bus endianess */
    grpci2_set_bus_big_endian(conf[0]);

    if (grpci2_get_dma(apb)){

      /* PCI-DMA loopback test */
//      report_subtest(9);

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
      ddesc1->paddr = (pci_addr & pci_addr_mask) | ((unsigned int)(ahbmem + 32) &~ pci_addr_mask);
      ddesc1->aaddr = (unsigned int)ahbmem;
      ddesc1->ctrl = GRPCI2_DMA_DESC_EN | GRPCI2_DMA_DESC_IRQEN*0 | GRPCI2_DMA_DESC_DIR*1 |
                     GRPCI2_DMA_DESC_TYPE_DATA |
                     (31 & GRPCI2_DMA_DESC_LENGTH_MASK) << GRPCI2_DMA_DESC_LENGTH;

      /* Setup PCI to AHB transfer 16 words */
      ddesc2->paddr = (pci_addr & pci_addr_mask) | ((unsigned int)(ahbmem + 32) &~ pci_addr_mask);
      ddesc2->aaddr = (unsigned int)(ahbmem + 64);
      ddesc2->ctrl = GRPCI2_DMA_DESC_EN | GRPCI2_DMA_DESC_IRQEN*0 | GRPCI2_DMA_DESC_DIR*0 |
                     GRPCI2_DMA_DESC_TYPE_DATA |
                     (15 & GRPCI2_DMA_DESC_LENGTH_MASK) << GRPCI2_DMA_DESC_LENGTH;

      /* Setup AHB to PCI transfer 1 word */
      ddesc3->paddr = (pci_addr & pci_addr_mask) | ((unsigned int)(ahbmem + 96) &~ pci_addr_mask);
      ddesc3->aaddr = (unsigned int)(ahbmem + 64);
      ddesc3->ctrl = GRPCI2_DMA_DESC_EN | GRPCI2_DMA_DESC_IRQEN*0 | GRPCI2_DMA_DESC_DIR*1 |
                     GRPCI2_DMA_DESC_TYPE_DATA |
                     (0 & GRPCI2_DMA_DESC_LENGTH_MASK) << GRPCI2_DMA_DESC_LENGTH;

      /* Start DMA*/
      apb->dma_ctrl = GRPCI2_DMACTRL_EN;

      /* Wait on desc1 done*/
      while((loadmem((unsigned int)&ddesc1->ctrl) & GRPCI2_DMA_DESC_EN) != 0);
      for (i=0; i<32; i++){
        if (loadmem((unsigned int)(ahbmem + 32 + i)) != i) /*fail(1);*/ return 6;
      };

      /* Wait on desc2 done*/
      while((loadmem((unsigned int)&ddesc2->ctrl) & GRPCI2_DMA_DESC_EN) != 0);
      for (i=0; i<16; i++){
        if (loadmem((unsigned int)(ahbmem + 64 + i)) != i) /*fail(2);*/ return 7;
      };

      /* Wait on desc3 done*/
      while((loadmem((unsigned int)&ddesc3->ctrl) & GRPCI2_DMA_DESC_EN) != 0);
      for (i=0; i<1; i++){
        if (loadmem((unsigned int)(ahbmem + 96 + i)) != i) /*fail(3);*/ return 8;
      };

    };
  };

