// GPIO IP Example
// GPIO IP Library (gpio_ip.c)
// Jason Losh

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: DE1-SoC Board

// Hardware configuration:
// GPIO Port:
//   GPIO_1[31-0] is used as a general purpose GPIO port
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
#include "spi_ip.h"         // gpio
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

void WriteData(uint32_t pin)
{
    *(base+OFS_DATA) =  pin;
}


void WriteStatus(uint32_t pin)
{
    *(base+OFS_STATUS) = pin;
}

void WriteControl(uint32_t pin)
{
    *(base+OFS_CONTROL) =  pin;
}

void WriteBRD(uint32_t pin)
{
    uint32_t mask = pin << 6;
    *(base+OFS_BRD)  = mask;
}

uint32_t ReadData()
{
    uint32_t value = *(base+OFS_DATA);
    return value;
}

uint32_t ReadStatus()
{
    uint32_t value = *(base+OFS_STATUS);
    return value;
}
uint32_t ReadControl()
{
    uint32_t value = *(base+OFS_CONTROL);
    return value;
}
uint32_t ReadBRD()
{
    uint32_t value = *(base+OFS_BRD);
    return value;
}

void control_enable()
{
	uint32_t mask = (1 << 15);
	*(base+OFS_CONTROL) |= mask;
}

void control_disable()
{
	uint32_t mask = ~(1 << 15);
	*(base+OFS_CONTROL) &= mask;
}
