#include "main.h"
// Noi khai bao hang so
#define PORTD_OUT   PORTD
#define TRISD_OUT   TRISD

void init_output(void);
// Noi khai bao bien toan cuc
unsigned int LED7_mask[11] = {~0x3F, ~0x06, ~0x5B, ~0x4F, ~0x66, ~0x6D, ~0x7D, ~0x07, ~0x7F, ~0x6F, ~0x00};
// Khai bao cac ham co ban IO
void init_system(void);
void init_leds(void);
void delay_ms(int value);

// Den giao thong
#define     INIT_SYSTEM         255
#define     INIT_TRAFFIC        1
#define     PHASE1              2
#define     PHASE2              3
#define     PHASE3              4
#define     PHASE4              5
#define     WAIT_TRAFFIC        6

#define     MAN_INIT            10
#define     MAN_PHASE1          11
#define     MAN_PHASE2          12
#define     MAN_PHASE3          13
#define     MAN_PHASE4          14

#define     TUN_INIT            20
#define     TUN_PHASE1          21
#define     TUN_PHASE2          22
#define     TUN_PHASE3          23

#define     UART_INIT           30
#define     UART_RED            31
#define     UART_GREEN          32
#define     UART_YELLOW         33
#define     UART_WAIT           34
#define     UART_CONFORM_RED        52
#define     UART_CONFORM_GREEN      47
#define     UART_CONFORM_YELLOW     59
#define     UART_RED_COMPLETE       37
#define     UART_GREEN_COMPLETE     38
#define     UART_YELLOW_COMPLETE    39

#define     UART_ERROR              200
#define     INIT_UART_ERROR         201
#define     UART_ERROR_YELLOW       202

#define     WAIT_MENU           40
#define     INIT_MENU           41

#define     MENU_1              42
#define     MENU_2              43
#define     MENU_3              44
#define     MENU_4              45

#define     MENU_MAN_1          50
#define     MENU_MAN_2          51
#define     MENU_MAN_3          52
#define     MENU_MAN_4          53

#define     MENU_TUN_1          60
#define     MENU_TUN_2          61
#define     MENU_TUN_3          62
#define     MENU_TUN_4          63
#define     MENU_TUN_CONFORM    64
#define     MENU_TUN_CONFORM_YES   65
#define     MENU_TUN_CONFORM_NO    66

#define     BASE_GREEN_TIME     3
#define     BASE_YELLOW_TIME    2
#define     BASE_RED_TIME       5
#define     WAIT_TIME           20
#define     ERROR_WAIT_TIME     2

#define     PRE_STATE_RED       70
#define     PRE_STATE_GREEN     71
#define     PRE_STATE_YELLOW    72

#define     SLOW_MODE           80
#define     UART_SLOW           81
#define     UART_AUTO           82

unsigned char statusOfSystem = INIT_SYSTEM;
unsigned char statusOfTraffic = INIT_TRAFFIC;
unsigned char timeOfRed = BASE_RED_TIME;
unsigned char timeOfGreen = BASE_GREEN_TIME;
unsigned char timeOfYellow = BASE_YELLOW_TIME;
unsigned char timeOf7Seg1 = 0;
unsigned char timeOf7Seg2 = 0;
unsigned char globalTimeOfGreen = BASE_GREEN_TIME;
unsigned char globalTimeOfRed = BASE_RED_TIME;
unsigned char globalTimeOfYellow = BASE_YELLOW_TIME;
unsigned char uartGlobalTimeOfGreen = BASE_GREEN_TIME;
unsigned char uartGlobalTimeOfRed = BASE_RED_TIME;
unsigned char uartGlobalTimeOfYellow = BASE_YELLOW_TIME;

unsigned char timeCounter = 0;
unsigned char trafficCounter = 0;
unsigned char secCounter = 20;
unsigned char secTrafficCounter = 20;
unsigned char secMenuCounter = 20;
unsigned char waitCounter = 0;
unsigned char tempTrafficCounter = 0;
unsigned char preTunState = PRE_STATE_RED;
unsigned char prePress = 0;
unsigned char uartPrePress = 0;
unsigned char uartPrePress1 = 0;
unsigned char tunPrePress = 0;
unsigned char redFlag = 0;
unsigned char greenFlag = 0;
unsigned char yellowFlag = 0;
unsigned char uartRedFlag = 0;
unsigned char uartGreenFlag = 0;
unsigned char uartYellowFlag = 0;

void AppTrafficLight();
void fsm_automatic();
void fsm_manual();
void fsm_tuning();
void menuRun();
void UartDataReceiveProcess();
////////////////////////////////////////////////////////////////////
//Hien thuc cac chuong trinh con, ham, module, function duoi cho nay
////////////////////////////////////////////////////////////////////

