#ifndef INC_IRQMP_H_
#define INC_IRQMP_H_

#include <stdint.h>

typedef struct {
	volatile uint32_t LEVEL;         // 0x00: Регистр уровней прерываний
	volatile uint32_t WAIT;          // 0x04: Регистр ожидающих обработки прерываний
	volatile uint32_t SET;           // 0x08: Регистр принудительной установки прерываний
	volatile uint32_t RESET;         // 0x0C: Регистр сброса прерываний
	volatile uint32_t STATUS;        // 0x10: Регистр статуса процессорной системы
	volatile uint32_t MASK;          // 0x14: Регистр маски прерывания процессора
	volatile uint32_t FORCED;        // 0x18: Регистр принудительной установки и сброса прерываний
	volatile uint32_t ENEXT;         // 0x1C: Регистр подтверждения приема расширенного прерывания
} IRQMP_TypeDef;



#endif /* INC_IRQMP_H_ */
