#ifndef INC_GRGPIO_H_
#define INC_GRGPIO_H_

#include <stdint.h>

typedef struct {
	volatile uint32_t DATA;          // 0x00: Регистр данных порта ввода-вывода
	volatile uint32_t OUTPUT;        // 0x04: Выходной регистр порта ввода-вывода
	volatile uint32_t DIRECTION;     // 0x08: Регистр направления порта ввода-вывода
	volatile uint32_t IRMASK;        // 0x0C: Регистр маски прерываний
	volatile uint32_t IRPOLARITY;    // 0x10: Регистр полярности прерываний
	volatile uint32_t IRFRONT;       // 0x14: Регистр фронтов прерываний
	volatile uint32_t FUNC;          // 0x18: Регистр поддерживаемоего функционала
} GRGPIO_TypeDef;


#endif /* INC_GRGPIO_H_ */
