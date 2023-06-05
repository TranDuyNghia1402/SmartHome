#include <mega328p.h>
#include <delay.h>
#include "I2C_lib.h"
#include "USART.h"
#include "oneWire_lib.h"
#include "DS18B20_lib.h"
#include <alcd_twi.h>

#define Ds1307ReadMode   0xD1  // DS1307 ID in read mode
#define Ds1307WriteMode  0xD0  // DS1307 ID in write mode
#define Ds1307SecondRegAddress   0x00   // Address to access Ds1307 SEC register
#define Ds1307DateRegAddress     0x04   // Address to access Ds1307 DATE register
#define Ds1307ControlRegAddress  0x07   // Address to access Ds1307 CONTROL register
#define PCF8574_I2C_ADDRESS 0x27
unsigned char rx;
unsigned char temp = 27;

unsigned char rtc_setTime(unsigned char sec,unsigned char min,unsigned char hour,unsigned char week_day,
                        unsigned char date,unsigned char month,unsigned char year)
{
    TWI_Start();
    TWI_Send_Byte(Ds1307WriteMode);
    TWI_Send_Byte(Ds1307SecondRegAddress);
    TWI_Send_Byte(sec);
    TWI_Send_Byte(min);
    TWI_Send_Byte(hour);
    TWI_Send_Byte(week_day);
    TWI_Send_Byte(date);
    TWI_Send_Byte(month);
    TWI_Send_Byte(year);
    TWI_Stop();
    return 0;
}
unsigned char rtc_getTime(unsigned char time[]) // 0-1 sec    2-3 min         4-5 hour      6-7 week_day
                                                  // 8-9 date   10-11 month     12-13 year
{
    unsigned int i;
    unsigned char temp;
    TWI_Start();
    TWI_Send_Byte(Ds1307WriteMode);
    TWI_Send_Byte(Ds1307SecondRegAddress);
    TWI_Stop();
    TWI_Start();
    TWI_Send_Byte(Ds1307ReadMode);
    for(i=0;i<12;i+=2)
    {
        temp = TWI_Read_Ack();
        time[i] = (0x30+(temp>>4));
        time[i+1] = (0x30+(temp&0x0F));
    }
    temp = TWI_Read_Nack();
    time[12] = (0x30+(temp>>4));
    time[13] = (0x30+(temp&0x0F));
    TWI_Stop();
    return 0;
}

void main(void)
{
    unsigned char time[6];
    TWI_Init(Ds1307WriteMode,0,100000);
    rtc_setTime(0x01,0x37,0x07,0x12,0x31,0x05,0x23); //sec-min-hour-week_day-date-month-year
    USART_init(9600);
    #asm("sei")
    lcd_twi_init(PCF8574_I2C_ADDRESS,16);
    twi_master_init(100);
    TWI_Init(Ds1307WriteMode,0,100000);
    lcd_clear();
    //onewire_init(4);
    //ds18b20_configResolution(0, 0);
    while (1)
    {
        rx = USART_received_char();
        switch (rx)
		{
            case 0x00:
                lcd_printfxy(0,0,"The khong hop le!");
                lcd_printfxy(0,1,"Moi thu lai!    ");
                delay_ms(2000);
			    break;
		    case 0x01:
                lcd_printfxy(0,0,"Xin chao:       ");
                lcd_printfxy(0,1,"Tran Duy Nghia! ");
			    break;
			case 0x02:
                lcd_printfxy(0,0,"Xin chao:       ");
                lcd_printfxy(0,1,"Nguyen Tien Dat!");
			    break;
            case 0x03:
                lcd_printfxy(0,0,"!!! Canh bao !!!");
                lcd_printfxy(0,1,"Phat hien co ga!");
			    break;
            default:
                rtc_getTime(time);
                twi_master_init(100);
                lcd_printfxy(0,0,"%c",time[4]);
                lcd_printfxy(1,0,"%c",time[5]);
                lcd_printfxy(2,0,":");
                lcd_printfxy(3,0,"%c",time[2]);
                lcd_printfxy(4,0,"%c",time[3]);
                lcd_printfxy(5,0,":");
                lcd_printfxy(6,0,"%c",time[0]);
                lcd_printfxy(7,0,"%c",time[1]);
                lcd_printfxy(8,0," hehe:-)");

                lcd_printfxy(0,1, "t: ");
                //temp = ds18b20_getTemp();
                lcd_printfxy(3,1, "27");
                lcd_printfxy(5,1, "C ");
                lcd_printfxy(7,1, "Gas: None");
			    break;
		}
    }
}
