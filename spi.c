// GPIO IP Example, gpio.c
// GPIO Shell Command
// Jason Losh

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: DE1-SoC Board

// Hardware configuration:
// GPIO Port:
//   GPIO_1[31-0] is used as a general purpose GPIO port
// IRQ:
//   IRQ80 is used as the interrupt interface to the HPS

//-----------------------------------------------------------------------------

#include <stdlib.h>          // EXIT_ codes
#include <stdio.h>           // printf
#include "spi_ip.h"         // SPI IP library

int main(int argc, char* argv[])
{
	uint32_t value;
	int pin;
    if (argc == 3)
    {
        spiOpen();

        if (strcmp(argv[1], "Read") == 0)
        {

        	if (strcmp(argv[2], "data") == 0)
            	printf("Data register -- 0x%08X\n" , ReadData());

       		else if (strcmp(argv[2], "status") == 0)
			{
       			value = ReadStatus();
       			printf("Status register -- 0x%08X\n" , value);
	        	if (value & 8)
	        		printf("TX FIFO OVERFLOW:	Yes\n");
	        	else
	        		printf("TX FIFO OVERFLOW:	No\n");
	        	if (value & 16)
					printf("TX FIFO FULL:	Yes\n");
				else
					printf("TX FIFO FULL:	No\n");
	        	if (value & 32)
					printf("TX FIFO EMPTY:	Yes\n");
				else
					printf("TX FIFO EMPTY:	No\n");
			}

        	else if (strcmp(argv[2], "control") == 0)
				printf("Control register -- 0x%08X\n" , ReadControl());

        	else if (strcmp(argv[2], "BRD") == 0)
        		printf("BRD register -- 0x%08X\n" , ReadBRD());
        }
        else if (strcmp(argv[1], "Write") == 0)
        {
			if (strcmp(argv[2], "enable") == 0)
				control_enable();
			else if (strcmp(argv[2], "disable") == 0)
				control_disable();
        }
    	else
	 	printf("argument %s not expected\n", argv[2]);
    }
    else if (argc==4)
    {
	spiOpen();

        if (strcmp(argv[1], "Write") == 0)
        {
        	if (strcmp(argv[2], "data") == 0)
            	WriteData(strtoul(argv[3], NULL,0));

       		else if (strcmp(argv[2], "status")== 0)
                WriteStatus(strtoul(argv[3], NULL,0));

        	else if (strcmp(argv[2], "control") == 0)
             	WriteControl(strtoul(argv[3], NULL,0));

        	else if (strcmp(argv[2], "BRD") == 0)
        		WriteBRD(strtoul(argv[3], NULL,0));
        }

     else
	printf("argument %s not expected\n", argv[2]);
    }
    
    else if (argc == 2 & (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0))
    {
        printf("  usage:\n");
        printf("  spi Write data <value>           make the pin a push-pull output\n");
        printf("  spi Write enable/disable            make the pin an open drain output\n");
        printf("  spi Read data            make the pin an open drain output\n");
        printf("  SPI Read status            make the pin an input\n");
        printf("  \n");
        printf("  SPI PIN high          set the pin high\n");
        printf("  SPI PIN low           set the pin low\n");
        printf("  SPI PIN status        get the pin status\n");

        printf("  \n");
        printf("  where PIN = [0, 31]\n");
    }
    else
        printf("  command not understood\n");
    return EXIT_SUCCESS;
}

