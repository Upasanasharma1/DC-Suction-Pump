#ifndef I2C_0_AVR128DA64_H_
#define I2C_0_AVR128DA64_H_

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
//#include "UART_1_AVR128DA64.h"

#define timeout 60000

bool i2c_timeout_flag = false;

void I2C_0_status(void)
{
	if (TWI0.MSTATUS & (1 << 7))
	{
		USART1_sendString("RIF");
	}
	if (TWI0.MSTATUS & (1 << 6))
	{
		USART1_sendString("WIF");
	}
	if (TWI0.MSTATUS & (1 << 5))
	{
		USART1_sendString("CLKHOLD");
	}
	if (!(TWI0.MSTATUS & (1 << 4)))
	{
		USART1_sendString("RXACK");
	}
	if (TWI0.MSTATUS & (1 << 3))
	{
		USART1_sendString("ARBLOST");
	}
	if (TWI0.MSTATUS & (1 << 2))
	{
		USART1_sendString("BUSERR");
	}
	if ((TWI0.MSTATUS & 0x3) == 0x1)
	{
		USART1_sendString("IDLE");
	}
	else if ((TWI0.MSTATUS & 0x3) == 0x2)
	{
		USART1_sendString("OWNER");
	}
	else if ((TWI0.MSTATUS & 0x3) == 0x3)
	{
		USART1_sendString("BUSY");
	}
}

void I2C_0_stop_transmission(void)
{
	TWI0.MSTATUS = 0x1;
	TWI0.MCTRLB = 0x3;
}

void I2C_0_initialization(void)
{
	TWI0.CTRLA &= ~(1 << 4);    // SDA setup time is four clock cycles
	TWI0.CTRLA &= ~(0x3 << 2);   // Hold time OFF
	TWI0.CTRLA &= ~(1 << 1);   // Operating in Standard mode or Fast mode
}

void I2C_0_master_initialization(void)
{
	//sei();
	
	//TWI0.MBAUD = 13;         // fscl = 100khz, rise time = 1us  when cpu clock 4MHZ
	TWI0.MBAUD = 103;         // fscl = 100khz, rise time = 1us when cpu clock 24MHZ
	TWI0.MSTATUS = 0x1;    // Idle bus state
	
	TWI0.MCTRLA |= (1 << 1)|(0x2 << 2);   // Smart mode enable, Quick Command enable, Inactive Bus Time-OUT set to 100us.
	TWI0.MCTRLA |= (1 << 0); // Enable TWI Master
	
	//I2C_0_status();
}

void I2C_0_sendAddress(char address, bool directionbit)
{
	//TWI0.MCTRLB |= (1 << 3);
	uint16_t timeout_temp = timeout;
	
	if ((TWI0.MSTATUS & 0x3) == 0x3)
	{
		USART1_sendString("I2C busy.");
		while(((TWI0.MSTATUS & 0x3) == 0x3) && (timeout_temp--));   // Wait till I2C busy.
	}
	 
	//USART1_sendString("I2C free.");
	
	TWI0.MSTATUS = (1 << 2)|0x1;
	timeout_temp = timeout;
	
	//TWI0.MADDR = (address << 1)|(directionbit << 0);
	if(directionbit)
	{
		TWI0.MADDR = (address << 1)|(1 << 0);
	}
	else
	{
		TWI0.MADDR = (address << 1)|0x00;
	}
	//TWI0.MADDR = address;
	
	while(((TWI0.MSTATUS & (1 << 4))) && (!(TWI0.MSTATUS & (1 << 6)))  && (--timeout_temp))    // This flag is set to ‘1’ when a master transmit address or byte write operation is completed.
	{
		;
	}
	
	//I2C_0_status();
	//
	if (timeout_temp <= 0)
	{
		USART1_sendString("timeout");
	}
	if ((TWI0.MSTATUS & (1 << 2)))
	{
		USART1_sendString("\n----> Error in sending addr.\n");
		//I2C_0_stop_transmission();
		//
		//while((TWI0.MSTATUS & 0x3) == 0x3);
		//
		//I2C_0_sendAddress(address,directionbit);
	}
	
}

void I2C_0_sendData(char data)
{
	_delay_ms(1);
	TWI0.MDATA = data;
	uint16_t timeout_temp = timeout;
	while((TWI0.MSTATUS & (1 << 4)) && (--timeout_temp))  // When this flag is read as ‘0’, it indicates that the most recent Acknowledge bit from the slave was ACK and the slave is ready for more data.
	{
		;
	}
	
	if (timeout_temp <= 0)
	{
		USART1_sendString("timeout");
	}
	
	if ((TWI0.MSTATUS & (1 << 3)) || (TWI0.MSTATUS & (1 << 2)))
	{
		USART1_sendString("Error in data sent.");
		I2C_0_stop_transmission();
	}
}

void I2C_0_sendString(char* str)
{
	while(*str != '\0')
	{
		I2C_0_sendData(*str);
		str++;
	}
}

uint8_t I2C_0_recieveData(void)
{
	uint16_t timeout_temp = timeout;
	while ((!(TWI0.MSTATUS & (1 << 7))) && (--timeout_temp))
	{
		;
	}
	if ((TWI0.MSTATUS & (1 << 3)) || (TWI0.MSTATUS & (1 << 2)))
	{
		USART1_sendString("Error in data recieved.");
		I2C_0_stop_transmission();
		i2c_timeout_flag = true;
	}
	
	if (timeout_temp <= 0)
	{
		USART1_sendString("timeout");
		i2c_timeout_flag = true;
	}
	return (TWI0.MDATA);
}


void I2C_0_recvtrans(void)
{
	TWI0.MCTRLB &= ~(1 << 4);   // Send ACK
	TWI0.MCTRLB |= 0x2;         // RECVTRANS
}


#endif