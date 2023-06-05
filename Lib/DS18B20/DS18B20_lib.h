#ifndef DS18B20_H
#define DS18B20_H

#include "oneWire_lib.h"

void ds18b20_configResolution(unsigned char R1, unsigned char R0);
unsigned char *ds18b20_readScratchpad(void);
float ds18b20_getTemp();

#endif