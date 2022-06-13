#ifndef SPI_0_AVR128DA64_H_
#define SPI_0_AVR128DA64_H_

#include "UART_1_AVR128DA64.h"

#define SlaveSelectDIR (PORTA_DIR |= (1 << 7))
#define SlaveSelectEnable (PORTA_OUT &= ~(1 << 7))
#define SlaveSelectDisable (PORTA_OUT |= (1 << 7))

#define timeout 60000

/************************************************************************/
/* Initialization
Initialize the SPI to a basic functional state by following these steps:
1. Configure the SS pin in the port peripheral.
2. Select the SPI master/slave operation by writing the Master/Slave Select (MASTER) bit in the Control A
(SPIn.CTRLA) register.
3. In Master mode, select the clock speed by writing the Prescaler (PRESC) bits and the Clock Double (CLK2X)
bit in SPIn.CTRLA.
4. Optional: Select the Data Transfer mode by writing to the MODE bits in the Control B (SPIn.CTRLB) register.
5. Optional: Write the Data Order (DORD) bit in SPIn.CTRLA.
6. Optional: Set up the Buffer mode by writing the BUFEN and BUFWR bits in the Control B (SPIn.CTRLB)
register.
7. Optional: To disable the multi-master support in Master mode, write ‘1’ to the Slave Select Disable (SSD) bit in
SPIn.CTRLB.
8. Enable the SPI by writing a ‘1’ to the ENABLE bit in SPIn.CTRLA.                                                                     */
/************************************************************************/
void SPI_0_init(void)
{
	SlaveSelectDIR;
	SlaveSelectDisable;
	
	PORTA.DIR |= PIN4_bm; // MOSI channel
	PORTA.DIR &= ~PIN5_bm; // MISO channel
	PORTA.DIR |= PIN6_bm; // SCK channel
	
	SPI0.CTRLA |= (1 << 5)|(1 << 4);    // SPI Master mode selected, CLK_PER/64, Clock double
	SPI0.CTRLA |= (1 << 0);               // SPI enabled.
	
	SlaveSelectDisable;
}

void SPI_0_send_char(uint8_t data)
{
	//SlaveSelectEnable;
	
	SPI0.DATA = data;
	while (!(SPI0.INTFLAGS & (1 << 7)))           // flag is set when a serial transfer is complete.
	{
		;
	}
	//SlaveSelectDisable;
	//_delay_ms(1);
}

void SPI_0_send_string(char *str)
{
	while(*str != '\0')
	{
		SPI_0_send_char(*str);
		//USART1_sendChar(*str);
		str++;
	}
}

void SPI_0_send_array(uint8_t SPI_data[], uint8_t datasize)
{
	for(int i=0; i<datasize; i++)
	{
		SPI0.DATA = SPI_data[i];
		while (!(SPI0.INTFLAGS & (1 << 7)))           // flag is set when a serial transfer is complete.
		{
			//USART1_sendString("wait");
			//_delay_ms(1);
		}
	}
}

#endif