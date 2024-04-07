#ifndef INC_GPTIMER1_H_
#define INC_GPTIMER1_H_

#include <stdint.h>

typedef struct {
	volatile uint32_t PRESCALER;     // 0x00: Регистр значения предделителя
	volatile uint32_t PRESCALERVAL;  // 0x04: Регистр значения перезагрузки предделителя
	volatile uint32_t CONF;          // 0x08: Регистр конфигураций
	volatile uint32_t CAPTURECONF;   // 0x0C: Регистр конфигураций захвата (не используется в GPTIMER 1)
	volatile uint32_t CNT1;          // 0x10: Регистр значений счетчика таймера 1
	volatile uint32_t CNTVAL1;       // 0x14: Регистр значений перезагрузки таймера 1
	volatile uint32_t CTRL1;         // 0x18: Регистр управления таймером 1
	volatile uint32_t CAPTURE1;      // 0x1C: Регистр захвата таймера 1 (не используется в GPTIMER 1)
	volatile uint32_t CNT2;          // 0x20: Регистр значений счетчика таймера 2
	volatile uint32_t CNTVAL2;       // 0x24: Регистр значений перезагрузки таймера 2
	volatile uint32_t CTRL2;         // 0x28: Регистр управления таймером 2
} GPTIMER1_TypeDef;


#endif /* INC_GPTIMER1_H_ */
