#include "mcs9865_driver.h"
#include "ahbpci/ahbpci.h"

#include <stdio.h>
#include <inttypes.h>

#define UART_DATA      (0x00) // Регистр данных (RBR/THR)
#define UART_IER       (0x01) // Регистр разрешения прерываний (IER)
#define UART_LCR       (0x03) // Регистр управления линией (LCR)
#define UART_LSR       (0x05) // Регистр состояния линии (LSR)
#define UART_DLL       (0x00) // Регистр младшего байта делителя (DLL)
#define UART_DLM       (0x01) // Регистр старшего байта делителя (DLM)
#define UART_FCR       (0x02) // Регистр управления FIFO (FCR)
#define UART_MCR       (0x04) // Регистр управления модемом (MCR)

static void write_mem(uint32_t mem, uint32_t bar, uint32_t offset, uint32_t val);
static uint32_t read_mem(uint32_t mem, uint32_t bar, uint32_t offset);

void mcs9865_driver_simple_test() {
	uint32_t* pci_mem = ahbpci_get_mem_ptr();
	struct grpci2_pci_conf_space_regs* cfg = get_pci_conf(7, 0);

	uint32_t bar0 = grpci2_tw(cfg->head->bar[0]);

	printf("Vendor/device id = %d\n", grpci2_tw(cfg->head->dev_ven_id));
	printf("BAR0 = %" PRIu32 "\n", bar0);

	// Настройка UART
	write_mem((uint32_t)pci_mem, bar0, UART_LCR, 0x80); // DLAB = 1
	write_mem((uint32_t)pci_mem, bar0, UART_DLM, 0x00); // Divisor MSB = 0
	write_mem((uint32_t)pci_mem, bar0, UART_DLL, 0x01); // Divisor LSB = 1 -> Baudrate = 115200
	write_mem((uint32_t)pci_mem, bar0, UART_LCR, 0x03); // DLAB = 0, 8bit

	const char message[] = "Hello, World\n";
	const char* c = message;
	while (*c) {
		// Ждём пока регистр передачи (THR) не станет пустым
		while ( (read_mem((uint32_t)pci_mem, bar0, UART_LSR) & 0x20) == 0 );
		write_mem((uint32_t)pci_mem, bar0, UART_DATA, *c);

		c++;
	}
}

static void write_mem(uint32_t mem, uint32_t bar, uint32_t offset, uint32_t val) {
	uint32_t* addr = (uint32_t*)(mem + 0x280 + bar + (offset * 4));
	*addr = val;
}

static uint32_t read_mem(uint32_t mem, uint32_t bar, uint32_t offset) {
	uint32_t* addr = (uint32_t*)(mem + 0x280 + bar + (offset * 4));
	return *addr;
}


