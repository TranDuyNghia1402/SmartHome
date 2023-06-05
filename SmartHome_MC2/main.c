/*
 * main.c
 *
 * Created: 5/30/2023 4:31:15 PM
 * Author: Tran Duy Nghia
 */

#include <mega328p.h>
#include <delay.h>
#include "USART.h"
#include "RFID.h"
#include "SPILib.h"
#include "Servo.h"
#include "ADCLib.h"
#include "CheckValidUID.h"

#define BUZZER PORTD6
#define LED PORTD7

void overGasRateHandler(void);
void cardExceptHandler(void);
void cardNotExceptHandler(void);

unsigned char uid[5];
unsigned char isHome = 0;
unsigned int gasRate = 0;
float temp = 0;

void main(void)
{
    DDRD |= (1 << BUZZER) | (1 << LED);
    ADC_init();
    USART_init(9600);
    servo_init();
    SPI_init();
    RFID_init();
    servo_set_angle(180);
    while (1)
    {
        if(RFID_isCard())
        {
            if (RFID_readCardSerial(uid))
            {
                isHome = checkValidUid(uid);
                if (isHome == 1) {
                    USART_send_byte(0x01);
                    cardExceptHandler();
                }
                else if (isHome == 2) {
                    USART_send_byte(0x02);
                    cardExceptHandler();
                }
                else if (isHome == 0){
                    USART_send_byte(0x00);
                    cardNotExceptHandler();
                }
            }
            RFID_selectTag(uid);
        }
        RFID_halt();
        USART_send_byte(0xFF);
        gasRate = ADC_read_data(A4);
        while (gasRate >= 500) {
            overGasRateHandler();
            USART_send_byte(0x03);
            gasRate = ADC_read_data(A4);
        }
        if (isHome == 1 || isHome == 2) {
            servo_set_angle(70);
            delay_ms(5000);
        }
        isHome = 0;
        servo_set_angle(180);
    }
}

void overGasRateHandler(void)
{
    PORTD |= (1 << BUZZER) | (1 << LED);
    delay_ms(300);
    PORTD &= ~(1 << BUZZER) & ~(1 << LED);
    delay_ms(300);
}
void cardExceptHandler(void)
{
    PORTD |= (1 << BUZZER);
    delay_ms(1000);
    PORTD &= ~(1 << BUZZER);
}
void cardNotExceptHandler(void)
{
    PORTD |= (1 << BUZZER) | (1 << LED);
    delay_ms(1000);
    PORTD &= ~(1 << BUZZER) & ~(1 << LED);
}