void main(void) {
    init_system();
    delay_ms(1000);
    while (1) {
        if (flag_timer3 == 1) {
            SetTimer3_ms(50);

            AppTrafficLight();
            fsm_manual();
            fsm_tuning();
            menuRun();
            UartDataReceiveProcess();
            scan_key_matrix_with_uart(); // 8 button
            DisplayLcdScreen();
        }
    }
}
// Hien thuc cac module co ban cua chuong trinh

void delay_ms(int value) {
    int i, j;
    for (i = 0; i < value; i++)
        for (j = 0; j < 238; j++);
}

void init_output(void) {
    TRISD_OUT = 0x00;
    PORTD_OUT = 0x00;
    TRISB = 0x00;
    PORTB = 0x00;
}

void init_system(void) {
    statusOfSystem = INIT_SYSTEM;
    statusOfTraffic = INIT_TRAFFIC;
    preTunState = PRE_STATE_RED;
    indexOfDataReceive = 0;
    init_output();
    offAllLeds();
    control_led1(10, 10);
    control_led2(10, 10);
    lcd_init();
    init_key_matrix_with_uart();
    init_interrupt();
    init_uart();
    lcd_clear();
    LcdClearS();
    delay_ms(500);
    init_timer3(46950); //dinh thoi 10ms
    SetTimer3_ms(50); //Chu ky thuc hien viec xu ly input, proccess, output
}

void AppTrafficLight() {
    switch (statusOfTraffic) {
        case WAIT_TRAFFIC:
            break;
        case INIT_TRAFFIC: //RED1 GREEN2
            if (redFlag == 1 && greenFlag == 1 && yellowFlag == 1) {
                redFlag = 0;
                greenFlag = 0;
                yellowFlag = 0;
                timeOfGreen = globalTimeOfGreen;
                timeOfRed = globalTimeOfRed;
                timeOfYellow = globalTimeOfYellow;
            }
            if (uartRedFlag == 1 && uartGreenFlag == 1 && uartYellowFlag == 1) {
                uartRedFlag = 0;
                uartGreenFlag = 0;
                uartYellowFlag = 0;
                timeOfGreen = uartGlobalTimeOfGreen;
                timeOfRed = uartGlobalTimeOfRed;
                timeOfYellow = uartGlobalTimeOfYellow;
            }
            LcdClearS();
            trafficCounter = timeOfGreen;
            timeOf7Seg1 = timeOfRed;
            timeOf7Seg2 = timeOfGreen;
            secTrafficCounter = 20;
            setRed1();
            setGreen2();
            statusOfTraffic = PHASE1;
            break;
        case PHASE1: //RED1 YELLOW2 
            display_led1(timeOf7Seg1);
            display_led2(timeOf7Seg2);
            if (secTrafficCounter <= 0) {
                timeOf7Seg1--;
                timeOf7Seg2--;
                secTrafficCounter = 20;
                trafficCounter--;
            }
            secTrafficCounter--;

            if (trafficCounter <= 0) {
                setYellow2();
                trafficCounter = timeOfYellow;
                timeOf7Seg1 = timeOfRed - timeOfGreen;
                timeOf7Seg2 = timeOfYellow;
                statusOfTraffic = PHASE2;
            }

            break;
        case PHASE2: //GREEN1 RED2
            display_led1(timeOf7Seg1);
            display_led2(timeOf7Seg2);
            if (secTrafficCounter <= 0) {
                timeOf7Seg1--;
                timeOf7Seg2--;
                secTrafficCounter = 20;
                trafficCounter--;
            }
            secTrafficCounter--;

            if (trafficCounter <= 0) {
                trafficCounter = timeOfGreen;
                setGreen1();
                setRed2();
                timeOf7Seg1 = timeOfGreen;
                timeOf7Seg2 = timeOfRed;
                statusOfTraffic = PHASE3;
            }

            break;
        case PHASE3: //YELLOW1 RED2
            display_led1(timeOf7Seg1);
            display_led2(timeOf7Seg2);
            if (secTrafficCounter <= 0) {
                timeOf7Seg1--;
                timeOf7Seg2--;
                secTrafficCounter = 20;
                trafficCounter--;
            }
            secTrafficCounter--;

            if (trafficCounter <= 0) {
                trafficCounter = timeOfYellow;
                setYellow1();
                timeOf7Seg1 = timeOfYellow;
                timeOf7Seg2 = timeOfRed - timeOfGreen;
                statusOfTraffic = PHASE4;
            }
            break;
        case PHASE4: //RED1 GREEN2
            display_led1(timeOf7Seg1);
            display_led2(timeOf7Seg2);
            if (secTrafficCounter <= 0) {
                timeOf7Seg1--;
                timeOf7Seg2--;
                secTrafficCounter = 20;
                trafficCounter--;
            }
            secTrafficCounter--;

            if (trafficCounter <= 0) {
                statusOfTraffic = INIT_TRAFFIC;
            }
            break;
        case SLOW_MODE:
            if (secTrafficCounter <= 0) {
                blinkYellows();
                secTrafficCounter = 20;
            }
            secTrafficCounter--;

            display_led1(100);
            display_led2(100);
            break;
        default:
            break;
    }
}

