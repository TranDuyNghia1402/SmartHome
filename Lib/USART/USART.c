#include <mega328p.h>
/*#include <stdio.h>*/
#include "USART.h"

//volatile static unsigned char check_busy = 1;
volatile static unsigned char rx_buffer[RX_BUFFER_SIZE] = {0};
volatile static unsigned char rx_index = 0;
//volatile static unsigned char test = 0;

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

int intToStr(int x, char str[], int d)
{
    int i = 0;
    do
	{
        str[i++] = (x % 10) + '0';
        x = x / 10;
	}
	while (x);
    while (i < d)
        str[i++] = '0';
    reverse(str, i);
    str[i] = '\0';
    return i;
}

void ftstr(float n, char* res)
{
    int ipart = (int)n;
    float fpart = n - (float)ipart;
    int i = intToStr(ipart, res, 0);
    res[i] = '.'; // add dot
    fpart = fpart * 10*10*10*10;
    intToStr((int)fpart, res + i + 1, 2);
}

interrupt [USART_RXC] void rxc_isr (void)
{
    rx_buffer[rx_index] = UDR0;
    rx_index++;
    if (rx_index >= RX_BUFFER_SIZE)
    {
        rx_index = 0;
    }
}
/*
interrupt [USART_TXC] void txc_isr (void)
{
    check_busy =  1;
}
*/
void USART_init(unsigned long BAUD)
{
    /*  Set baud rate   */
    unsigned long ubrr = FOSC / (16 * BAUD) - 1;
    UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	/*  Enable Transmitter and Receiver  */
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
    /* Enable RX_UART interrupt  */
    UCSR0B |= (1 << RXCIE0) | (0 << TXCIE0);
    /*  Set frame format: 8-bit data, 1stop bit   */
    /*  Asynchronous USART, non-parity mode   */
    UCSR0C |= (0 << UMSEL01) | (0 << UMSEL00);
    UCSR0C |= (0 << UPM01) | (0 << UPM00);
    UCSR0C |= (0 << USBS0);
    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
    #asm("sei");
}

void USART_send_byte(unsigned char data)
{
    while(!(UCSR0A & (1 << UDRE0)));
    //while (check_busy == 0);
    //check_busy = 0;
    UDR0 = data;
}

void USART_print_str(const char *data)
{
    while(*data)
        USART_send_byte(*data++);
    //USART_send_byte('\r');
}

void USART_print_strln(const char *data)
{
    while(*data)
        USART_send_byte(*data++);
    USART_send_byte('\r');
}

void USART_print_int(int data)
{
    char buffer[10];
    intToStr(data, buffer, 0);
    USART_print_strln(buffer);
}

void USART_print_float(float data)
{
    char buffer[10];
    ftstr(data, buffer);
    USART_print_strln(buffer);
}
/*
unsigned char USART_received_num(void)
{
    return test - 48;
}
*/
unsigned char USART_received_char()
{
    return UDR0;
}



