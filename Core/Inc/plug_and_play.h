#ifndef INC_PLUG_AND_PLAY_H_
#define INC_PLUG_AND_PLAY_H_

#include <stdint.h>
#include <bcc/ambapp.h>
#include <bcc/ambapp_ids.h>

uint32_t get_GPTIMER();
uint32_t get_GRGPIO();
uint32_t get_IRQMP();
uint32_t get_GRPCI2();
uint32_t get_GRPCI2_AHB();


#endif /* INC_PLUG_AND_PLAY_H_ */