void fsm_manual() {
    switch (statusOfSystem) {
        case MAN_INIT:
            LcdClearS();
            statusOfTraffic = WAIT_TRAFFIC;
            statusOfSystem = MAN_PHASE1;
            setRed1();
            setGreen2();

            break;
        case MAN_PHASE1:
            statusOfTraffic = WAIT_TRAFFIC;
            LcdPrintStringS(0, 0, "PHASE 1: RED");
            LcdPrintStringS(1, 0, "PHASE 2: GREEN");
            display_led1(100);
            display_led2(100);
            if (secCounter <= 0) {
                secCounter = 20;
                waitCounter--;
            }
            secCounter--;

            if (waitCounter <= 0) {
                statusOfTraffic = INIT_TRAFFIC;
                statusOfSystem = INIT_SYSTEM;
            }

            if (isButtonEnter(1) == 1) {
                secCounter = 20;
                statusOfSystem = MAN_PHASE4;
                setYellow1();
                setRed2();
                LcdClearS();
                waitCounter = WAIT_TIME;
            }
            if (isButtonEnter(2) == 1) {
                secCounter = 20;
                statusOfSystem = MAN_PHASE2;
                setRed1();
                setYellow2();
                LcdClearS();
                waitCounter = WAIT_TIME;
            }

            if (isButtonEnter(0) == 1) {
                prePress = 1;
                LcdClearS();
                statusOfSystem = MENU_MAN_1;
            }
            break;

        case MAN_PHASE2:
            statusOfTraffic = WAIT_TRAFFIC;
            LcdPrintStringS(0, 0, "PHASE 1: RED");
            LcdPrintStringS(1, 0, "PHASE 2: YELLOW");
            display_led1(100);
            display_led2(100);
            if (secCounter <= 0) {
                secCounter = 20;
                waitCounter--;
            }
            secCounter--;

            if (waitCounter <= 0) {
                statusOfTraffic = PHASE1;
                trafficCounter = 0;
                statusOfSystem = INIT_SYSTEM;
            }

            if (isButtonEnter(1) == 1) {
                secCounter = 20;
                statusOfSystem = MAN_PHASE1;
                setRed1();
                setGreen2();
                LcdClearS();
                waitCounter = WAIT_TIME;
            }

            if (isButtonEnter(2) == 1) {
                secCounter = 20;
                statusOfSystem = MAN_PHASE3;
                setGreen1();
                setRed2();
                LcdClearS();
                waitCounter = WAIT_TIME;
            }

            if (isButtonEnter(0) == 1) {
                prePress = 1;
                LcdClearS();
                trafficCounter = 0;
                statusOfSystem = MENU_MAN_2;
            }
            break;

        case MAN_PHASE3:
            statusOfTraffic = WAIT_TRAFFIC;
            LcdPrintStringS(0, 0, "PHASE 1: GREEN");
            LcdPrintStringS(1, 0, "PHASE 2: RED");
            display_led1(100);
            display_led2(100);
            if (secCounter <= 0) {
                secCounter = 20;
                waitCounter--;
            }
            secCounter--;

            if (waitCounter <= 0) {
                statusOfTraffic = PHASE2;
                trafficCounter = 0;
                statusOfSystem = INIT_SYSTEM;
            }

            if (isButtonEnter(1) == 1) {
                secCounter = 20;
                statusOfSystem = MAN_PHASE2;
                setRed1();
                setYellow2();
                LcdClearS();
                waitCounter = WAIT_TIME;
            }

            if (isButtonEnter(2) == 1) {
                secCounter = 20;
                statusOfSystem = MAN_PHASE4;
                setYellow1();
                setRed2();
                LcdClearS();
                waitCounter = WAIT_TIME;
            }

            if (isButtonEnter(0) == 1) {
                prePress = 1;
                LcdClearS();
                statusOfSystem = MENU_MAN_3;
            }
            break;

        case MAN_PHASE4:
            statusOfTraffic = WAIT_TRAFFIC;
            LcdPrintStringS(0, 0, "PHASE 1: YELLOW");
            LcdPrintStringS(1, 0, "PHASE 2: RED");
            display_led1(100);
            display_led2(100);
            if (secCounter <= 0) {
                secCounter = 20;
                waitCounter--;
            }
            secCounter--;

            if (waitCounter <= 0) {
                statusOfTraffic = PHASE3;
                trafficCounter = 0;
                statusOfSystem = INIT_SYSTEM;
            }

            if (isButtonEnter(1) == 1) {
                secCounter = 20;
                statusOfSystem = MAN_PHASE3;
                setGreen1();
                setRed2();
                LcdClearS();
                waitCounter = WAIT_TIME;
            }

            if (isButtonEnter(2) == 1) {
                secCounter = 20;
                statusOfSystem = MAN_PHASE1;
                setRed1();
                setGreen2();
                LcdClearS();
                waitCounter = WAIT_TIME;
            }

            if (isButtonEnter(0) == 1) {
                prePress = 1;
                LcdClearS();
                statusOfSystem = MENU_MAN_4;
            }
            break;
        default:
            break;
    }
}

