/*
 * Uc1698u controller library
 * for JHD240160-G56PQWD display
 *
 * _UC1698U.h
 *
 * Created: 04/02/2022
 *  Author: shivam gupta @ agvahealthcare
 */ 


#ifndef UC1698U_H_
#define UC1698U_H_

#include <util/delay.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "SPI_0_AVR128DA64.h"
#include "UART_1_AVR128DA64.h"


#define UC1698U_DISPLAYSIZE_X	240
#define UC1698U_DISPLAYSIZE_Y	160

#define start_row_addr_of_display 0
#define start_column_addr_of_display 24			//as display considers 1 address for 3 pixels therefore, there are only (103-24=(0-79)) address spaces because (240/3=80) 
#define ending_column_address_of_display 103
#define ending_row_address_of_display 160

#define cs_port		PORTA
#define cs_pin		(1<<7)
#define a0_port		PORTB
#define a0_pin		(1<<4)
#define rst_port	PORTC
#define rst_pin		(1<<5)

#define UC1698U_CS_ENABLE()		(cs_port.OUT &= ~cs_pin)	//WHEN CS0 IS LOW THE CHIS IS SELECTED CS1 IS NOT USED IN SPI MODE
#define UC1698U_CS_DISABLE()	(cs_port.OUT |= cs_pin)
#define UC1698U_CMDMODE()		(a0_port.OUT &= ~a0_pin)		// WHEN CD OR A0 PIN IS 0 THEN COMMAND WILL BE SEND ELSE DATA
#define UC1698U_DATAMODE()		(a0_port.OUT |= a0_pin)
#define UC1698U_CLRRST()		(rst_port.OUT |= rst_pin)			//RST MUST BE AT VDD WHEN NOT USED
#define UC1698U_SETRST()		(rst_port.OUT &= ~rst_pin)
#define UC1698U_SERIALWRITE(x)		SPI_0_send_char(x)

#define UC1698U_COLOR_BLACK		1
#define UC1698U_COLOR_WHITE		0

uint8_t pre_def[3][2] = { {0xF8,0x00}, {0x07,0xE0}, {0x00,0x1F} };

enum UC1698U_INSTRUCTION_TABLE
{
	NOP = 0xE3,						// no operation
	RESET = 0xE2,					// system reset

	DISPLAY_OFF = 0b10101110,		// 	(D0)DC[2]: Display ON/OFF (Default 0: OFF)
	DISPLAY_ON	= 0b10101101,		// 	(D1)DC[3]: Gray-shade Modulation mode; 0 : On/Off mode; (DEFAULT)1 : 32-shade Mode
	// 	(D2)DC[4]: Green Enhance Mode. Only valid in 4K-color mode; 0 : Enable. Allows an extra display bit for green color; (DEFAULT)1 : Disable

	SET_POWER_CONTROL = 0b00101011,	// (D0)PC[0]: (DEFAULT)0b: LCD U 13nF    1b: 13nF < LCD U 22nF
	// (D1)PC[1]: 0b: External VLCD (DEFAULT)1b: Internal VLCD (10x charge pump)

	PARTIALMODE_ON = 0b10000101,	// 	(D0)LC[8] : Partial Display Control;
	PARTIALMODE_OFF = 0b10000100,	//	(DEFAULT)0: Disable Mux-Rate = CEN+1 (DST, DEN not used)
	// 	1: Enabled Mux-Rate = DEN-DST+1+LC[0] x (FLT+FLB) x 2

	INVERSE_DISPLAY_ON = 0b10100111, // (D0)DC[0]: PXV: Pixels Inverse. Bit-wise data inversion. (Default 0: OFF)
	INVERSE_DISPLAY_OFF = 0b10100110,
	
	ALL_PIXEL_ON = 0b10100101,			// (D0)DC[1]: APO: All Pixels ON (Default 0: OFF)
	ALL_PIXEL_ON_DISABLE = 0b10100100,

	//Display Data RAM Column Address (counted in RGB triplet); CA value range: 0~127
	//in case of JHD240160-G56PQWD 1 column address represents 3 led segments and as it has 240 columns therefore, CA value range: 0-79
	SET_COLUMN_ADDR_LSB_CMD = 0b00000000,	// use D3-D0 to represent the 4bit lsb of the column address
	SET_COLUMN_ADDR_MSB_CMD = 0b00010000,	// use D2-D0 to represent the 3bit msb of the column address

	// Display Data RAM Row Address; Possible value = 0~159
	SET_ROW_ADDR_LSB_CMD = 0b01100000,	// use D3-D0 to represent 4bit lsb of the row address
	SET_ROW_ADDR_MSB_CMD = 0b01110000,	// use D3-D0 to represent 4bit msb of the row address

