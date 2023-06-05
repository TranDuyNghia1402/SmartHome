#ifndef _ADC_LIB_H_
#define _ADC_LIB_H_

#include <mega328p.h>
#include <delay.h>

#define ADC_VREF_TYPE ((0 << REFS1) | (1 << REFS0) | (0 << ADLAR))
#define A0 0
#define A1 1
#define A2 2
#define A3 3
#define A4 4
#define A5 5

static unsigned int data_container = 0;

void ADC_init(void)
{
    //  ADMUX = 0; ADCSRA = 0; DIDR0 = 0;
    ADMUX |= ADC_VREF_TYPE;
    ADCSRA |= (1 << ADEN) | (0 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

unsigned int ADC_read_data(unsigned char ADC_pin)
{
    ADMUX |= ADC_pin;
    DIDR0 |= (1 << ADC_pin);
    delay_ms(10);
    ADCSRA |= (1 << ADSC);
    while(!(ADCSRA & (1 << ADIF)));
    data_container = ADCW;
    ADCSRA |= (1 << ADIF);
    return data_container;
}

#endif