void fsm_tuning() {
    switch (statusOfSystem) {
        case TUN_INIT:
            LcdClearS();
            waitCounter = WAIT_TIME;
            statusOfSystem = TUN_PHASE1;
            tempTrafficCounter = timeOfRed;
            break;

        case TUN_PHASE1:
            LcdClearS();
            LcdPrintStringS(0, 0, "RED TIME:");
            LcdPrintNumS(0, 10, tempTrafficCounter);
            LcdPrintStringS(1, 0, "Submit? Press A!");
            if (secCounter <= 0) {
                secCounter = 20;
                waitCounter--;
            }
            secCounter--;

            if (waitCounter <= 0) {
                statusOfSystem = INIT_SYSTEM;
            }

            if (isButtonIncrease() == 1) {
                waitCounter = WAIT_TIME;
                tempTrafficCounter++;

                if (tempTrafficCounter > 99) {
                    tempTrafficCounter = 1;
                }
            }
            if (isButtonDecrease() == 1) {
                waitCounter = WAIT_TIME;
                tempTrafficCounter--;

                if (tempTrafficCounter < 1) {
                    tempTrafficCounter = 99;
                }
            }
            if (isButtonEnter(3) == 1) {
                waitCounter = WAIT_TIME;
                preTunState = PRE_STATE_RED;
                LcdClearS();
                statusOfSystem = MENU_TUN_CONFORM_YES;
            }
            if (isButtonEnter(0) == 1) {
                tunPrePress = 0;
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_TUN_1;
            }
            break;
        case TUN_PHASE2:
            LcdClearS();
            LcdPrintStringS(0, 0, "GREEN TIME:");
            LcdPrintNumS(0, 12, tempTrafficCounter);
            LcdPrintStringS(1, 0, "Submit? Press A!");
            if (secCounter <= 0) {
                secCounter = 20;
                waitCounter--;
            }
            secCounter--;

            if (waitCounter <= 0) {
                statusOfSystem = INIT_SYSTEM;
            }


            if (isButtonIncrease() == 1) {
                waitCounter = WAIT_TIME;
                tempTrafficCounter++;

                if (tempTrafficCounter > 99) {
                    tempTrafficCounter = 1;
                }
            }
            if (isButtonDecrease() == 1) {
                waitCounter = WAIT_TIME;
                tempTrafficCounter--;

                if (tempTrafficCounter < 1) {
                    tempTrafficCounter = 99;
                }
            }
            if (isButtonEnter(3) == 1) {
                waitCounter = WAIT_TIME;
                preTunState = PRE_STATE_GREEN;
                LcdClearS();
                statusOfSystem = MENU_TUN_CONFORM_YES;
            }
            if (isButtonEnter(0) == 1) {
                tunPrePress = 0;
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_TUN_2;
            }
            break;

        case TUN_PHASE3:
            LcdClearS();
            LcdPrintStringS(0, 0, "YELLOW TIME:");
            LcdPrintNumS(0, 13, tempTrafficCounter);
            LcdPrintStringS(1, 0, "Submit? Press A!");
            if (secCounter <= 0) {
                secCounter = 20;
                waitCounter--;
            }
            secCounter--;

            if (waitCounter <= 0) {
                statusOfSystem = INIT_SYSTEM;
            }

            if (isButtonIncrease() == 1) {
                waitCounter = WAIT_TIME;
                tempTrafficCounter++;

                if (tempTrafficCounter > 99) {
                    tempTrafficCounter = 1;
                }
            }
            if (isButtonDecrease() == 1) {
                waitCounter = WAIT_TIME;
                tempTrafficCounter--;

                if (tempTrafficCounter < 1) {
                    tempTrafficCounter = 99;
                }
            }
            if (isButtonEnter(3) == 1) {
                waitCounter = WAIT_TIME;
                preTunState = PRE_STATE_YELLOW;
                LcdClearS();
                statusOfSystem = MENU_TUN_CONFORM_YES;
            }
            if (isButtonEnter(0) == 1) {
                tunPrePress = 0;
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_TUN_3;
            }
            break;
        default:
            break;
    }
}

