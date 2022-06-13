#ifndef UART_1_AVR128DA64_H_
#define UART_1_AVR128DA64_H_

#include <string.h>

#define USART1_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

void USART1_init(uint16_t);
void USART1_sendChar(char c);
void USART1_sendString(char *str);
char USART1_readChar(void);
void USART1_sendString_without_newline(char *str);


char buff[50];
// Reverses a string 'str' of length 'len'
void reverse(char* str, int len)
{
	int i = 0, j = len - 1, temp;
	while (i < j) {
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++;
		j--;
	}
}

// Converts a given integer x to string str[].
// d is the number of digits required in the output.
// If d is more than the number of digits in x,
// then 0s are added at the beginning.
int intToStr(long x, char str[], int d)
{
	int i = 0;
	while (x) {
		str[i++] = (x % 10) + '0';
		x = x / 10;
	}
	
	// If number of digits required is more, then
	// add 0s at the beginning
	while (i < d)
	str[i++] = '0';
	
	reverse(str, i);
	str[i] = '\0';
	return i;
}

// Converts a floating-point/double number to a string.
void ftoa(double n, char* res, int afterpoint)
{
	// Extract integer part
	int ipart = (int)n;
	
	// Extract floating part
	float fpart = n - (float)ipart;
	
	// convert integer part to string
	int i = intToStr(abs(ipart), res, 0);
	
	// check for display option after point
	if (afterpoint != 0) {
		res[i] = '.'; // add dot
		
		// Get the value of fraction part upto given no.
		// of points after dot. The third parameter
		// is needed to handle cases like 233.007
		fpart = fpart * pow(10, afterpoint);
		
		intToStr(abs((int)fpart), res + i + 1, afterpoint);
	}
}


void USART1_init(uint16_t baud)
{
	PORTC.DIR &= ~PIN1_bm;
	PORTC.DIR |= PIN0_bm;
	
	USART1.BAUD = (uint16_t)USART1_BAUD_RATE(baud);

	USART1.CTRLB |= USART_TXEN_bm|USART_RXEN_bm;
}

void USART1_sendChar(char c)
{
	while (!(USART1.STATUS & USART_DREIF_bm))
	{
		;
	}
	USART1.TXDATAL = c;
}

void USART1_sendString(char *str)
{
	for(size_t i = 0; i < strlen(str); i++)
	{
		USART1_sendChar(str[i]);
	}
	USART1_sendChar('\n');
}

void USART1_sendString_without_newline(char *str)
{
	for(size_t i = 0; i < strlen(str); i++)
	{
		USART1_sendChar(str[i]);
	}
}
char USART1_readChar(void)
{
	while (!(USART1.STATUS & USART_RXCIF_bm))
	{
		;
	}
	return USART1.RXDATAL;
}

void USART1_sendInt(long val)
{
	intToStr(val, buff, 0);
	USART1_sendString(buff);
}

//void USART1_sendFloat(double val)
//{
	//ftoa(val, buff, 2);
	//USART1_sendString(buff);
//}

void USART1_sendFloat(double val, int size)
{
	ftoa(val, buff, size);
	USART1_sendString(buff);
}

#endif