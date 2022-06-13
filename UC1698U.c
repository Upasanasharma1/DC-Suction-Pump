
#include "_UC1698U.h"

uint8_t data = 0;

void UC1698U_gpio_init(void)
{
	cs_port.DIR |= cs_pin;
	a0_port.DIR |= a0_pin;
	rst_port.DIR |= rst_pin;
}

void UC1698U_init(void)
{
	UC1698U_CS_ENABLE();
	UC1698U_DATAMODE();
	
	UC1698U_SETRST();
	_delay_ms(300);
	UC1698U_CLRRST();
	_delay_ms(500);		//reset takes more than >=150msec
	
	UC1698U_SEND_CMD(SET_MTP_CONTROL_COMMAND);
	UC1698U_SEND_CMD_DATA(MTP_CONTROL_DATA);
	
	UC1698U_SEND_CMD(RESET);//software reset
	_delay_ms(200);

	UC1698U_SEND_CMD(SET_BIAS_RATIO);
	UC1698U_SEND_CMD(SET_VBIAS_POT_COMMAND);
	UC1698U_SEND_CMD(SET_VBIAS_POT_DATA);
	UC1698U_SEND_CMD(SET_POWER_CONTROL);
	UC1698U_SEND_CMD(SET_LCD_MAPPING_CONTROL);
	UC1698U_SEND_CMD(SET_RAM_ADDRESS_CONTROL);
	UC1698U_SEND_CMD(SET_TEMP_COMPENSATION);
	UC1698U_SEND_CMD(SET_LINE_RATE);
	UC1698U_SEND_CMD(SET_COM_END);							//TO SET LAST ROW WE ARE USING IN THE DISPLAY
	UC1698U_SEND_CMD_DATA(159);
	UC1698U_SEND_CMD(PARTIALMODE_OFF);						//Please maintain the following relationship:
	UC1698U_SEND_CMD(PARTIAL_DISPLAY_START_ADDR);			//CEN = the actual number of pixel rows on the LCD - 1
	UC1698U_SEND_CMD_DATA(000);									//CEN >=DEN>=DST+ 9
	UC1698U_SEND_CMD(PARTIAL_DISPLAY_END_ADDR);
	UC1698U_SEND_CMD_DATA(50);
	UC1698U_SEND_CMD(SET_SCROLL_LINE_LSB);
	UC1698U_SEND_CMD(SET_SCROLL_LINE_MSB);
	UC1698U_SEND_CMD(SET_COLOUR_PATTERN_RGB);
	UC1698U_SEND_CMD(SET_COLOUR_MODE);
	UC1698U_SEND_CMD(N_LINE_INVERSION_COMMAND);
	UC1698U_SEND_CMD(N_LINE_INVERSION_DATA);
	UC1698U_SEND_CMD(SET_COM_SCAN_FUNCTION);
	//UC1698U_SEND_CMD(SET_WINDOW_PROGRAME_INSIDE_MODE);
	set_all_pixel_off();
	UC1698U_SEND_CMD(DISPLAY_ON);
}

void set_row_addr(uint8_t row_addr)
{
	uint8_t _set_row_addr_lsb = (SET_ROW_ADDR_LSB_CMD | (row_addr & ~(0b11110000)));
	uint8_t _set_row_addr_msb = (SET_ROW_ADDR_MSB_CMD | (row_addr>>4));
	UC1698U_SEND_CMD_DATA(_set_row_addr_lsb);
	UC1698U_SEND_CMD_DATA(_set_row_addr_msb);
}

void set_colm_addr(uint8_t colm_addr)
{
	uint8_t _set_colm_addr_lsb = (SET_COLUMN_ADDR_LSB_CMD | (colm_addr & 0x0F));
	uint8_t _set_colm_addr_msb = (SET_COLUMN_ADDR_MSB_CMD | (colm_addr >> 4));
	UC1698U_SEND_CMD_DATA(_set_colm_addr_lsb);
	UC1698U_SEND_CMD_DATA(_set_colm_addr_msb);
}

void UC1698U_SEND_ARRAY_DATA ( uint8_t _dat[], int dataSize)	//FUCTION TO SEND MULTIPLE BYTE OF DATA
{
	UC1698U_DATAMODE();
	UC1698U_CS_ENABLE();
	SPI_0_send_array(_dat, dataSize);
	UC1698U_CS_DISABLE();
}

void UC1698U_SEND_BYTE_DATA ( uint8_t _data)	//FUNCTION TO SEND SINGLE BYTE OF DATA
{
	UC1698U_DATAMODE();
	UC1698U_CS_ENABLE();
	UC1698U_SERIALWRITE(_data);
	UC1698U_CS_ENABLE();
}

void UC1698U_SEND_CMD(enum UC1698U_INSTRUCTION_TABLE cmd)	//TO SEND CMDS FROM ENUM DECLARED ABOVE
{
	UC1698U_CMDMODE();
	UC1698U_CS_ENABLE();
	UC1698U_SERIALWRITE(cmd);
	UC1698U_CS_DISABLE();
	UC1698U_DATAMODE();
}