void menuRun() {
    switch (statusOfSystem) {
        case INIT_SYSTEM:
            if (secCounter <= 0) {
                secCounter = 20;
                waitCounter--;
            }

            if (waitCounter <= 0) {
                statusOfSystem = INIT_SYSTEM;
            }

            LcdPrintStringS(0, 0, "SYSTEM'S WORKING");
            LcdPrintStringS(1, 0, "Press 1 for menu");

            if (isButtonEnter(0) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = INIT_MENU;
            }
            break;
        case INIT_MENU:
            if (secCounter <= 0) {
                secCounter = 20;
                waitCounter--;
            }

            if (waitCounter <= 0) {
                statusOfSystem = INIT_SYSTEM;
            }
            secCounter--;
            LcdPrintStringS(0, 0, "MENU: 1.AUTO");
            LcdPrintStringS(1, 6, "2.MANUAL");
            LcdPrintStringS(0, 15, "<");

            if (isButtonEnter(1) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_3;
            }

            if (isButtonEnter(2) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_1;
            }

            if (isButtonEnter(3) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = INIT_SYSTEM;
                statusOfTraffic = INIT_TRAFFIC;
            }
            break;
        case MENU_1:
            if (secCounter <= 0) {
                secCounter = 20;
                waitCounter--;
            }

            if (waitCounter <= 0) {
                statusOfSystem = INIT_SYSTEM;
            }
            secCounter--;
            LcdPrintStringS(0, 0, "MENU: 1.AUTO");
            LcdPrintStringS(1, 6, "2.MANUAL");
            LcdPrintStringS(1, 15, "<");

            if (isButtonEnter(1) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = INIT_MENU;
            }

            if (isButtonEnter(2) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_2;
            }

            if (isButtonEnter(3) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_MAN_1;
            }


            if (isButtonEnter(0) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = INIT_SYSTEM;
            }
            break;

        case MENU_MAN_1:
            if (secCounter <= 0) {
                secCounter = 20;
                waitCounter--;
            }
            if (waitCounter <= 0) {
                statusOfTraffic = INIT_TRAFFIC;
                statusOfSystem = INIT_SYSTEM;
            }
            secCounter--;
            LcdPrintStringS(0, 0, "MANUAL: 1. R-G");
            LcdPrintStringS(1, 8, "2. R-Y");
            LcdPrintStringS(0, 15, "<");

            if (isButtonEnter(1) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_MAN_4;
            }

            if (isButtonEnter(2) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_MAN_2;
            }

            if (isButtonEnter(3) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MAN_INIT;
            }
            if (isButtonEnter(0) == 1 && prePress == 0) {
                statusOfTraffic = INIT_TRAFFIC;
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_1;
            } else {
                prePress = 0;
            }
            break;
        case MENU_MAN_2:
            if (secCounter <= 0) {
                secCounter = 20;
                waitCounter--;
            }
            if (waitCounter <= 0) {
                statusOfTraffic = PHASE1;
                trafficCounter = 0;
                statusOfSystem = INIT_SYSTEM;
            }
            secCounter--;
            LcdPrintStringS(0, 0, "MANUAL: 1. R-G");
            LcdPrintStringS(1, 8, "2. R-Y");
            LcdPrintStringS(1, 15, "<");

            if (isButtonEnter(1) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_MAN_1;
            }

            if (isButtonEnter(2) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_MAN_3;
            }

            if (isButtonEnter(3) == 1) {
                waitCounter = WAIT_TIME;
                setRed1();
                setYellow2();
                LcdClearS();
                statusOfSystem = MAN_PHASE2;
            }
            if (isButtonEnter(0) == 1 && prePress == 0) {
                statusOfTraffic = PHASE1;
                trafficCounter = 0;
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_1;
            } else {
                prePress = 0;
            }
            break;

        case MENU_MAN_3:
            if (secCounter <= 0) {
                secCounter = 20;
                waitCounter--;
            }
            if (waitCounter <= 0) {
                statusOfTraffic = PHASE2;
                trafficCounter = 0;
                statusOfSystem = INIT_SYSTEM;
            }
            secCounter--;
            LcdPrintStringS(0, 0, "MANUAL: 3. G-R");
            LcdPrintStringS(1, 8, "4. Y-R");
            LcdPrintStringS(0, 15, "<");

            if (isButtonEnter(1) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_MAN_2;
            }

            if (isButtonEnter(2) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_MAN_4;
            }

            if (isButtonEnter(3) == 1) {
                waitCounter = WAIT_TIME;
                setGreen1();
                setRed2();
                LcdClearS();
                statusOfSystem = MAN_PHASE3;
            }
            if (isButtonEnter(0) == 1 && prePress == 0) {
                statusOfTraffic = PHASE2;
                trafficCounter = 0;
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_1;
            } else {
                prePress = 0;
            }
            break;

        case MENU_MAN_4:
            if (secCounter <= 0) {
                secCounter = 20;
                waitCounter--;
            }
            if (waitCounter <= 0) {
                statusOfTraffic = PHASE3;
                trafficCounter = 0;
                statusOfSystem = INIT_SYSTEM;
            }
            secCounter--;
            LcdPrintStringS(0, 0, "MANUAL: 3. G-R");
            LcdPrintStringS(1, 8, "4. Y-R");
            LcdPrintStringS(1, 15, "<");

            if (isButtonEnter(1) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_MAN_3;
            }

            if (isButtonEnter(2) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_MAN_1;
            }

            if (isButtonEnter(3) == 1) {
                waitCounter = WAIT_TIME;
                setYellow1();
                setRed2();
                LcdClearS();
                statusOfSystem = MAN_PHASE4;
            }
            if (isButtonEnter(0) == 1 && prePress == 0) {
                statusOfTraffic = PHASE3;
                trafficCounter = 0;
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_1;
            } else {
                prePress = 0;
            }
            break;

        case MENU_2:
            if (secCounter <= 0) {
                secCounter = 20;
                waitCounter--;
            }
            if (waitCounter <= 0) {
                statusOfSystem = INIT_SYSTEM;
            }
            secCounter--;
            LcdPrintStringS(0, 0, "MENU: 3.TUNNING");
            LcdPrintStringS(1, 6, "4.SLOW");
            LcdPrintStringS(0, 15, "<");

            if (isButtonEnter(1) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_1;
            }

            if (isButtonEnter(2) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_3;
            }

            if (isButtonEnter(3) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_TUN_1;
            }
            if (isButtonEnter(0) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = INIT_SYSTEM;
            }
            break;

        case MENU_TUN_1:
            if (secCounter <= 0) {
                secCounter = 20;
                waitCounter--;
            }
            if (waitCounter <= 0) {
                statusOfSystem = INIT_SYSTEM;
            }
            secCounter--;
            LcdPrintStringS(0, 0, "TUN: 1.RED");
            LcdPrintStringS(1, 5, "2.GREEN");
            LcdPrintStringS(0, 15, "<");

            if (isButtonEnter(1) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_TUN_3;
            }

            if (isButtonEnter(2) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_TUN_2;
            }

            if (isButtonEnter(3) == 1) {
                waitCounter = WAIT_TIME;
                preTunState = PRE_STATE_RED;
                LcdClearS();
                statusOfSystem = TUN_INIT;
            }
            if (isButtonEnter(0) == 1 && tunPrePress == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_2;
            } else {
                tunPrePress = 1;
            }
            break;
        case MENU_TUN_2:
            if (secCounter <= 0) {
                secCounter = 20;
                waitCounter--;
            }
            if (waitCounter <= 0) {
                statusOfSystem = INIT_SYSTEM;
            }
            secCounter--;
            LcdPrintStringS(0, 0, "TUN: 1.RED");
            LcdPrintStringS(1, 5, "2.GREEN");
            LcdPrintStringS(1, 15, "<");

            if (isButtonEnter(1) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_TUN_1;
            }

            if (isButtonEnter(2) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_TUN_3;
            }

            if (isButtonEnter(3) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                tempTrafficCounter = timeOfGreen;
                statusOfSystem = TUN_PHASE2;
            }
            if (isButtonEnter(0) == 1 && tunPrePress == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_2;
            } else {
                tunPrePress = 1;
            }
            break;
        case MENU_TUN_3:
            if (secCounter <= 0) {
                secCounter = 20;
                waitCounter--;
            }
            if (waitCounter <= 0) {
                statusOfSystem = INIT_SYSTEM;
            }
            secCounter--;
            LcdPrintStringS(0, 0, "TUN: 3.YELLOW");
            LcdPrintStringS(0, 15, "<");

            if (isButtonEnter(1) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_TUN_2;
            }

            if (isButtonEnter(2) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_TUN_1;
            }

            if (isButtonEnter(3) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                tempTrafficCounter = timeOfYellow;
                statusOfSystem = TUN_PHASE3;
            }
            if (isButtonEnter(0) == 1 && tunPrePress == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_2;
            } else {
                tunPrePress = 1;
            }
            break;
        case MENU_TUN_CONFORM_YES:
            if (secCounter <= 0) {
                secCounter = 20;
                waitCounter--;
            }
            if (waitCounter <= 0) {
                statusOfSystem = INIT_SYSTEM;
            }
            secCounter--;
            LcdPrintStringS(0, 0, "Conform?   YES");
            LcdPrintStringS(1, 11, "NO");
            LcdPrintStringS(0, 15, "<");

            if (isButtonEnter(1) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_TUN_CONFORM_NO;
            }

            if (isButtonEnter(2) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_TUN_CONFORM_NO;
            }

            if ((isConformHold() == 1) && (preTunState == PRE_STATE_RED)) {
                redFlag = 1;
                waitCounter = WAIT_TIME;
                LcdClearS();
                globalTimeOfRed = tempTrafficCounter;
                statusOfSystem = MENU_TUN_1;
            }

            if ((isConformHold() == 1) && (preTunState == PRE_STATE_GREEN)) {
                greenFlag = 1;
                waitCounter = WAIT_TIME;
                LcdClearS();
                globalTimeOfGreen = tempTrafficCounter;
                statusOfSystem = MENU_TUN_2;
            }

            if ((isConformHold() == 1) && (preTunState == PRE_STATE_YELLOW)) {
                yellowFlag = 1;
                waitCounter = WAIT_TIME;
                LcdClearS();
                globalTimeOfYellow = tempTrafficCounter;
                statusOfSystem = MENU_TUN_3;
            }

            if (isButtonEnter(0) == 1 && preTunState == PRE_STATE_RED) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_TUN_1;
            }
            if (isButtonEnter(0) == 1 && preTunState == PRE_STATE_GREEN) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_TUN_2;
            }
            if (isButtonEnter(0) == 1 && preTunState == PRE_STATE_YELLOW) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_TUN_3;
            }
            break;
        case MENU_TUN_CONFORM_NO:
            if (secCounter <= 0) {
                secCounter = 20;
                waitCounter--;
            }
            if (waitCounter <= 0) {
                statusOfSystem = INIT_SYSTEM;
            }
            secCounter--;
            LcdPrintStringS(0, 0, "Conform?   YES");
            LcdPrintStringS(1, 11, "NO");
            LcdPrintStringS(1, 15, "<");

            if (isButtonEnter(1) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_TUN_CONFORM_YES;
            }

            if (isButtonEnter(2) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_TUN_CONFORM_YES;
            }

            if (isConformHold() == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_TUN_1;
            }

            if (isButtonEnter(0) == 1 && preTunState == PRE_STATE_RED) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_TUN_1;
            }
            if (isButtonEnter(0) == 1 && preTunState == PRE_STATE_GREEN) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_TUN_2;
            }
            if (isButtonEnter(0) == 1 && preTunState == PRE_STATE_YELLOW) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_TUN_3;
            }
            break;
        case MENU_3:
            if (secCounter <= 0) {
                secCounter = 20;
                waitCounter--;
            }
            if (waitCounter <= 0) {
                statusOfSystem = INIT_SYSTEM;
            }
            secCounter--;
            LcdPrintStringS(0, 0, "MENU: 3.TUNNING");
            LcdPrintStringS(1, 6, "4.SLOW");
            LcdPrintStringS(1, 15, "<");

            if (isButtonEnter(1) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_2;
            }

            if (isButtonEnter(2) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = INIT_MENU;
            }

            if (isButtonEnter(3) == 1) {
                offAllLeds();
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = SLOW_MODE;
                statusOfTraffic = SLOW_MODE;
            }
            if (isButtonEnter(0) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = INIT_SYSTEM;
            }
            break;
        case SLOW_MODE:
            LcdPrintStringS(0, 0, "ENTER SLOW MODE");
            LcdPrintStringS(1, 0, "Press 1 for menu");
            if (isButtonEnter(0) == 1) {
                waitCounter = WAIT_TIME;
                LcdClearS();
                statusOfSystem = MENU_3;
            }
            break;

        case UART_RED_COMPLETE:
            uartRedFlag = 1;

            if (secCounter <= 0) {
                secCounter = 20;
                waitCounter--;
            }
            secCounter--;

            if (waitCounter <= 0) {
                statusOfUart = UART_WAIT;
                statusOfSystem = INIT_SYSTEM;
            }
            LcdPrintStringS(0, 0, "Data received!");
            LcdPrintStringS(1, 0, "Red time:");
            LcdPrintNumS(1, 10, uartGlobalTimeOfRed);

            if (isButtonEnter(0) == 1 && uartPrePress1 == 1) {
                statusOfUart = UART_WAIT;
                LcdClearS();
                statusOfSystem = INIT_SYSTEM;
            } else {
                uartPrePress1 = 1;
            }
            break;
        case UART_GREEN_COMPLETE:
            uartGreenFlag = 1;

            if (secCounter <= 0) {
                secCounter = 20;
                waitCounter--;
            }
            secCounter--;
            if (waitCounter <= 0) {
                statusOfUart = UART_WAIT;
                statusOfSystem = INIT_SYSTEM;
            }

            LcdPrintStringS(0, 0, "Data received!");
            LcdPrintStringS(1, 0, "Green time: ");
            LcdPrintNumS(1, 12, uartGlobalTimeOfGreen);

            if (isButtonEnter(0) == 1 && uartPrePress1 == 1) {
                statusOfUart = UART_WAIT;
                LcdClearS();
                statusOfSystem = INIT_SYSTEM;
            } else {
                uartPrePress1 = 1;
            }
            break;
        case UART_YELLOW_COMPLETE:
            uartYellowFlag = 1;

            if (secCounter <= 0) {
                secCounter = 20;
                waitCounter--;
            }
            secCounter--;
            if (waitCounter <= 0) {
                statusOfUart = UART_WAIT;
                statusOfSystem = INIT_SYSTEM;
            }

            LcdPrintStringS(0, 0, "Data received!");
            LcdPrintStringS(1, 0, "Yellow time: ");
            LcdPrintNumS(1, 13, uartGlobalTimeOfYellow);

            if (isButtonEnter(0) == 1 && uartPrePress1 == 1) {
                statusOfUart = UART_WAIT;
                LcdClearS();
                statusOfSystem = INIT_SYSTEM;
            } else {
                uartPrePress1 = 1;
            }
            break;
        default:
            break;
    }
}

