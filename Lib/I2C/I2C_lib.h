#ifndef I2C_LIB_H
#define I2C_LIB_H

#include <mega328p.h>
#include <delay.h>

#define fcpu 16000000
#define BR400   12
#define BR200   32
#define BR100   72

void TWI_Init(char sla,char gcall, char sclock){
    //sla can take value of 1 to 127
    //gcall can take value of 0 or 1
    //sclock <= 400000
    //Assign slave address
    TWAR = (sla<<1) + gcall;
    //Set up the sclock
    TWSR &= 0b11111100;     //TWI prescale  = 1
    TWBR = (unsigned char)((fcpu/sclock -16)/2);
    //Enable TWI, enable TWI interupt, enable ACK
    TWCR = 0b01000101;
}

unsigned char TWI_Start(void){

    TWCR = 0b10100100;
    while (!(TWCR & 0b10000000));
    return (TWSR & 0xF8);
}

unsigned char TWI_Repeated_Start(unsigned char read_address)                /* I2C repeated start function */
{
    int status;                                            /* Declare variable */
    TWCR = 0b10100100;                    /* Enable TWI, generate start condition and clear interrupt flag */
    while (!(TWCR & 0b10000000));                            /* Wait until TWI finish its current job (start condition) */
    status = TWSR & 0xF8;                                    /* Read TWI status register with masking lower three bits */
    if (status != 0x10)                                        /* Check weather repeated start condition transmitted successfully or not? */
    return 0;                                                /* If no then return 0 to indicate repeated start condition fail */
    TWDR = read_address;                                    /* If yes then write SLA+R in TWI data register */
    TWCR = 0b10000100;                            /* Enable TWI and clear interrupt flag */
    while (!(TWCR & 0b10000000));                            /* Wait until TWI finish its current job (Write operation) */
    status = TWSR & 0xF8;                                    /* Read TWI status register with masking lower three bits */
    if (status == 0x40)                                        /* Check weather SLA+R transmitted & ack received or not? */
    return 1;                                                /* If yes then return 1 to indicate ack received */
    if (status == 0x20)                                        /* Check weather SLA+R transmitted & nack received or not? */
    return 2;                                                /* If yes then return 2 to indicate nack received i.e. device is busy */
    else
    return 3;                                                /* Else return 3 to indicate SLA+W failed */
}

void TWI_Stop(void) {
    TWCR = 0b10010100;
}

unsigned char TWI_Send_Byte(unsigned char b){
    TWDR = b;
    TWCR = 0b10000100;
    while (!(TWCR & 0b10000000));
    return (TWSR & 0xF8);
}

unsigned char TWI_Read_Ack()                                            /* I2C read ack function */
{
    TWCR = 0b11000100;                                        /* Enable TWI, generation of ack and clear interrupt flag */
    while (!(TWCR & 0b10000000));                            /* Wait until TWI finish its current job (read operation) */
    return TWDR;                                            /* Return received data */
}

unsigned char TWI_Read_Nack()                                        /* I2C read nack function */
{
    TWCR = 0b10000100;                                        /* Enable TWI and clear interrupt flag */
    while (!(TWCR & 0b10000000));                            /* Wait until TWI finish its current job (read operation) */
    return TWDR;                                            /* Return received data */
}

#endif
