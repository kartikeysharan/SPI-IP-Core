// GPIO IP Library Registers
// Jason Losh

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: DE1-SoC Board
// Target uC:       -
// System Clock:    -

// Hardware configuration:
// GPIO IP core connected to light-weight Avalon bus

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#ifndef SPI_REGS_H_
#define SPI_REGS_H_

#define OFS_DATA             0
#define OFS_STATUS           1
#define OFS_CONTROL          2
#define OFS_BRD              3

#define WORDSIZE_MASK	0x1F
#define CS_SELECT_MASK	0x3
#define DEVICE_MODE_MASK	0x3

#define CS_SELECT_BIT_OFS	13
#define DEVICE_MODE_BIT_OFS	16
#define CS_AUTO_BIT_OFS	5
#define CS_ENABLE_BIT_OFS	9

#define IODIR 0x00
#define GPPU 0x06
#define GPIO 0x09
#define OPCODE 0x40

#define SPAN_IN_BYTES 16

#endif

