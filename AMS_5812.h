#ifndef AMS5812_H_
#define AMS5812_H_

#define AMS_ADDR 0x78  //address of sensor

void AMS_5812_init(void);
float AMS_raw(void);
float AMS_5812_psi_read (void);
float AMS_psi_average (uint8_t);
float AMS_mmhg_average (uint8_t);

void AMS_5812_init(void)			// to initialize i2c, UART, and other peripherals for communication with sensor
{
// 		PORTA.PIN2CTRL |= (1 << 2);   // SDA pull up //PA2
// 		PORTA.PIN3CTRL |= (1 << 3);   // SCL pull up //PA3

	USART1_init(9600);
	I2C_0_initialization();
	I2C_0_master_initialization();
		I2C_0_sendAddress(0x78,1);  //1 for read
// 		_delay_ms(10);

}


float AMS_raw(void)
{
	uint16_t pressure_raw = 0, temp_raw = 0;
	
// 		I2C_0_sendAddress(0x78,1);  //1 for read
//   		_delay_ms(2);
// 

	pressure_raw = I2C_0_recieveData();
//   	_delay_us(10);
	pressure_raw <<= 8;
	pressure_raw |=I2C_0_recieveData();
//   	_delay_us(10);
	temp_raw = I2C_0_recieveData();
//   	_delay_us(10);
	temp_raw <<= 8;
	temp_raw |= I2C_0_recieveData();
//   	_delay_us(10);
// 	TWI0.MCTRLB |= (1 << 2);   // Send NACK

//   	I2C_0_stop_transmission();
// 	_delay_ms(1000);

	return (float)(pressure_raw);
}

float AMS_5812_psi_read (void)
{
// 	uint16_t _data2 = AMS_raw();
// 	for (int i = 0; i<100; i++)
// 	{
// 		_data2 += ABP_read_raw();
// 		_data2 /= 2;
// 	}
	//USART1_sendInt(_data2);
	return ((AMS_raw() - 3277.0 ) / (29490.0/5.0));
  //return ((AMS_raw()-3277.0) / (262.14));         // AMS 5812-1000-D
}

float AMS_psi_average (uint8_t average_no)
{
	float AMS_current_psi = 0.0;
	for (int i = 0; i<average_no; i++)
	{
		AMS_current_psi += AMS_5812_psi_read();
		AMS_current_psi /= 2.0;
	}
	return AMS_current_psi;
}

float AMS_mmhg_average (uint8_t average_no)
{
	float AMS_current_psi = 0.0;
	for (int i = 0; i<average_no; i++)
	{
		AMS_current_psi += AMS_5812_psi_read();
		AMS_current_psi /= 2.0;
	}
	return (AMS_current_psi*51.7149);
}

#endif