#ifndef ADC_AVR128DA64_H_
#define ADC_AVR128DA64_H_

#include <avr/io.h>

#define channel_0 0x00
#define channel_1 0x01
#define channel_2 0x02
#define channel_3 0x03
#define channel_4 0x04
#define channel_5 0x05
#define channel_6 0x06
#define channel_7 0x07
#define channel_8 0x08
#define channel_9 0x09
#define channel_10 0x0A
#define channel_11 0x0B
#define channel_12 0x0C
#define channel_13 0x0D
#define channel_14 0x0E
#define channel_15 0x0F

#define channel_16 0x10
#define channel_17 0x11
#define channel_18 0x12
#define channel_19 0x13
#define channel_20 0x14
#define channel_21 0x15



// ADC functions..
void ADC0_init(void)
{
	VREF.ADC0REF = 0x5 | (1 << 7);
	ADC0.CTRLB = 0x04;						// to accumulate 16 results
	ADC0.CTRLC = ADC_PRESC_DIV4_gc;        /* CLK_PER divided by 4 */
	ADC0.CTRLA = ADC_ENABLE_bm             /* ADC Enable: enabled */
	| ADC_RESSEL_12BIT_gc;      /* 12-bit mode */
	//ADC0.MUXPOS = ADC_MUXPOS_AIN3_gc;      /* Select ADC channel AIN3 <-> PD3 */
}

/* This function starts the ADC conversions*/
void ADC0_start(void)
{
	/* Start ADC conversion */
	ADC0.COMMAND = ADC_STCONV_bm;
}

/* This function returns the ADC conversion result */
uint16_t ADC0_read(char pin)
{
	ADC0.MUXPOS = pin;
	ADC0_start();
	/* Wait for ADC result to be ready */
	while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));
	/* Clear the interrupt flag by reading the result */
	return ADC0.RES;
}


#endif