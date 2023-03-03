#ifndef _LED7SEG_H_
#define _LED7SEG_H_

#include <p18f4620.h>

// Define pinout
#define DATA_PIN1 PORTDbits.RD2
#define CLK_PIN1 PORTDbits.RD3
#define LATCH_PIN1 PORTDbits.RD4

#define DATA_PIN2 PORTDbits.RD5
#define CLK_PIN2 PORTDbits.RD6
#define LATCH_PIN2 PORTDbits.RD7
#define ROOT 10

extern unsigned int LED7_mask[11];

void control_led1(int num1, int num2);
void control_led2(int num1, int num2);
void display_led1(int data);
void display_led2(int data);
void write_8bit1(int num);
void write_8bit2(int num);
void delay_ms_7seg(int value);


#endif