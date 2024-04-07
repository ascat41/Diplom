#ifndef INC_AHBPCI_AHBPCI_H_
#define INC_AHBPCI_AHBPCI_H_

#include <stdint.h>

/**
 * This typedef contains IP hardware configuration information.
 */

typedef  struct {
	uint16_t  DeviceId;			    /**< Unique ID of PCIe IP */
	uint32_t* BaseAddress;		    /**< Register base address */
	uint8_t   LocalBarsNum;		    /* The number of local bus (AXI) BARs
					                 * in hardware
					                 */
	uint8_t   IncludeBarOffsetReg;	/**<Are BAR Offset registers built in
					                 * hardware
					                 */
	uint8_t   IncludeRootComplex;	/**< Is IP built as root complex */
} AHBPCI_Config_t;

/**
 * The XAxiPcie driver instance data. The user is required to allocate a
 * variable of this type for every PCIe device in the system that will be
 * using this API. A pointer to a variable of this type is passed to the driver
 * API functions defined here.
 */

typedef struct {
	AHBPCI_Config_t Config;             /**< Configuration data */
	uint32_t        IsReady;            /**< Is IP been initialized and ready */
	uint32_t        MaxNumOfBuses;		/**< If this is RC IP, Max Number of
                                         * Buses */
} AHBPCI_t;


#endif /* INC_AHBPCI_AHBPCI_H_ */
