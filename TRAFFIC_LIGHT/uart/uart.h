#ifndef _UART_H_
#define _UART_H_

#include <p18f4620.h>

void init_uart();
void uart_putchar(unsigned char data);
void uart_send_str(const char *str);
void UartSendString(const rom char *str);
unsigned long uart_power_of(int a, int x);
void UartSendNum(long num);
void uart_isr();

#endif