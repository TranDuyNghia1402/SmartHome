#include <mega328p.h>
#include "SPILib.h"

void SPI_init()
{
    DDRB = (DDRB & 0xC3) | 0x2C;
    SPCR |= 0x50;
}

unsigned char SPI_Transfer(unsigned char sData)
{
    unsigned char xData;
    //PORTB &= ~(0x04);
    SPDR = sData;
    while (!(SPSR & 0x80));
    xData = SPDR;
    //PORTB |= 0x04;
    return xData;
}