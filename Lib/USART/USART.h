#ifndef _USART_H_
#define _USART_H_

#define FOSC 16000000UL
#define RX_BUFFER_SIZE 128

void USART_init(unsigned long BAUD);
void USART_send_byte(unsigned char data);
void USART_print_str(const char *data);
void USART_print_strln(const char *data);
void USART_print_int(int data);
void USART_print_float(float data);
//unsigned char USART_received_num();
char USART_received_char();

#endif
