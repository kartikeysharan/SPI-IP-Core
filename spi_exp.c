// spi IP Example
// spi IP Library (spi_ip.c)
// Jason Losh

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: DE1-SoC Board

// Hardware configuration:
// spi Port:
//   spi_1[31-0] is used as a general purpose spi port
// HPS interface:
//   Mapped to offset of 0 in light-weight MM interface aperature
//   IRQ80 is used as the interrupt interface to the HPS

//-----------------------------------------------------------------------------

#include <stdint.h>          // C99 integer types -- uint32_t
#include <stdbool.h>         // bool
#include <fcntl.h>           // open
#include <sys/mman.h>        // mmap
#include <unistd.h>          // close
#include "../address_map.h"  // address map
#include "spi_ip.h"         // spi
#include "spi_regs.h"       // registers

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

uint32_t *base = NULL;

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

bool spiOpen()
{
    // Open /dev/mem
    int file = open("/dev/mem", O_RDWR | O_SYNC);
    bool bOK = (file >= 0);
    if (bOK)
    {
        // Create a map from the physical memory location of
        // /dev/mem at an offset to LW avalon interface
        // with an aperature of SPAN_IN_BYTES bytes
        // to any location in the virtual 32-bit memory space of the process
        base = mmap(NULL, SPAN_IN_BYTES, PROT_READ | PROT_WRITE, MAP_SHARED,
                    file, LW_BRIDGE_BASE + SPI_BASE_OFFSET);
        bOK = (base != MAP_FAILED);

        // Close /dev/mem
        close(file);
    }
    return bOK;
}

void selectPinPullOutput(uint8_t pin)
{
	uint32_t mask = (OPCODE << 16);		// Opcode
	mask &= ~(1 << 16);	// Setting the R/W signal as write
	mask = (GPPU << 8);
	mask |= (1 << pin);
    *(base+OFS_DATA) = mask;
}

void selectPinPushOutput(uint8_t pin)
{
	uint32_t mask = (OPCODE << 16);		// Opcode
	mask &= ~(1 << 16);	// Setting the R/W signal as write
	mask = (GPPU << 8);
	mask &= ~(1 << pin);
    *(base+OFS_DATA) = mask;
}

void selectPinDirectionInput(uint8_t pin)
{
	uint32_t mask = (OPCODE << 16);		// Opcode
	mask &= ~(1 << 16);	// Setting the R/W signal as write
	mask = (IODIR << 8);
	mask |= (1 << pin);
    *(base+OFS_DATA) = mask;
}

void selectPinDirectionOutput(uint8_t pin)
{
	uint32_t mask = (OPCODE << 16);		// Opcode
	mask &= ~(1 << 16);	// Setting the R/W signal as write
	mask = (IODIR << 8);
	mask &= ~(1 << pin);
    *(base+OFS_DATA) = mask;
}

void setPinValue(uint8_t pin, bool value)
{
	uint32_t mask = (OPCODE << 16);		// Opcode
	mask &= (1 << 16);	// Setting the R/W signal as write
	mask = (GPIO << 8);
    if (value)
    	mask |= (1 << pin);
    else
    	mask &= ~(1 << pin);
    *(base+OFS_DATA) = mask;
}

bool getPinValue(uint8_t pin)
{
	uint32_t mask = (OPCODE << 16);		// Opcode
	mask &= ~(1 << 16);	// Setting the R/W signal as write
	mask = (GPIO << 8);
	*(base+OFS_DATA) = mask;
	while(!(*(base+OFS_STATUS) & 32)); // Make sure TX FIFO is non empty
    return (*(base+OFS_DATA) >> 2;
}

//void setPortValue(uint32_t value)
//{
//     *(base+OFS_DATA) = value;
//}
//
//uint32_t getPortValue()
//{
//    uint32_t value = *(base+OFS_DATA);
//    return value;
//}
