#include <mega328p.h>
#include <delay.h>
#include "DS18B20.h"
#include "oneWire_lib.h"

void ds18b20_configResolution(unsigned char R1, unsigned char R0)
{
    unsigned char resolution = 0b00011111;
    resolution |= (R1 << 6)  | (R0 << 5);
    #if onewire_STOPINTERRUPTONREAD == 1
	#asm ("cli")
	#endif
    onewire_reset();
    onewire_writebyte(onewire_CMD_SKIPROM);
    onewire_writebyte(DS18B20_CMD_WSCRATCHPAD);

    onewire_writebyte(0x55);
    onewire_writebyte(0x05);
    onewire_writebyte(resolution);

    onewire_writebyte(DS18B20_CMD_CPYSCRATCHPAD);
    while(!onewire_readbit());
    #if onewire_STOPINTERRUPTONREAD == 1
	#asm ("sei")
	#endif
}

unsigned char *ds18b20_readScratchpad(void)
{
    unsigned char scratchpad[8];
    unsigned char idx = 0;
    #if onewire_STOPINTERRUPTONREAD == 1
	#asm ("cli")
	#endif
    onewire_reset();
    onewire_writebyte(onewire_CMD_SKIPROM);
    onewire_writebyte(DS18B20_CMD_RSCRATCHPAD);
    for (idx = 0; idx < 8; idx++)
    {
        scratchpad[idx] = onewire_readbyte();
    }
    #if onewire_STOPINTERRUPTONREAD == 1
	#asm ("sei")
	#endif

    return scratchpad;
}

/*
 * get temperature
 */
float ds18b20_getTemp() {
	int temperature_l;
	int temperature_h;
	float retd = 0;

	#if onewire_STOPINTERRUPTONREAD == 1
	#asm ("cli")
	#endif

	onewire_reset(); //reset
	onewire_writebyte(onewire_CMD_SKIPROM); //skip ROM
	onewire_writebyte(DS18B20_CMD_CONVERTTEMP); //start temperature conversion

	while(!onewire_readbit()); //wait until conversion is complete

	onewire_reset(); //reset
	onewire_writebyte(onewire_CMD_SKIPROM); //skip ROM
	onewire_writebyte(DS18B20_CMD_RSCRATCHPAD); //read scratchpad

	//read 2 byte from scratchpad
	temperature_l = onewire_readbyte();
	temperature_h = onewire_readbyte();

	#if onewire_STOPINTERRUPTONREAD == 1
	#asm ("sei")
	#endif

	//convert the 12 bit value obtained
	retd = ((temperature_h << 8) + temperature_l) * 0.0625;

	return retd;
}