	// These are 16 bit commands starting 8 bits are commands then 8 bit data in form of address needs to be send
	PARTIAL_DISPLAY_START_ADDR = 0xF2,		// after sending the command send 7 bit DST address (DST[6:0])
	PARTIAL_DISPLAY_END_ADDR = 0xF3,		// after sending the command send 7 bit DEN address (DEN[6:0])
	SET_COM_END = 0xF1,						// after sending the command sent 7bit COM END address (CEN[6:0])

	// Scroll the displayed image up by SL rows. The valid SL value is between 0 (for no scrolling) and (159 – 2x(FLT+FLB)). Setting SL outside of
	// This range causes undefined effect on the displayed image.
	SET_SCROLL_LINE_LSB = 0b01000000,	// use D3:D0 to represent lsb of scroll line
	SET_SCROLL_LINE_MSB = 0b01010000,	// use D3:D0 to represent msb of scroll line
	
	// Fixed Lines. The first FLTx2 lines and the last FLBx2 lines (relative to CEN) of each frame are fixed and are not affected by scrolling (SL).
	// When FLT and/or FLB are non-zero, the screen is effectively separated into three regions: one scrollable, surrounded by two non-scrollable regions.
	// it is a 2 byte command msb byte for the command and lsb byte for data
	SET_FIXED_LINES = 0x90,	// after sending this command send the 8 bit data in which msb represents the 4 bit FLT, and lsb represents 4 bit FLB

	N_LINE_INVERSION_COMMAND = 0xC8, // command for N line inversion
	N_LINE_INVERSION_DATA = 0b00000101,	// (D2-D0)NIV[2:0]: 000b: 11 lines 001b: 19 lines 010b: 21 lines 011b: 25 lines
	//					100b: 29 lines (DEFAULT)101b: 31 lines 110b: 37 lines 111b: 43 lines
	// (D3)NIV[3]: 0b: non-XOR (DEFAULT)1b: XOR
	// (D4)NIV[4]: 0b: Disable NIV (DEFAULT)1b: Enable NIV

	SET_BIAS_RATIO = 0b11101011,		//(D1-D0)Bias Ratio. The ratio between VLCD and VBIAS.
	//00b: 5 01b: 10
	//10b: 11 (DEFAULT)11b: 12
	
	//UC1698u supports on-chip swapping of R-B data mapping to the SEG drivers.
	SET_COLOUR_PATTERN_BGR = 0xD0,	//DEFAULT
	SET_COLOUR_PATTERN_RGB = 0xD1,
	
	SET_COLOUR_MODE = 0b11010110,	// (D1-D0)LC[7:6] : Color and input mode
	//		when DC[4]=1: (GREEN ENHANCED MODE IS DISABLED)
	//					01b : 4K color mode. 4R-4G-4B (12-bit/RGB)
	//					(DEFAULT)10b : 64K color mode. 5R-6G-5B (16-bit/RGB)
	//		when DC[4]=0:
	//					01b : 4K color mode. 4R-5G-3B (12-bit/RGB)
	//					10b : 64K color mode. 5R-6G-5B (16-bit/RGB)
	
	SET_COM_SCAN_FUNCTION = 0b11011000,	// COM Scan Function
	// (D0)CSF[0]: Interlace Scan Function
	//				(DEFAULT)0b: LRM sequence: AEBCD-AEBCD
	//				1b: LRM sequence: AEBCD-EBCDA
	// (D1)CSF[1]: FRC function
	//			(DEFAULT)0: Disable FRC
	//			1: Enable FRC
	// (D2)CSF[2]:	Shade-1 / Shade-30 option
	//				(DEFAULT)0: Dither directly on input data (SRAM Change)
	//				1: PWM (Pulse-width modulation) on SEG output stage

	SET_LCD_MAPPING_CONTROL = 0b11000100,	// This command is used for programming LC[2:0] to control COM (row) mirror (MY), SEG (column) mirror (MX).
	// (D2)LC[2] controls Mirror Y (MY): MY is implemented by reversing the mapping order between RAM and COM
	// 	         electrodes. The data stored in RAM is not affected by the MY action. MY will have immediate effect on
	// 	         the display image. (Default 0: OFF).
	// (D1)LC[1] controls Mirror X (MX): MX is implemented by selecting the CA or 127-CA as write/read (from host
	//           interface) display RAM column address so this function will only take effect after rewriting the RAM data.
	//			  (Default 0: OFF).
	// (D0)LC[0] controls whether soft icon sections (2xFLT, 2xFLB) are displayed during partial display mode.
	//			  (Default 0: OFF).

	SET_LINE_RATE = 0b10100010,			//(D1-D0)Program LC [4:3] for line rate setting (Frame-Rate = Line-Rate / Mux-Rate). The line rate is automatically
	// 	scaled down by 2/3, 1/2, 1/3 and 1/4 at Mux-Rate = 108, 80, 56, and 40.
	// 	The following are line rates at Mux Rate = 109 ~ 160.
	// 	00b: 25.2 Klps 01b: 30.5 Klps (DEFAULT)10b: 37.0 Klps 11b: 44.8 Klps
	// 	In On/Off Mode
	// 	00b: 8.5 Klps 01b: 10.4 Klps (DEFAULT)10b: 12.6 Klps 11b: 15.2 Klps

