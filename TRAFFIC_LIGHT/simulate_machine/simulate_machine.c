#include "simulate_machine.h"
#include "..\button_matrix\button.h"
#include "..\lcd\lcd.h"

#define     INIT_RECEIVE            0
#define     WAIT_HEADER_1           1
#define     WAIT_HEADER_2           2
#define     RECEIVE_DATA            3
#define     END_OF_RECEIVE_1        4
#define     END_OF_RECEIVE_2        5
#define     INIT_SYSTEM             0
#define     CALL                    1  
#define     MESSAGE                 2
#define     WAITING                 3
#define     MESSAGE_GPRS            4
#define     WAITING_GPRS            5
// Noi khai bao bien toan cuc
unsigned char statusSim900 = INIT_SYSTEM;
unsigned char WaitResponse = 0;

unsigned char i;
unsigned char statusReceive = INIT_RECEIVE;
unsigned char flagOfDataReceiveComplete = 0;
unsigned char indexOfMachine = 0;
unsigned char indexOfDataReceive = 0;
unsigned char indexOfdataSend = 0;
unsigned char numberOfDataReceive = 3;
unsigned char dataReceive [50];
unsigned char numberOfDataSend = 3;
unsigned char dataSend [50];
unsigned char statusOfUart = 34;

unsigned int timeChangedataSend = 0;

void uart_isr_simulate_machine() {
    unsigned char tempReceive;
    tempReceive = RCREG;
    switch (statusReceive) {
        case INIT_RECEIVE:
            //            statusOfUart = 0;
        case WAIT_HEADER_1:
            if (tempReceive == 0x21) {
                statusOfUart = 0;
                indexOfDataReceive = 0;
                statusReceive = WAIT_HEADER_2;
            }
            break;
        case WAIT_HEADER_2:
            if (tempReceive == 0x21)
                statusReceive = RECEIVE_DATA;
            else
                statusReceive = WAIT_HEADER_1;
            break;
        case RECEIVE_DATA:
            dataReceive[indexOfDataReceive] = tempReceive;
            indexOfDataReceive++;
            if (indexOfDataReceive >= numberOfDataReceive)
                statusReceive = END_OF_RECEIVE_1;
            break;
        case END_OF_RECEIVE_1:
            if (tempReceive == 0x23)
                statusReceive = END_OF_RECEIVE_2;
            else
                statusReceive = WAIT_HEADER_1;
            break;
        case END_OF_RECEIVE_2:
            if (tempReceive == 0x23) {
                flagOfDataReceiveComplete = 1;
                if (dataReceive[0] == 0x52) {
                    statusOfUart = 52;
                } else if (dataReceive[0] == 0x47) {
                    statusOfUart = 47;
                } else if (dataReceive[0] == 0x59) {
                    statusOfUart = 59;
                } else if (dataReceive[0] == 0x53) {
                    statusOfUart = 81;
                } else if (dataReceive[0] == 0x41) {
                    statusOfUart = 82;
                } else if (dataReceive[0] != 0x52 && dataReceive[0] != 0x47 && dataReceive[0] != 0x59) {
                    statusOfUart = 201;
                }
                statusReceive = INIT_RECEIVE;
            } else
                statusReceive = WAIT_HEADER_1;
            break;
        default:
            statusReceive = INIT_RECEIVE;
            break;
    }
}

void SimulatedataSend() {

}