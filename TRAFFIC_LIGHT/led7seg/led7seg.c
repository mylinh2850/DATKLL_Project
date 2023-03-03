#include "led7seg.h"

void write_8bit1(int num) {
    int i = 0;
    int Data_bit;
    unsigned int Temp = LED7_mask[num];
    CLK_PIN1 = 0;
    for (i; i < 8; i++) {
        Data_bit = Temp & 0x80;
        Temp = Temp << 1;
        DATA_PIN1 = (Data_bit == 0x80);
        CLK_PIN1 = 1;
        delay_ms_7seg(1);
        CLK_PIN1 = 0;
    }
}

void write_8bit2(int num) {
    int i = 0;
    int Data_bit;
    unsigned int Temp = LED7_mask[num];
    CLK_PIN2 = 0;
    for (i; i < 8; i++) {
        Data_bit = Temp & 0x80;
        Temp = Temp << 1;
        DATA_PIN2 = (Data_bit == 0x80);
        CLK_PIN2 = 1;
        delay_ms_7seg(1);
        CLK_PIN2 = 0;
    }
}

void display_led1(int data) {
    int data1 = data / 10;
    int data2 = data % 10;

    if (data1 == 0) {
        data1 = ROOT;
    }

    if (data == 100) {
        data1 = ROOT;
        data2 = ROOT;
    }

    control_led1(data1, data2);
}

void display_led2(int data) {
    int data1 = data / 10;
    int data2 = data % 10;

    if (data1 == 0) {
        data1 = ROOT;
    }

    if (data == 100) {
        data1 = ROOT;
        data2 = ROOT;
    }

    control_led2(data1, data2);
}

void control_led1(int num1, int num2) {
    LATCH_PIN1 = 0;
    // Write Led 1
    write_8bit1(num2);
    write_8bit1(num1);
    
    LATCH_PIN1 = 1;
    delay_ms_7seg(1);
    LATCH_PIN1 = 0;
    delay_ms_7seg(1);
}

void control_led2(int num1, int num2) {
    LATCH_PIN2 = 0;
    // Write Led 1
    write_8bit2(num2);
    write_8bit2(num1);

    LATCH_PIN2 = 1;
    delay_ms_7seg(1);
    LATCH_PIN2 = 0;
    delay_ms_7seg(1);
}

void delay_ms_7seg(int value) {
    int i, j;
    for (i = 0; i < value; i++)
        for (j = 0; j < 1; j++);
}

