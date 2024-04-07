#include "plug_and_play.h"
#include "grgpio.h"
#include "gptimer1.h"

#include <bcc/bcc.h>

#include <stdio.h>
#include <inttypes.h>

// private variables
struct bcc_isr_node inode7   = {0};
GRGPIO_TypeDef*      GPIO_ptr   = NULL;
GPTIMER1_TypeDef*    TIMER1_ptr = NULL;
GRPCI2_REGS_TypeDef* PCICFG_ptr = NULL;

// function prototypes
void timer1_handler(void* arg, int source);
void init();
int main();

void timer1_handler(void* arg, int source) {
	uint32_t tmp = ~GPIO_ptr->OUTPUT & 0x0000FFFF;
	GPIO_ptr->OUTPUT = tmp;
	printf("Timer interrupt!\n");
}

void init() {
	GPIO_ptr   = (GRGPIO_TypeDef*)      get_GRGPIO();
	TIMER1_ptr = (GPTIMER1_TypeDef*)    get_GPTIMER();
	PCICFG_ptr = (GRPCI2_REGS_TypeDef*) get_GRPCI2();

	inode7.source  = 7;
	inode7.handler = timer1_handler;
	inode7.arg     = NULL;
	bcc_isr_register_node(&inode7);
	bcc_int_unmask(7);

	TIMER1_ptr->PRESCALER    = 0x00000001;  // Предделитель частоты
	TIMER1_ptr->PRESCALERVAL = 0x00000001;  // Значение предделителя частоты = 2
	TIMER1_ptr->CTRL1        = 0x0000000B;  // Выставлены биты: ES (разрешение таймера), RS (разрешение перезагрузки), IE (разрешение прерывания)
	TIMER1_ptr->CNT1         = 12500000;    // Счетчик
	TIMER1_ptr->CNTVAL1      = 12500000;    // Значение счетчика = 12500000. При clk = 50МГц, переполнение таймера 1 - 2Гц

	GPIO_ptr->DIRECTION      = 0x0000FFFF;  // Порт на вывод
}

int main() {

	return 0;
}
