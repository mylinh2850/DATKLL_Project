#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <p18f4620.h>

#define PORT_BUTTON		PORTC
#define TRIS_BUTTON		TRISC
#define	MAX_COL			4
#define MAX_ROW			4

extern unsigned int key_code[];



void init_key_matrix();
void init_key_matrix_with_uart_i2c();
void scan_key_matrix();
void scan_key_matrix_with_uart_i2c();
void button_process();
void button_delay_ms(int value);
void scan_key_matrix_demo();

unsigned char isButtonMode();
unsigned char isButtonModeHold();
unsigned char isButtonEnter();
unsigned char isButtonEnterHold();
unsigned char isButtonIncrease();
unsigned char isButtonDecrease();
#endif