/*
 * SmartGarbageCan.c
 *
 * Created: 5/15/2023 10:41:24 PM
 * Author: Tran Duy Nghia
 */

#include <mega328p.h>
#include <delay.h>
#include "Servo.h"
#include "hc_sr04.h"
#include "USART.h"

#define trigPin PORTD5

int d;

void main(void)
{
    USART_init(9600);
    servo_init();
    HC_SR04_init(trigPin);
    #asm("sei");
    //servo_set_angle(0);
    USART_print_strln("hello, class");
    USART_print_strln("Welcome to our Final Project!");
    while (1)
    {
        HC_SR04_start(trigPin);
        d = HC_SR04_get_distance();
        USART_print_float(d);
        if (d >= 2 && d < 7) {
            servo_set_angle(180);
            delay_ms(3000);
        }
        else
            servo_set_angle(0);
    }
}
