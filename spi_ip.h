// GPIO IP Example
// GPIO IP Library (gpio_ip.h)
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

#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>
#include <stdbool.h>

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------
bool spiOpen();
void WriteData(uint32_t pin);
void WriteStatus(uint32_t pin);
void WriteControl(uint32_t pin);
void WriteBRD(uint32_t pin);
void control_enable();
void control_disable();

uint32_t ReadData();
uint32_t ReadStatus();
uint32_t ReadControl();
uint32_t ReadBRD();

void selectPinPullOutput(uint8_t pin);
void selectPinPushOutput(uint8_t pin);
void selectPinDirectionInput(uint8_t pin);
void selectPinDirectionOutput(uint8_t pin);
void setPinValue(uint8_t pin, bool value);
bool getPinValue(uint8_t pin);

#endif
