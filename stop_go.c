// spi IP Example
// Stop_Go Application (stop_go.c)
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
// Red LED:
//   Pin spi_1_0 drives a red LED through a 470 ohm resistror
// Green LED:
//   Pin spi_1_1 drives a green LED through a 470 ohm resistror
// Pushbutton:
//   Pin spi_1_2 pulled low by switch, pulled high through 10k ohm resistor

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include "spi_ip.h"

// Pins
#define RED_LED 0
#define GREEN_LED 1
#define PUSH_BUTTON 2

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Blocking function that returns only when SW1 is pressed
void waitPbPress()
{
	while(getPinValue(PUSH_BUTTON));
}

// Initialize Hardware
void initHw()
{
    // Initialize spi IP
    spiOpen();

    // Configure the I/O direction
    selectPinDirectionInput(PUSH_BUTTON);
    selectPinDirectionOutput(GREEN_LED);
	selectPinDirectionOutput(RED_LED);
    
    // Configure LED and pushbutton pins
	selectPinPullOutput(GREEN_LED);
	selectPinPullOutput(RED_LED);
    

}

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

int main(void)
{
	// Initialize hardware
	initHw();

    // Turn off green LED, turn on red LED
	setPinValue(GREEN_LED, 0);
    setPinValue(RED_LED, 1);

    // Wait for PB press
    waitPbPress();

    // Turn off red LED, turn on green LED
    setPinValue(RED_LED, 0);
    setPinValue(GREEN_LED, 1);
}
