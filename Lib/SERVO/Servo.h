/*
    DDRB: Use for servo at PORTB1, no need interrupt
    DDRD: Use for servo at PORTD5, need interrupt function
*/

#ifndef SERVO_H
#define SERVO_H

#include <mega328p.h>

unsigned char goc = 0;
volatile unsigned int counter = 0;

interrupt [TIM1_OVF] void tim1_ovf_isr(void)
{
    PORTD |= (1 << PORTD5);
    counter++;
}

interrupt [TIM1_COMPA] void tim1_compa_isr(void)
{
    PORTD &= ~(1 << PORTD5);
}

void servo_init()
{
    //DDRB |= (1 << DDB1);
    DDRD |= (1 << DDD5);
    TCCR1A = 0; TCCR1B = 0; TIMSK1 = 0;
    TCCR1A |= (1 << WGM11); TCCR1B |= (1 << WGM13) | (1 << WGM12);
    TCCR1A |= (1 << COM1A1);
    TCCR1B |= (1 << CS11);
    OCR1AH = 0x03;
    OCR1AL = 0xE8;
    ICR1H = 0x9C;
    ICR1L = 0x40;
    TIMSK1 |= (1 << OCIE1A) | (1 << TOIE1);
    #asm("sei");
}

void servo_set_angle(unsigned int x)
{
    x = 1000 + x * 21;
    if (goc != x) {
        OCR1AH = x >> 8;
        OCR1AL = x & 0x000000FF;
        goc = x;
    }
    else {
        return;
    }
}

#endif
