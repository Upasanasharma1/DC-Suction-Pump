#ifndef ABP_READ_h
#define ABP_READ_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include "I2C_0_AVR128DA64.h"

#define ABP_ADDR 0x28  //address of sensor
#define p_min 0
#define p_max 5	//in psi
#define average_time	5

uint16_t raw_pressure_data = 0, lsb_pressure_data = 0;						//16 bit pressure data sent by sensor
float pressure_psi = 0.0, pressure_cmh2o = 0.0;		//actual pressure data converted by the controller using pressure_data mentioned above

float abp_average_pressure(uint8_t);


void ABP_init(void)			// to initialize i2c, UART, and other peripherals for communication with sensor
{
// 	PORTA.PIN2CTRL |= (1 << 3);   // SDA pull up //PA2		//external pullup of 2k should be applaied	*****
// 	PORTA.PIN3CTRL |= (1 << 3);   // SCL pull up //PA3
// 	USART1_init(9600);

	I2C_0_master_initialization();
	_delay_ms(50);
}

uint16_t ABP_read_raw (void)
{
	I2C_0_sendAddress(ABP_ADDR, 1);
	_delay_ms(20);
	raw_pressure_data =I2C_0_recieveData();
	raw_pressure_data <<= 8;
	raw_pressure_data |=I2C_0_recieveData();
	//		TWI0.MCTRLB |= (1 << 2);   // Send NACK
 	I2C_0_stop_transmission();
	return (raw_pressure_data);
}

float average(void)
{	double sum = 0;
	for (int i=0; i<average_time; i++)
	{
		sum += (double)ABP_read_raw();
	}
	float _average = sum/(double)average_time; 
//	USART1_sendFloat((float)sum, 3);
	return _average;
}

float ABP_psi_read (void)
{
	float output = ((average()/16383.0)*100.0);				//transfer function of the ABP sensor
	float _pressure_psi = (( output - 10.0 ) * ( 5.0 / 80.0 ));	//transfer function of the ABP sensor
//  	USART1_sendString("PSI");
//  	USART1_sendFloat(_pressure_psi,3);
	return _pressure_psi;
}

float ABP_mmHg_read (void)
{
	float _pressure_mmHg = (ABP_psi_read()*51.715);
	if ( (_pressure_mmHg > 350) || (_pressure_mmHg < 0))
	{
		_pressure_mmHg = 0.0;
	}
// 	USART1_sendString("mmHg");
// 	USART1_sendFloat(_pressure_mmHg, 3);
	return _pressure_mmHg;

}

float abp_average_pressure(uint8_t average_count)
{
	float average = 0.0;
	for(int i=0; i<average_count; i++ )
	{
		average += ABP_mmHg_read();
	}
	return (average/(float)average_count);
}

#endif