
#define F_CPU 24000000UL
#define _DEBUG 

#include "Dc_suction_declarations.h"
#include "Dc_suction_keypad.c"
#include "Dc_suction_protocol.c"
#include "Dc_suction_STP.c"
#include "Dc_suction_foot_switch.c"
#include "Dc_suction_NTC_read.c"
#include "Dc_suction_timmers.c"
#include "Dc_suction_lcd_backlit.c"
#include "DC_suction_lcd_uc1698.c"
#include "Dc_suction_lcd_pressure_bar.c"
#include "Dc_suction_pwm.c"



int main(void)
{
   	sei();
   	_PROTECTED_WRITE (CLKCTRL.OSCHFCTRLA, ((CLKCTRL_FRQSEL_24M_gc)|(CLKCTRL_AUTOTUNE_bm)));
 	timmer_init();
 	USART1_init(9600); 	 	PORTC.DIR |= (1<<4);
 	PORTC.OUT &= ~(1<<4);				//ams
 _delay_ms(1000);
 
	TCA0_PWM_init();
 	SPI_0_init();
 	AMS_5812_init();
 	motor_sol_gpio_init();
 	LCD_backlit_gpio_init();
 	NTC_init();
 	keypad_gpio_init();
 	lcd_uc1698u_init();
 	foot_switch_init();
 	lcd_pressure_bar_init();
 	lcd_loading_page();
 	lcd_all_icon_update();
 	STP();
 	USART1_sendString("//////////////////////////////////////////////////////////////////");  
	  	
    while (1) 
    {
		
		dc_suction_pressure_main (50);
		keypad_main();
		foot_switch_main();
		//read_ntc();
		LCD_backlit_main();
		dc_suction_protocol_main();
		lcd_uc1698u_main_screen();


		//sensor_reset();          // for time ,motor_on_off button will act as pwm button
    }
}