	SET_RAM_ADDRESS_CONTROL = 0b10001001,	// Program registers AC[2:0] for RAM address control.
	// (D0)AC[0]: WA, Automatic column/row wrap around.
	//		      0: CA or RA (depends on AC[1]= 0 or 1) will stop incrementing after reaching boundary
	//		      (DEFAULT)1: CA or RA (depends on AC[1]= 0 or 1) will restart, and RA or CA will increment by one step.
	// (D1)AC[1]: Auto-Increment order
	//            (DEFAULT)0 : column (CA) increment (+1) first until CA reaches CA boundary, then RA will increment by (+/-1).
	//            1 : row (RA) increment (+/-1) first until RA reach RA boundary, then CA will increment by (+1).
	// (D2)AC[2]: RID, row address (RA) auto increment direction ( 0/1 = +/- 1 )(DEFAULT 0)
	//            When WA=1 and CA reaches CA boundary, RID controls whether row address will be adjusted by +1 or -1.

	SET_VBIAS_POT_COMMAND = 0x81,    //set vop
	SET_VBIAS_POT_DATA = 75,		//set vop=16.5v?72?, 15.8V?46?16.3V(64)
	
	SET_TEMP_COMPENSATION = 0b00100100,	//Temperature compensation curve definition:
	//	(DEFAULT)00b = -0.00%/oC 01b = -0.05%/oC 10b = -0.15%/oC 11b = -0.25%/oC

	SET_MTP_CONTROL_COMMAND		= 0xB8,			//MTP Programming Control:
	MTP_CONTROL_DATA			= 0x10,			//MTPC[2:0] : MTP command
	//			(DEFAULT)000 : Idle 001 : Read
	//			 010 : Erase 011 : Program
	//			1xx : For UltraChip’s _DEBUG use only
	//MTPC[3] : MTP Enable (automatically cleared after each MTP command)
	//MTPC[4] : Ignore/Use MTP. 0: Ignore (DEFAULT)1: Use
	
	SET_WINDOW_PROGRAME_INSIDE_MODE = 0b11111000,	//(D0)AC[3]=0: Inside Mode
	SET_WINDOW_PROGRAME_OUTSIDE_MODE = 0b11111001,		//When Window Programming is under “Inside” mode , the CA and RA increment and wrap-around will be performed automatically around the boundaries as defined by registers WPC0, WPC1, WPP0, and
	//WPP1, so that the CA/RA address will stay within the defined window of SRAM address, and therefore allow effective data update within the window.
	//(D0)AC[3]=1: Outside Mode
	//When Window Programming is under “Outside” mode, the CA and RA increment and wrap-around boundary will cover the entire UC1698u SRAM map (CA: 0~127, RA:0~159). However, when CA/RA
	//points to a memory location within the window defined by registers WPC0, WPC1, WPP0, and WPP1,the SRAM data update operation will be suspended, the existing data will be retained and the input data
	//will be ignored.
	
	SET_WINDOW_PROGRAM_START_ROW_ADDR_CMD = 0xF5,		//after sending the command send the 8bit addr
	SET_WINDOW_PROGRAM_START_COLOMN_ADDR_CMD = 0xF4,
	SET_WINDOW_PROGRAM_END_ROW_ADDR_CMD = 0xF7,
	SET_WINDOW_PROGRAM_END_COLUMN_ADDR_CMD = 0xF6
};


// void UC1698U_SEND_CMD_DATA(uint8_t C);
 void UC1698U_gpio_init(void);
// void UC1698U_SEND_DATA(uint8_t _data, uint8_t dataSize);
 void UC1698U_SEND_CMD(enum UC1698U_INSTRUCTION_TABLE cmd); void UC1698U_SEND_CMD_DATA(uint8_t);	//TO SEND DATA OF COMMANDS WICH ARE OUT OF THE SCOPE OF ENUM void UC1698U_SEND_ARRAY_DATA ( uint8_t _data[], int dataSize);

 void set_row_addr (uint8_t row_addr);		//for setting the initial row and colounm address void set_colm_addr (uint8_t colm_addr); void window_program_start_end_colm_row_addr (uint8_t, uint8_t, uint8_t, uint8_t);


 void set_all_pixel_off (void);
 void set_all_pixel_on (void);

// void UC1698U_SEND_BYTE_DATA (uint8_t);

void UC1698U_init(void);
void print_icon (uint8_t, uint8_t, uint8_t *, uint8_t, uint8_t);
void print_line (uint8_t,uint8_t,uint8_t,uint8_t,bool);void print_number (uint8_t x, uint8_t y, uint8_t *font, uint8_t height, uint8_t width, uint8_t no);
void draw_delete_rectangle (uint8_t,uint8_t,uint8_t,uint8_t);void draw_filled_rectangle(uint8_t, uint8_t, uint8_t, uint8_t);

#endif