void UC1698U_SEND_CMD_DATA(uint8_t C)	//TO SEND DATA OF COMMANDS WICH ARE OUT OF THE SCOPE OF ENUM
{
	UC1698U_CMDMODE();
	UC1698U_CS_ENABLE();
	UC1698U_SERIALWRITE(C);
	UC1698U_CS_DISABLE();
	UC1698U_DATAMODE();

}

void set_all_pixel_off (void)
{
	window_program_start_end_colm_row_addr (0,0,79,159);
	for (int i=0; i<25600; i++)
	{
		UC1698U_SEND_BYTE_DATA(0);
	}
}

void set_all_pixel_on (void)	//do not use set_all_pixel_on cmd from the datasheet as it will permanently turn on all pixels and will not let u write any data on pixel even after its execution
{
	window_program_start_end_colm_row_addr (0,0,79,159);
	for (int i=0; i<25600; i++)
	{
		UC1698U_SEND_BYTE_DATA(0xff);
	}
}



 void print_icon (uint8_t x, uint8_t y, uint8_t *font, uint8_t height, uint8_t width)
 {
	 long new_pos = 0, buff_length = ((width/3)*2);	//WIDTH=NO. OF PIXEL OF WIDTH OF ICON /3=TO FIND THE NO. OF COLUMN ADDRESS *2=AS 1 COLUMN ADDRESS REQUIRES 2 BYTES OF DATA
	 
	 for ( int _length = 0; _length < height; _length++)
	 {
		 uint8_t _buff[buff_length];
		 for (int i = 0; i < buff_length; i++)			// to make the buff initialize with 0
		 {
			 _buff[i] = 0x00;
		 }
		 uint8_t k = 0;
		 uint8_t array_data = 0, t = 0;
		 
		 for (int _width = 0; _width < (width/8); _width++)
		 {
			 array_data =  pgm_read_byte(&(font[new_pos]));
			 // 			USART1_sendString("new position");
			 // 			USART1_sendInt(new_pos);
			 // 			USART1_sendString("pixel data");
			 // 			USART1_sendInt(array_data);
			 new_pos++;
			 for (int j = 7; j >= 0; j--)
			 {
				 // 				USART1_sendString("j");
				 // 				USART1_sendInt(j);
				 // 				USART1_sendInt(array_data & (1<<j));
				 if (array_data & (1<<j))
				 {
					 _buff[t] = _buff[t] | pre_def[k][0];
					 _buff[t+1] = _buff[t+1] | pre_def[k][1];
					 // 					USART1_sendInt(_buff[i]);
					 // 					USART1_sendInt(_buff[i+1]);
				 }
				 if(++k > 2)
				 {
					 k = 0;
					 t+=2;
				 }
			 }
			 
		 }
		 t = 0;
		 window_program_start_end_colm_row_addr(x,(y+_length),(x+(width/3)),(y+_length));
		 UC1698U_SEND_ARRAY_DATA(_buff,buff_length);
	 }
 }

 void print_number (uint8_t x, uint8_t y, uint8_t *font, uint8_t height, uint8_t width, uint8_t no)
 {
	 
	 unsigned long new_pos = (((long)width/8)*(long)height*(long)no), buff_length = ((width/3)*2);	//WIDTH=NO. OF PIXEL OF WIDTH OF ICON /3=TO FIND THE NO. OF COLUMN ADDRESS *2=AS 1 COLUMN ADDRESS REQUIRES 2 BYTES OF DATA
	 
	 for ( int _length = 0; _length < height; _length++)
	 {
		 uint8_t _buff[buff_length];
		 for (int i = 0; i < buff_length; i++)			// to make the buff initialize with 0
		 {
			 _buff[i] = 0x00;
		 }
		 uint8_t k = 0;
		 uint8_t array_data = 0, t = 0;

		 for (int _width = 0; _width < (width/8); _width++)
		 {
			 array_data =  pgm_read_byte(&(font[new_pos]));
			 // 			USART1_sendString("new position");
			 // 			USART1_sendInt(new_pos);
			 // 			USART1_sendString("pixel data");
			 // 			USART1_sendInt(array_data);
			 new_pos++;
			 for (int j = 7; j >= 0; j--)
			 {
				 // 				USART1_sendString("j");
				 // 				USART1_sendInt(j);
				 // 				USART1_sendInt(array_data & (1<<j));
				 if (array_data & (1<<j))
				 {
					 _buff[t] = _buff[t] | pre_def[k][0];
					 _buff[t+1] = _buff[t+1] | pre_def[k][1];
					 // 					USART1_sendInt(_buff[i]);
					 // 					USART1_sendInt(_buff[i+1]);
				 }
				 if(++k > 2)
				 {
					 k = 0;
					 t+=2;
				 }
			 }

		 }
		 t = 0;
		 window_program_start_end_colm_row_addr(x,(y+_length),(x+(width/3)),(y+_length));
		 UC1698U_SEND_ARRAY_DATA(_buff,buff_length);
	 }

 }

