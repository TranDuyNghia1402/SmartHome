#ifndef HC_SR04_H
#define HC_SR04_H

#include <mega328p.h>
#include <delay.h>

#define EchoPin PORTD2

volatile unsigned char state = 0;
volatile unsigned char count = 0;
volatile float distance = 0;

interrupt [TIM0_OVF] void tim0_isr (void)
{
    count++;
}

interrupt [EXT_INT0] void int_0_isr (void)
{
    if (state)
    {
        TCCR0B = 0;
        distance = 1.0976 * (count * 256 + TCNT0); // distance = 343 * 100 / 2 * (count * 256) + TCNT0 * 1024 / (16 * 10^6) 
        TCNT0 = 0;
        count = 0;
        state = 0;
    }
    else
    {
        TCCR0B |= (1 << CS02) | (1 << CS00);
        state = 1;
    }
}

void HC_SR04_init(unsigned char TrigPin)
{
    /*external interrupt*/
    EICRA |= (1 << ISC00);
    EIMSK |= (1 << INT0);
    /*------------------*/
    DDRD |= (1 << TrigPin) & ~(1 << EchoPin);
    PORTD |= (1 << EchoPin);
    /*timer 0*/
    TCCR0A = 0; TCCR0B = 0;
    TIMSK0 |= (1 << TOIE0);
    TCNT0 = 0;
    /*-------*/
    //#asm("sei");
}

void HC_SR04_start(unsigned char TrigPin)
{
    PORTD |= (1 << TrigPin);
    delay_us(20);
    PORTD &= ~(1 << TrigPin);
}

float HC_SR04_get_distance()
{
    return distance;
}

#endif
