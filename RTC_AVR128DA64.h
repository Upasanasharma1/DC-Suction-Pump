#ifndef RTC_AVR128DA64_H_
#define RTC_AVR128DA64_H_

#include <stdbool.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include "UART_1_AVR128DA64.h"
//#include "MotorCalculation.h"


#define dirPin 0
#define stepPin 1

//#define BACKWARD (digitalWrite(dirPin, LOW))
//#define FORWARD (digitalWrite(dirPin, HIGH))

#define BACKWARD (PORTB_OUT &= ~(1 << dirPin))
#define FORWARD (PORTB_OUT |= (1 << dirPin))

#define MOTOR_DIR (PORTB_DIR |= (1 << dirPin))
#define STEPPIN_DIR (PORTB_DIR |= (1 << stepPin))

#define MOTOR_ON   (PORTB_OUT |= (1 << stepPin))
#define MOTOR_OFF  (PORTB_OUT &= ~(1 << stepPin))

//// RTC prototypes..
//void clock_config_rtc(void);
////void RTC_init(void);
//void set_rtc(uint16_t);

//char buff[20];
unsigned long millis;
//bool toggle_motor = 0;
int step_number = 0;           // Step-sequence (0 - 4)
bool DoseComplete = false;

/************************************************************************/
/* Initialization
Before enabling the RTC peripheral and the desired actions (interrupt requests and output events), the source clock
for the RTC counter must be configured to operate the RTC.

Configure the Clock CLK_RTC
To configure the CLK_RTC, follow these steps:
1. Configure the desired oscillator to operate as required, in the Clock Controller (CLKCTRL) peripheral.
2. Write the Clock Select (CLKSEL) bit field in the Clock Selection (RTC.CLKSEL) register accordingly.
The CLK_RTC clock configuration is used by both RTC and PIT functionality.

Configure RTC
To operate the RTC, follow these steps:
1. Set the compare value in the Compare (RTC.CMP) register, and/or the overflow value in the Period
(RTC.PER) register.
2. Enable the desired interrupts by writing to the respective interrupt enable bits (CMP, OVF) in the Interrupt
Control (RTC.INTCTRL) register.
3. Configure the RTC internal prescaler by writing the desired value to the Prescaler (PRESCALER) bit field in
the Control A (RTC.CTRLA) register.
4. Enable the RTC by writing a ‘1’ to the RTC Peripheral Enable (RTCEN) bit in the RTC.CTRLA register.
Note: The RTC peripheral is used internally during device start-up. Always check the Synchronization Busy bits in
the Status (RTC.STATUS) and Periodic Interrupt Timer Status (RTC.PITSTATUS) registers, and on the initial
configuration.
                                                                     
*/
/************************************************************************/

void clock_config_rtc()
{
	RTC_CLKSEL = RTC_CLKSEL_OSC32K_gc;
}

// Time < 7.99 sec, because of prescaler 4.
uint16_t set_rtc(uint16_t time)
{
	return (((time * 32768) / 32000) + 0.5);
}

void RTC_init(uint16_t time)
{
	sei();
	//USART1_sendString("RTC initializing..");
	/* Set period */
	RTC.PER = set_rtc(time);
	/* 32.768kHz Internal Crystal Oscillator (XOSC32K) */
	RTC.CLKSEL = RTC_CLKSEL_OSC32K_gc;

	RTC.CTRLA = RTC_PRESCALER_DIV32_gc /* 4 */
	| RTC_RTCEN_bm /* Enable: enabled */
	| RTC_RUNSTDBY_bm; /* Run In Standby: enabled */

	/* Enable Overflow Interrupt */
	RTC.INTCTRL |= RTC_OVF_bm;
	
	USART1_sendString("RTC set.");
}


ISR(RTC_CNT_vect)
{	
	//USART1_sendChar('a');
	//if (!DoseComplete)
	//{
		//if (step_number > 3)
		//{
			//step_number = 0;
		//}
		//
		//if (step_number == 0)
		//{
			//PORTE.OUT |= (0x0A);
			//PORTE.OUT |= (1 << 0);
		//}
		//else if (step_number == 1)
		//{
			//PORTE.OUT |= (0x0A);
			//PORTE.OUT &= ~(1 << 2);
		//}
		//else if (step_number == 2)
		//{
			//PORTE.OUT |= (0x0A);
			//PORTE.OUT &= ~(1 << 0);
		//}
		//else if (step_number == 3)
		//{
			//PORTE.OUT |= (0x0A);
			//PORTE.OUT |= (0xFF << 0);
		//}
		//step_number++;
		//motor_steps++;
	//}
	//USART1_sendString("RTC_CNT_vect");
	//_delay_ms(1);
	//intToStr(millis,buff,10);
	//USART1_sendString(buff);
	
	RTC_INTFLAGS |= (1 << 0);
}

void GO_TO_SLEEP()
{
	sleep_enable();
	set_sleep_mode(SLEEP_MODE_STANDBY);
	sleep_cpu();
}


// User function prototypes..
//void set_millis(void);

/************************************************************************/
/* Initialization
To start using the timer/counter in a basic mode, follow these steps:
1. Write a TOP value to the Period (TCAn.PER) register.
2. Enable the peripheral by writing a ‘1’ to the Enable (ENABLE) bit in the Control A (TCAn.CTRLA) register.
The counter will start counting clock ticks according to the prescaler setting in the Clock Select (CLKSEL) bit
field in TCAn.CTRLA.
3. Optional: By writing a ‘1’ to the Enable Counter Event Input A (CNTAEI) bit in the Event Control
(TCAn.EVCTRL) register, events are counted instead of clock ticks.
4. The counter value can be read from the Counter (CNT) bit field in the Counter (TCAn.CNT) register.                                                                     */
/************************************************************************/
// TCB0 as millis..-------------------------
void set_millis(void)
{
	sei();
	TCB0_CCMP = 3999;				// Write a TOP value to the Compare/Capture (TCBn.CCMP) register
	
	TCB0_CTRLB |= (0x0 << 0);
	TCB0_INTCTRL |= (1<<0);
	
	TCB0_CTRLA |= (1<<0)|(0x0 <<1); // ENABLE bit in the Control A (TCBn.CTRLA) register,
}

ISR(TCB0_INT_vect)
{
	millis++;
	TCB0_INTFLAGS |= (1<<0);
}

#endif