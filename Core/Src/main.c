#include "plug_and_play.h"
#include "grgpio.h"
#include "gptimer1.h"
#include "grpci2/grpci2api.h"
#include "ahbpci/ahbpci.h"

#include <bcc/bcc.h>
#include <bcc/bcc_param.h>

#include <stdio.h>
#include <inttypes.h>

// private macros
#define RAM_START        (0x80000000)
#define AHBMST2PCIADR    (RAM_START)

// private variables
struct bcc_isr_node inode7   = {0};
GRGPIO_TypeDef*      GPIO_ptr    = NULL;
GPTIMER1_TypeDef*    TIMER1_ptr  = NULL;
//GRPCI2_REGS_TypeDef* PCIREGS_ptr = NULL;
uint32_t* AHBPCI_ptr             = NULL;

// function prototypes
void timer1_handler(void* arg, int source);
void init();
int main();

int __bcc_con_init(void) {
	__bcc_con_handle = 0x80000600;
	return 0;
}

void timer1_handler(void* arg, int source) {
	uint32_t tmp = ~GPIO_ptr->OUTPUT & 0x0000FFFF;
	GPIO_ptr->OUTPUT = tmp;
	printf("Timer interrupt!\n");
}

void init() {
	GPIO_ptr    = (GRGPIO_TypeDef*)      get_GRGPIO();
	TIMER1_ptr  = (GPTIMER1_TypeDef*)    get_GPTIMER();
	AHBPCI_ptr  = (uint32_t*)            get_GRPCI2_AHB();

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

	// TODO: необходимо прокинуть указатель на пространства pci и apb регистров хост-контроллера
	ahbpci_init();
}

int main() {
//	setbuf(stdout, NULL);
	init();

	while (1) {}

	return 0;
}