void UartDataReceiveProcess() {
    switch (statusOfUart) {
        case UART_WAIT:
            break;
        case UART_CONFORM_RED:
            if (flagOfDataReceiveComplete == 1) {
                flagOfDataReceiveComplete = 0;
                if (dataReceive[0] == 'R' && ('0' <= dataReceive[1] && dataReceive[1] <= '9')
                        && ('0' <= dataReceive[2] && dataReceive[2] <= '9')) {
                    uartGlobalTimeOfRed = (dataReceive[1] - 48)*10 + (dataReceive[2] - 48);
                    LcdClearS();
                    waitCounter = 2;
                    statusOfSystem = UART_RED_COMPLETE;
                    statusOfUart = UART_WAIT;
                    UartSendString("Data received successfully! \r\n");
                    UartSendString("  New RED time is: ");
                    UartSendNum(uartGlobalTimeOfRed);
                    UartSendString(" seconds. \r\n");
                    UartSendString("\r\n");
                } else {
                    flagOfDataReceiveComplete = 1;
                    statusOfUart = UART_ERROR;
                }
            }
            break;
        case UART_CONFORM_GREEN:
            if (flagOfDataReceiveComplete == 1) {
                flagOfDataReceiveComplete = 0;
                if (dataReceive[0] == 'G' && ('0' <= dataReceive[1] && dataReceive[1] <= '9')
                        && ('0' <= dataReceive[2] && dataReceive[2] <= '9')) {
                    uartGlobalTimeOfGreen = (dataReceive[1] - 48)*10 + (dataReceive[2] - 48);
                    waitCounter = 2;
                    LcdClearS();
                    statusOfSystem = UART_GREEN_COMPLETE;
                    statusOfUart = UART_WAIT;
                    UartSendString("Data received successfully! \r\n");
                    UartSendString("  New GREEN time is: ");
                    UartSendNum(uartGlobalTimeOfGreen);
                    UartSendString(" seconds. \r\n");
                    UartSendString("\r\n");
                } else {
                    flagOfDataReceiveComplete = 1;
                    statusOfUart = UART_ERROR;
                }
            }
            break;
        case UART_CONFORM_YELLOW:
            if (flagOfDataReceiveComplete == 1) {
                flagOfDataReceiveComplete = 0;
                if (dataReceive[0] == 'Y' && ('0' <= dataReceive[1] && dataReceive[1] <= '9')
                        && ('0' <= dataReceive[2] && dataReceive[2] <= '9')) {
                    uartGlobalTimeOfYellow = (dataReceive[1] - 48)*10 + (dataReceive[2] - 48);
                    waitCounter = 2;
                    LcdClearS();
                    statusOfSystem = UART_YELLOW_COMPLETE;
                    statusOfUart = UART_WAIT;
                    UartSendString("Data received successfully! \r\n");
                    UartSendString("  New YELLOW time is: ");
                    UartSendNum(uartGlobalTimeOfYellow);
                    UartSendString(" seconds. \r\n");
                    UartSendString("\r\n");
                } else {
                    flagOfDataReceiveComplete = 1;
                    statusOfUart = UART_ERROR;
                }
            }
            break;
        case UART_SLOW:
            if (flagOfDataReceiveComplete == 1) {
                flagOfDataReceiveComplete = 0;
                if (dataReceive[0] == 'S' && dataReceive[1] == 'L' && dataReceive[2] == 'O') {
                    offAllLeds();
                    waitCounter = WAIT_TIME;
                    LcdClearS();
                    statusOfSystem = SLOW_MODE;
                    statusOfUart = UART_WAIT;
                    statusOfTraffic = SLOW_MODE;
                    UartSendString("Data received successfully! \r\n");
                    UartSendString("  Slow mode activated!");
                    UartSendString("\r\n");
                    UartSendString("\r\n");
                } else {
                    flagOfDataReceiveComplete = 1;
                    statusOfUart = UART_ERROR;
                }
            }
            break;
        case UART_AUTO:
            if (flagOfDataReceiveComplete == 1) {
                flagOfDataReceiveComplete = 0;
                if (dataReceive[0] == 'A' && dataReceive[1] == 'T' && dataReceive[2] == 'O') {
                    waitCounter = WAIT_TIME;
                    LcdClearS();
                    statusOfSystem = INIT_SYSTEM;
                    statusOfUart = UART_WAIT;
                    statusOfTraffic = INIT_TRAFFIC;
                    UartSendString("Data received successfully! \r\n");
                    UartSendString("  Automatic mode activated!");
                    UartSendString("\r\n");
                    UartSendString("\r\n");
                } else {
                    flagOfDataReceiveComplete = 1;
                    statusOfUart = UART_ERROR;
                }
            }
            break;
        case INIT_UART_ERROR:
            statusOfUart = UART_ERROR;
            break;
        case UART_ERROR:
            if (flagOfDataReceiveComplete == 1) {
                flagOfDataReceiveComplete = 0;
                UartSendString("Syntax error! Please try again. \r\n");
                UartSendString("\r\n");
            }
            break;
        default:
            break;
    }
}