void print_double_line (uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool orientation)//orientation will be false for vertical line and true for horizontal line
{
	// 	x1 += 24;
	// 	x2 += 24;
	if (orientation)
	{	uint8_t j = 0;
		uint8_t buff_leng = (x2-x1) ;
		window_program_start_end_colm_row_addr(x1, y1, x2, y2);
		for (j = 0; j < (buff_leng); j++)
		{
			UC1698U_SEND_BYTE_DATA(0xFF);
			UC1698U_SEND_BYTE_DATA(0xFF);
		}
	}
	else
	{
		unsigned int j = 0;
		unsigned int buff_leng=0;
		window_program_start_end_colm_row_addr(x1, y1, x2, y2);
		buff_leng = (y2-y1);
		for (j = 0; j < (buff_leng); j++)
		{
			UC1698U_SEND_BYTE_DATA(0xFF);
			UC1698U_SEND_BYTE_DATA(0x00);
		}
	}
}
void print_line (uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool orientation)//orientation will be false for vertical line and true for horizontal line
{
	// 	x1 += 24;
	// 	x2 += 24;
	if (orientation)
	{	uint8_t j = 0;
		uint8_t buff_leng = (x2-x1) ;
		window_program_start_end_colm_row_addr(x1, y1, x2, y2);
		for (j = 0; j < (buff_leng); j++)
		{
			UC1698U_SEND_BYTE_DATA(0xFF);
			UC1698U_SEND_BYTE_DATA(0xFF);
		}
	}
	else
	{
		unsigned int j = 0;
		unsigned int buff_leng=0;
		window_program_start_end_colm_row_addr(x1, y1, x2, y2);
		buff_leng = (y2-y1);
		for (j = 0; j < (buff_leng); j++)
		{
			UC1698U_SEND_BYTE_DATA(0xF8);
			UC1698U_SEND_BYTE_DATA(0x00);
		}
	}
}


void draw_delete_rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
	uint8_t no_rows = y2-y1, no_col = ((x2-x1)/3);
	for (uint8_t i=0; i<no_rows; i++)
	{
		window_program_start_end_colm_row_addr(x1, (y1+i), x2, (y1+i));
		for (uint8_t j=0; j<no_col; j++)
		{
			UC1698U_SEND_BYTE_DATA(0x00);
			UC1698U_SEND_BYTE_DATA(0x00);
		}
	}
}

void draw_filled_rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
	uint8_t no_rows = y2-y1, no_col = ((x2-x1)/3);
	for (uint8_t i=0; i<no_rows; i++)
	{
		window_program_start_end_colm_row_addr(x1, (y1+i), x2, (y1+i));
		for (uint8_t j=0; j<no_col; j++)
		{
			UC1698U_SEND_BYTE_DATA(0xff);
			UC1698U_SEND_BYTE_DATA(0xff);
		}
	}
}


void window_program_start_end_colm_row_addr (uint8_t start_col_addr, uint8_t start_row_addr, uint8_t end_col_addr, uint8_t end_row_addr)
{
	start_col_addr += 24;
	end_col_addr += 24;
	
	// NOTE THE UC1698U DOES NOT PROVIDE WORKING OF START_ROW_ADDRESS IN WINDOW PROGRAME MODE THEREFORE TO USE THE WINDOW PROGRAME MODE PROVIDE THE START ROW ADDRESS THROUGH SET_ROW_ADDRESS_LSB AND SET_ROW_ADDRESS_MSB COMMANDS
	// AND LET SET_COLUMN_ADDRESS_LSB AND MSB COMMANDS BE EQUALS TO SET_STARTING_COLUMN_ADDRESS COMMAND OF WINDOW PROGRAMING MODE
	// REST OF THE ENDING COLUMN AND ENDING ROW ADDRESS COMMANDS FOR WINDOW COMPARE MODE IS WORKING FINE.
	set_colm_addr(start_col_addr);
	set_row_addr(start_row_addr);
	UC1698U_SEND_CMD(SET_WINDOW_PROGRAM_START_COLOMN_ADDR_CMD);
	UC1698U_SEND_CMD_DATA(start_col_addr);
	UC1698U_SEND_CMD(SET_WINDOW_PROGRAM_END_COLUMN_ADDR_CMD);
	UC1698U_SEND_CMD_DATA(end_col_addr);
	//  	UC1698U_SEND_CMD_DATA(0xF5);
	//  	UC1698U_SEND_CMD_DATA(50);
	UC1698U_SEND_CMD(SET_WINDOW_PROGRAM_END_ROW_ADDR_CMD);
	UC1698U_SEND_CMD_DATA(end_row_addr);
	UC1698U_SEND_CMD(SET_WINDOW_PROGRAME_INSIDE_MODE);

}
void delete_rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
	uint8_t no_rows = y2-y1, no_col = ((x2-x1));
	for (uint8_t i=0; i<no_rows; i++)
	{
		window_program_start_end_colm_row_addr(x1, (y1+i), x2, (y1+i));
		for (uint8_t j=0; j<no_col; j++)
		{
			UC1698U_SEND_BYTE_DATA(0x00);
			UC1698U_SEND_BYTE_DATA(0x00);
		}
	}
}