#ifndef INC_GRPCI2_H_
#define INC_GRPCI2_H_

#include <stdint.h>

typedef struct {
	volatile uint32_t CTRL;           // 0x00:        Регистр управления
	volatile uint32_t STS_CAP;        // 0x04:        Регистр статуса и характеристик
	volatile uint32_t PPREF;          // 0x08:        Регистр границы пакета предварительной выборки ведущего устройства PCI
	volatile uint32_t IO_MAP;         // 0x0C:        Регистр отображения доступов AHB в доступы PCI для области ввода-вывода PCI
	volatile uint32_t DMA_CTRL;       // 0x10:        Регистр управления и статуса DMA
	volatile uint32_t DMA_BDBASE;     // 0x14:        Регистр базового адреса (BAR) дескриптора DMA
	volatile uint32_t DMA_CHACT;      // 0x18:        Регистр активного канала DMA
	volatile uint32_t RESERVED1;      // 0x1C:        Зарезервировано
	volatile uint32_t BARS[6];        // 0x20 - 0x34: Регистр базового адреса (BAR) отображения доступа PCI в доступ AHB
	volatile uint32_t RESERVED2[2];   // 0x38 - 0x3C: Зарезервировано
	volatile uint32_t AHBMST_MAP[16]; // 0x40 - 0x7C: Регистр отображения адреса доступа ведущего устройства AHB в адрес доступа к области памяти PCI
} GRPCI2_REGS_TypeDef;

typedef struct {
	volatile uint32_t DEVID;          // 0x00:        Регистр идентификации устройства и идентификации поставщика
	volatile uint32_t STS_CTRL;       // 0x04:        Регистр статуса и управления
	volatile uint32_t VERID;          // 0x08:        Регистр кода класса и идентификации версии
	volatile uint32_t BIST;           // 0x0C:        Регистр BIST, типа заголовка, таймера периода ожидания и размера строки кэша
	volatile uint32_t BAR;            // 0x10:        Регистр базового адреса
	volatile uint32_t RESERVED1[8];   // 0x14 - 0x30: Зарезервировано
	volatile uint32_t CHARPTR;        // 0x34:        Регистр указателя характеристик
	volatile uint32_t RESERVED2;      // 0x38:        Зарезервировано
	volatile uint32_t INT;            // 0x3C:        Регистр Max_Lat, Min_Gnt, вывода прерывания и строки прерывания

	// Расширенная область конфигурации PCI
	volatile uint32_t CTRL;           // 0x40 + 0x00: Регистр размера, следующего указателя, идентификации
	volatile uint32_t PCI2AHB_MAP[6]; // 0x40 + 0x04: Регистр отображения PCI BAR в адрес AHB
	volatile uint32_t EXT2AHB_MAP;    // 0x40 + 0x1C: Регистр отображения расширенной области конфигурации PCI в адрес AHB
	volatile uint32_t IO_MAP;         // 0x40 + 0x20: Регистр базового адреса области ввода-вывода AHB и конфигурации шины PCI
	volatile uint32_t PCIBAR_SIZE[6]; // 0x40 + 0x24: Регистр размера PCI BAR и предварительной выборки
	volatile uint32_t AHB_PREF;       // 0x40 + 0x3C: Регистр границы пакета предварительной выборки ведущего устройства AHB
} GRPCI2_CFG_TypeDef;


#endif /* INC_GRPCI2_H_ */
