#include "main.h"
// Noi khai bao hang so
#define     LED                 PORTD
#define     ON                  1
#define     OFF                 0
#define     ENABLE              1
#define     DISABLE             0

#define     ADDRESS_FIRST_PROGRAM   0x20
#define     ADDRESS_HOUR_ALARM      0x10
#define     ADDRESS_MINUTE_ALARM    0x11
#define     ADDRESS_BIT_ALARM       0x12
#define     ADDRESS_FLAG_ALARM      0x13

#define     INIT_SYSTEM         0
#define     CHOOSE_MODE         1
#define     INIT_WORLD_TIME     2
#define     CHOOSE_WORLD_TIME   3
#define     SET_HOUR            100
#define     SET_MINUTE          101
#define     SET_DAY             102
#define     SET_DATE            103
#define     SET_MONTH           104
#define     SET_YEAR            105
#define     SET_HOUR_ALARM      106
#define     SET_MINUTE_ALARM    107
#define     SET_BIT_ALARM       108
#define     NEW_YORK_TIME       109
#define     PARIS_TIME          110
#define     LONDON_TIME         111

// Noi khai bao bien toan cuc
unsigned char hour = 0;
unsigned char minute = 0;
unsigned char second = 0;
unsigned char day = 0;
unsigned char date = 0;
unsigned char month = 0;
unsigned char year = 0;

unsigned char worldHour = 0;
unsigned char worldMinute = 0;
unsigned char worldSecond = 0;
unsigned char worldDate = 0;
unsigned char worldMonth = 0;
unsigned char worldYear = 0;

unsigned char statusSetUpTime = INIT_SYSTEM;
unsigned char hourAlarm = 0;
unsigned char minuteAlarm = 0;
unsigned char bitAlarm = 1;
unsigned char timeBlink = 0;
unsigned char mode = 1;
unsigned char worldTimeMode = 1;
unsigned char worldTimeFlag = 0;

// Khai bao cac ham co ban IO
void init_system(void);
void delay_ms(int value);

void SetupTimeForRealTime();
void SetupForFirstProgram(void);
void ReadDataFromDS1307(void);
void SetUpTime();
void SetUpAlarm();
void DisplayTime();
void DisplayAlarmTime();
void DisplayWorldTime();
void Alarm();
void WorldTime();
////////////////////////////////////////////////////////////////////
//Hien thuc cac chuong trinh con, ham, module, function duoi cho nay
////////////////////////////////////////////////////////////////////

void main(void) {
    unsigned int k = 0;

    init_system();
    lcd_clear();
    LcdClearS();
    SetupForFirstProgram();
    delay_ms(1000);
    LcdPrintString(0, 3, "SMART CLOCK");
    ReadDataFromDS1307();
    delay_ms(2000);
    SetUpTime();
    while (1) {
        while (!flag_timer3);
        flag_timer3 = 0;
        scan_key_matrix();
        k = (k + 1) % 2000;
        DisplayTime();
        SetUpTime();
        SetUpAlarm();
        Alarm();
        WorldTime();
        DisplayLcdScreenOld();
    }
}
// Hien thuc cac module co ban cua chuong trinh

void delay_ms(int value) {
    int i, j;
    for (i = 0; i < value; i++)
        for (j = 0; j < 238; j++);
}

void init_system(void) {
    TRISB = 0x00; //setup PORTB is output
    TRISD = 0x00;
    init_lcd();
    //LED = 0x00;
    init_interrupt();
    delay_ms(1000);
    init_timer0(4695); //dinh thoi 1ms sai so 1%
    init_timer1(9390); //dinh thoi 2ms
    init_timer3(46950); //dinh thoi 10ms
    SetTimer0_ms(2);
    SetTimer1_ms(10);
    SetTimer3_ms(50); //Chu ky thuc hien viec xu ly input,proccess,output
    init_key_matrix();
    init_i2c();
}

///////I2C

void SetupTimeForRealTime() {
    hour = 0;
    minute = 0;
    second = 0;
    day = 1;
    date = 1;
    month = 1;
    year = 23;
    Write_DS1307(ADDRESS_SECOND, second);
    Write_DS1307(ADDRESS_MINUTE, minute);
    Write_DS1307(ADDRESS_HOUR, hour);
    Write_DS1307(ADDRESS_DAY, day);
    Write_DS1307(ADDRESS_DATE, date);
    Write_DS1307(ADDRESS_MONTH, month);
    Write_DS1307(ADDRESS_YEAR, year);
}

void SetupForFirstProgram(void) {
    if (Read_DS1307(ADDRESS_FIRST_PROGRAM) != 0x22) {
        SetupTimeForRealTime();

        Write_DS1307(ADDRESS_HOUR_ALARM, 0);
        Write_DS1307(ADDRESS_MINUTE_ALARM, 0);
        Write_DS1307(ADDRESS_BIT_ALARM, 0);
        Write_DS1307(ADDRESS_FLAG_ALARM, 0);

        Write_DS1307(ADDRESS_FIRST_PROGRAM, 0x22);
    }
}

void ReadDataFromDS1307(void) {
    hourAlarm = Read_DS1307(ADDRESS_HOUR_ALARM);
    minuteAlarm = Read_DS1307(ADDRESS_MINUTE_ALARM);
    bitAlarm = Read_DS1307(ADDRESS_BIT_ALARM);
}

void DisplayTime() {
    hour = Read_DS1307(ADDRESS_HOUR);
    minute = Read_DS1307(ADDRESS_MINUTE);
    second = Read_DS1307(ADDRESS_SECOND);
    day = Read_DS1307(ADDRESS_DAY);
    date = Read_DS1307(ADDRESS_DATE);
    month = Read_DS1307(ADDRESS_MONTH);
    year = Read_DS1307(ADDRESS_YEAR);

    LcdClearS();

    //Display day
    switch (day) {
        case 1:
            LcdPrintStringS(0, 0, "SUN");
            break;
        case 2:
            LcdPrintStringS(0, 0, "MON");
            break;
        case 3:
            LcdPrintStringS(0, 0, "TUE");
            break;
        case 4:
            LcdPrintStringS(0, 0, "WED");
            break;
        case 5:
            LcdPrintStringS(0, 0, "THU");
            break;
        case 6:
            LcdPrintStringS(0, 0, "FRI");
            break;
        case 7:
            LcdPrintStringS(0, 0, "SAT");
            break;
    }

    //Display month
    switch (month) {
        case 1:
            LcdPrintStringS(1, 0, "JAN");
            break;
        case 2:
            LcdPrintStringS(1, 0, "FEB");
            break;
        case 3:
            LcdPrintStringS(1, 0, "MAR");
            break;
        case 4:
            LcdPrintStringS(1, 0, "APR");
            break;
        case 5:
            LcdPrintStringS(1, 0, "MAY");
            break;
        case 6:
            LcdPrintStringS(1, 0, "JUN");
            break;
        case 7:
            LcdPrintStringS(1, 0, "JUL");
            break;
        case 8:
            LcdPrintStringS(1, 0, "AUG");
            break;
        case 9:
            LcdPrintStringS(1, 0, "SEP");
            break;
        case 10:
            LcdPrintStringS(1, 0, "OCT");
            break;
        case 11:
            LcdPrintStringS(1, 0, "NOV");
            break;
        case 12:
            LcdPrintStringS(1, 0, "DEC");
            break;
    }

    if (hour < 10) {
        LcdPrintStringS(0, 4, "0");
        LcdPrintNumS(0, 5, hour);
    } else LcdPrintNumS(0, 4, hour);

    LcdPrintStringS(0, 6, ":");

    if (minute < 10) {
        LcdPrintStringS(0, 7, "0");
        LcdPrintNumS(0, 8, minute);
    } else LcdPrintNumS(0, 7, minute);

    LcdPrintStringS(0, 9, ":");

    if (second < 10) {
        LcdPrintStringS(0, 10, "0");
        LcdPrintNumS(0, 11, second);
    } else LcdPrintNumS(0, 10, second);

    //Display date
    if (date < 10) {
        LcdPrintStringS(1, 4, "0");
        LcdPrintNumS(1, 5, date);
    } else LcdPrintNumS(1, 4, date);

    //Display year
    LcdPrintNumS(1, 7, 20);
    LcdPrintNumS(1, 9, year);

    //Display bit alarm
    if (bitAlarm == 0) {
        LcdPrintStringS(1, 13, "OFF");
    } else if (bitAlarm == 1) {
        LcdPrintStringS(1, 14, "ON");
    }
}

void DisplayWorldTime() {
    LcdClearS();

    //Display month
    switch (worldMonth) {
        case 1:
            LcdPrintStringS(1, 3, "JAN");
            break;
        case 2:
            LcdPrintStringS(1, 3, "FEB");
            break;
        case 3:
            LcdPrintStringS(1, 3, "MAR");
            break;
        case 4:
            LcdPrintStringS(1, 3, "APR");
            break;
        case 5:
            LcdPrintStringS(1, 3, "MAY");
            break;
        case 6:
            LcdPrintStringS(1, 3, "JUN");
            break;
        case 7:
            LcdPrintStringS(1, 3, "JUL");
            break;
        case 8:
            LcdPrintStringS(1, 3, "AUG");
            break;
        case 9:
            LcdPrintStringS(1, 3, "SEP");
            break;
        case 10:
            LcdPrintStringS(1, 3, "OCT");
            break;
        case 11:
            LcdPrintStringS(1, 3, "NOV");
            break;
        case 12:
            LcdPrintStringS(1, 3, "DEC");
            break;
    }

    if (worldTimeMode == 1) {
        LcdPrintStringS(0, 0, "NEWYORK");
    } else if (worldTimeMode == 2) {
        LcdPrintStringS(0, 0, "PARIS");
    } else if (worldTimeMode == 3) {
        LcdPrintStringS(0, 0, "LONDON");
    }

    if (worldHour < 10) {
        LcdPrintStringS(0, 8, "0");
        LcdPrintNumS(0, 9, worldHour);
    } else LcdPrintNumS(0, 8, worldHour);

    LcdPrintStringS(0, 10, ":");

    if (worldMinute < 10) {
        LcdPrintStringS(0, 11, "0");
        LcdPrintNumS(0, 12, worldMinute);
    } else LcdPrintNumS(0, 11, worldMinute);

    LcdPrintStringS(0, 13, ":");

    if (second < 10) {
        LcdPrintStringS(0, 14, "0");
        LcdPrintNumS(0, 15, second);
    } else LcdPrintNumS(0, 14, second);

    //Display date
    if (worldDate < 10) {
        LcdPrintStringS(1, 7, "0");
        LcdPrintNumS(1, 8, worldDate);
    } else LcdPrintNumS(1, 7, worldDate);

    //Display year
    LcdPrintNumS(1, 10, 20);
    LcdPrintNumS(1, 12, worldYear);
}

void DisplayAlarmTime() {
    LcdClearS();
    LcdPrintStringS(0, 3, "ALARM CLOCK");

    //Display hour alarm
    if (hourAlarm < 10) {
        LcdPrintStringS(1, 3, "0");
        LcdPrintNumS(1, 4, hourAlarm);
    } else {
        LcdPrintNumS(1, 3, hourAlarm);
    }

    LcdPrintStringS(1, 5, ":");

    //Display minute alarm
    if (minuteAlarm < 10) {
        LcdPrintStringS(1, 6, "0");
        LcdPrintNumS(1, 7, minuteAlarm);
    } else {
        LcdPrintNumS(1, 6, minuteAlarm);
    }

    //Display bit alarm
    if (bitAlarm == 0) {
        LcdPrintStringS(1, 11, "OFF");
    } else if (bitAlarm == 1) {
        LcdPrintStringS(1, 11, "ON");
    }
}

void SetUpTime() {
    switch (statusSetUpTime) {
        case INIT_SYSTEM:
            if (isButtonMode() == 1) {
                statusSetUpTime = CHOOSE_MODE;
            }
            break;

        case CHOOSE_MODE:
            if (mode == 1) {
                LcdClearS();
                LcdPrintStringS(0, 0, "1. Set Clock");
                LcdPrintStringS(0, 15, "<");
                LcdPrintStringS(1, 0, "2. Set Alarm");

                if (isButtonDecrease() == 1) {
                    mode = 2;
                }

                if (isButtonIncrease() == 1) {
                    mode = 3;
                }
            } else if (mode == 2) {
                LcdClearS();
                LcdPrintStringS(0, 0, "1. Set Clock");
                LcdPrintStringS(1, 0, "2. Set Alarm");
                LcdPrintStringS(1, 15, "<");

                if (isButtonDecrease() == 1) {
                    mode = 3;
                }

                if (isButtonIncrease() == 1) {
                    mode = 1;
                }
            } else if (mode == 3) {
                LcdClearS();
                LcdPrintStringS(0, 0, "2. Set Alarm");
                LcdPrintStringS(1, 0, "3. World Clock");
                LcdPrintStringS(1, 15, "<");

                if (isButtonDecrease() == 1) {
                    mode = 1;
                }

                if (isButtonIncrease() == 1) {
                    mode = 2;
                }
            }

            if (isButtonEnter() == 1) {
                if (mode == 1) {
                    statusSetUpTime = SET_HOUR;
                    timeBlink = 0;
                } else if (mode == 2) {
                    statusSetUpTime = SET_HOUR_ALARM;
                    timeBlink = 0;
                } else if (mode == 3) {
                    worldTimeMode = 1;
                    statusSetUpTime = INIT_WORLD_TIME;
                    timeBlink = 0;
                }
            }

            if (isButtonMode() == 1) {
                statusSetUpTime = INIT_SYSTEM;
            }
            break;

        case SET_HOUR:
            timeBlink = (timeBlink + 1) % 20;
            DisplayTime();

            if (timeBlink < 5) {
                LcdPrintStringS(0, 4, "  ");
            }

            if (isButtonIncrease() == 1) {
                hour++;
                if (hour > 23) {
                    hour = 0;
                }
                Write_DS1307(ADDRESS_HOUR, hour);
            }

            if (isButtonDecrease() == 1) {
                hour--;
                if (hour > 23) {
                    hour = 23;
                }
                Write_DS1307(ADDRESS_HOUR, hour);
            }

            if (isButtonEnter() == 1) {
                statusSetUpTime = SET_MINUTE;
                timeBlink = 0;
            }

            if (isButtonEnterHold() == 1) {
                statusSetUpTime = INIT_SYSTEM;
                LcdClearS();
            }

            if (isButtonMode() == 1) {
                statusSetUpTime = CHOOSE_MODE;
            }
            break;

        case SET_MINUTE:
            timeBlink = (timeBlink + 1) % 20;

            if (timeBlink < 5) {
                LcdPrintStringS(0, 7, "  ");
            }

            if (isButtonIncrease() == 1) {
                minute++;
                if (minute > 59) {
                    minute = 0;
                }
                Write_DS1307(ADDRESS_MINUTE, minute);
            }

            if (isButtonDecrease() == 1) {
                minute--;
                if (minute > 59) {
                    minute = 59;
                }
                Write_DS1307(ADDRESS_MINUTE, minute);
            }

            if (isButtonEnter() == 1) {
                statusSetUpTime = SET_DAY;
                timeBlink = 0;
            }

            if (isButtonEnterHold() == 1) {
                statusSetUpTime = INIT_SYSTEM;
                LcdClearS();
            }

            if (isButtonMode() == 1) {
                statusSetUpTime = CHOOSE_MODE;
            }
            break;

        case SET_DAY:
            timeBlink = (timeBlink + 1) % 20;

            if (timeBlink < 5) {
                LcdPrintStringS(0, 0, "   ");
            }

            if (isButtonIncrease() == 1) {
                day++;
                if (day > 7) {
                    day = 1;
                }
                Write_DS1307(ADDRESS_DAY, day);
            }

            if (isButtonDecrease() == 1) {
                day--;
                if (day > 7 || day < 1) {
                    day = 7;
                }
                Write_DS1307(ADDRESS_DAY, day);
            }

            if (isButtonEnter() == 1) {
                statusSetUpTime = SET_DATE;
                timeBlink = 0;
            }

            if (isButtonModeHold() == 1) {
                statusSetUpTime = INIT_SYSTEM;
                LcdClearS();
            }

            if (isButtonMode() == 1) {
                statusSetUpTime = CHOOSE_MODE;
            }
            break;

        case SET_DATE:
            timeBlink = (timeBlink + 1) % 20;

            if (timeBlink < 5) {
                LcdPrintStringS(1, 4, "  ");
            }

            if (isButtonIncrease() == 1) {
                date++;
                if (date > 31) {
                    date = 1;
                }
                Write_DS1307(ADDRESS_DATE, date);
            }

            if (isButtonDecrease() == 1) {
                date--;
                if (date > 31 || date < 1) {
                    date = 31;
                }

                Write_DS1307(ADDRESS_DATE, date);
            }

            if (isButtonEnter() == 1) {
                statusSetUpTime = SET_MONTH;
                timeBlink = 0;
            }

            if (isButtonEnterHold() == 1) {
                statusSetUpTime = INIT_SYSTEM;
                LcdClearS();
            }

            if (isButtonMode() == 1) {
                statusSetUpTime = CHOOSE_MODE;
            }
            break;

        case SET_MONTH:
            timeBlink = (timeBlink + 1) % 20;

            if (timeBlink < 5) {
                LcdPrintStringS(1, 0, "   ");
            }

            if (isButtonIncrease() == 1) {
                month++;
                if (month > 12) {
                    month = 1;
                }
                Write_DS1307(ADDRESS_MONTH, month);
            }

            if (isButtonDecrease() == 1) {
                month--;
                if (month > 12 || month < 1) {
                    month = 12;
                }
                Write_DS1307(ADDRESS_MONTH, month);
            }

            if (isButtonEnter() == 1) {
                statusSetUpTime = SET_YEAR;
                timeBlink = 0;
            }

            if (isButtonEnterHold() == 1) {
                statusSetUpTime = INIT_SYSTEM;
                LcdClearS();
            }

            if (isButtonMode() == 1) {
                statusSetUpTime = CHOOSE_MODE;
            }
            break;

        case SET_YEAR:
            timeBlink = (timeBlink + 1) % 20;

            if (timeBlink < 5) {
                LcdPrintStringS(1, 7, "    ");
            }

            if (isButtonIncrease() == 1) {
                year++;
                Write_DS1307(ADDRESS_YEAR, year);
            }

            if (isButtonDecrease() == 1) {
                year--;
                Write_DS1307(ADDRESS_YEAR, year);
            }

            if (isButtonEnter() == 1) {
                statusSetUpTime = SET_HOUR;
                timeBlink = 0;
            }

            if (isButtonEnterHold() == 1) {
                statusSetUpTime = INIT_SYSTEM;
                LcdClearS();
            }

            if (isButtonMode() == 1) {
                statusSetUpTime = CHOOSE_MODE;
            }
            break;
    }
}

void SetUpAlarm() {
    switch (statusSetUpTime) {
        case SET_HOUR_ALARM:
            DisplayAlarmTime();
            timeBlink = (timeBlink + 1) % 20;

            if (timeBlink < 5) {
                LcdPrintStringS(1, 3, "  ");
            }

            if (isButtonIncrease() == 1) {
                hourAlarm = (hourAlarm + 1) % 24;
                Write_DS1307(ADDRESS_HOUR_ALARM, hourAlarm);
            }

            if (isButtonDecrease() == 1) {
                hourAlarm--;
                if (hourAlarm > 23) {
                    hourAlarm = 23;
                }
                Write_DS1307(ADDRESS_HOUR_ALARM, hourAlarm);
            }

            if (isButtonEnter() == 1) {
                statusSetUpTime = SET_MINUTE_ALARM;
                timeBlink = 0;
            }
            else if (isButtonEnterHold() == 1) {
                statusSetUpTime = INIT_SYSTEM;
                LcdClearS();
            }
            else if (isButtonMode() == 1) {
                statusSetUpTime = CHOOSE_MODE;
            }
            break;

        case SET_MINUTE_ALARM:
            DisplayAlarmTime();
            timeBlink = (timeBlink + 1) % 20;

            if (timeBlink < 5) {
                LcdPrintStringS(1, 6, "  ");
            }

            if (isButtonIncrease() == 1) {
                minuteAlarm = (minuteAlarm + 1) % 60;
                Write_DS1307(ADDRESS_MINUTE_ALARM, minuteAlarm);
            }

            if (isButtonDecrease() == 1) {
                minuteAlarm--;
                if (minuteAlarm > 59) {
                    minuteAlarm = 59;
                }
                Write_DS1307(ADDRESS_MINUTE_ALARM, minuteAlarm);
            }

            if (isButtonEnter() == 1) {
                statusSetUpTime = SET_BIT_ALARM;
                timeBlink = 0;
            }

            else if (isButtonEnterHold() == 1) {
                statusSetUpTime = INIT_SYSTEM;
                LcdClearS();
            }

            else if (isButtonMode() == 1) {
                statusSetUpTime = CHOOSE_MODE;
            }
            break;

        case SET_BIT_ALARM:
            DisplayAlarmTime();
            timeBlink = (timeBlink + 1) % 20;

            if (timeBlink < 5) {
                LcdPrintStringS(1, 11, "   ");
            }

            if (isButtonIncrease() == 1) {
                bitAlarm++;
                if (bitAlarm > 1) {
                    bitAlarm = 0;
                }
                Write_DS1307(ADDRESS_BIT_ALARM, bitAlarm);
            }

            if (isButtonDecrease() == 1) {
                bitAlarm--;
                if (bitAlarm > 1) {
                    bitAlarm = 1;
                }
                Write_DS1307(ADDRESS_BIT_ALARM, bitAlarm);
            }

            if (isButtonEnter() == 1) {
                statusSetUpTime = SET_HOUR_ALARM;
                timeBlink = 0;
            }

            else if (isButtonEnterHold() == 1) {
                statusSetUpTime = INIT_SYSTEM;
                LcdClearS();
            }

            else if (isButtonMode() == 1) {
                statusSetUpTime = CHOOSE_MODE;
            }
            break;
    }
}

void Alarm() {
    if (hourAlarm == hour && minuteAlarm == minute && bitAlarm == 1 && statusSetUpTime == INIT_SYSTEM) {
        LcdClearS();
        LcdPrintStringS(0, 0, "RENG RENG");
        LcdPrintStringS(1, 0, "RENG RENG");

        if (isButtonEnter() == 1 || minute == minuteAlarm + 1) {
            bitAlarm = 0;
            LcdClearS();
            statusSetUpTime = INIT_SYSTEM;
        }
    }
}

void WorldTime() {
    switch (statusSetUpTime) {
        case INIT_WORLD_TIME:
            if (worldTimeMode == 1) {
                LcdClearS();
                LcdPrintStringS(0, 0, "NEW YORK TIME");
                LcdPrintStringS(0, 15, "<");
                LcdPrintStringS(1, 0, "PARIS TIME");

                if (isButtonDecrease() == 1) {
                    worldTimeMode = 2;
                }

                if (isButtonIncrease() == 1) {
                    worldTimeMode = 3;
                }
            }
            else if (worldTimeMode == 2) {
                LcdClearS();
                LcdPrintStringS(0, 0, "NEW YORK TIME");
                LcdPrintStringS(1, 0, "PARIS TIME");
                LcdPrintStringS(1, 15, "<");

                if (isButtonDecrease() == 1) {
                    worldTimeMode = 3;
                }

                if (isButtonIncrease() == 1) {
                    worldTimeMode = 1;
                }
            }
            else if (worldTimeMode == 3) {
                LcdClearS();
                LcdPrintStringS(0, 0, "PARIS TIME");
                LcdPrintStringS(1, 0, "LONDON TIME");
                LcdPrintStringS(1, 15, "<");

                if (isButtonDecrease() == 1) {
                    worldTimeMode = 1;
                }

                if (isButtonIncrease() == 1) {
                    worldTimeMode = 2;
                }
            }

            if (isButtonMode() == 1) {
                statusSetUpTime = CHOOSE_MODE;
            }
            
            else if (isButtonEnterHold() == 1) {
                if (worldTimeMode == 1) {
                    statusSetUpTime = NEW_YORK_TIME;
                } else if (worldTimeMode == 2) {
                    statusSetUpTime = PARIS_TIME;
                } else if (worldTimeMode == 3) {
                    statusSetUpTime = LONDON_TIME;
                }
            }
            break;

        case NEW_YORK_TIME:
            LcdClearS();
            worldHour = hour - 12;
            worldMinute = minute;
            worldSecond = second;
            worldDate = date;
            worldMonth = month;
            worldYear = year;
            if (worldHour > 23) {
                worldHour = worldHour - 24;
                worldDate++;
                if (worldDate > 31) {
                    worldMonth++;
                    worldDate = 1;
                }

                if (worldMonth > 12) {
                    worldYear++;
                    worldMonth = 1;
                }
            }

            DisplayWorldTime();

            if (isButtonEnterHold() == 1) {
                statusSetUpTime = INIT_SYSTEM;
            }
            if (isButtonMode() == 1) {
                statusSetUpTime = INIT_WORLD_TIME;
            }
            if (isButtonIncrease() == 1) {
                worldTimeMode = 3;
                statusSetUpTime = LONDON_TIME;
            }
            if (isButtonDecrease() == 1) {
                worldTimeMode = 2;
                statusSetUpTime = PARIS_TIME;
            }
            break;

        case PARIS_TIME:
            LcdClearS();
            worldHour = hour - 6;
            worldMinute = minute;
            worldSecond = second;
            worldDate = date;
            worldMonth = month;
            worldYear = year;
            if (worldHour > 23) {
                worldHour = worldHour - 24;
                worldDate++;
                if (worldDate > 31) {
                    worldMonth++;
                    worldDate = 1;
                }

                if (worldMonth > 12) {
                    worldYear++;
                    worldMonth = 1;
                }
            }

            DisplayWorldTime();

            if (isButtonEnterHold() == 1) {
                statusSetUpTime = INIT_SYSTEM;
            }
            if (isButtonMode() == 1) {
                statusSetUpTime = INIT_WORLD_TIME;
            }
            if (isButtonIncrease() == 1) {
                worldTimeMode = 1;
                statusSetUpTime = NEW_YORK_TIME;
            }
            if (isButtonDecrease() == 1) {
                worldTimeMode = 3;
                statusSetUpTime = LONDON_TIME;
            }
            break;

        case LONDON_TIME:
            LcdClearS();
            worldHour = hour - 7;
            worldMinute = minute;
            worldSecond = second;
            worldDate = date;
            worldMonth = month;
            worldYear = year;
            if (worldHour > 23) {
                worldHour = worldHour - 24;
                worldDate++;
                if (worldDate > 31) {
                    worldMonth++;
                    worldDate = 1;
                }

                if (worldMonth > 12) {
                    worldYear++;
                    worldMonth = 1;
                }
            }

            DisplayWorldTime();

            if (isButtonEnterHold() == 1) {
                statusSetUpTime = INIT_SYSTEM;
            }
            if (isButtonMode() == 1) {
                statusSetUpTime = INIT_WORLD_TIME;
            }
            if (isButtonIncrease() == 1) {
                worldTimeMode = 2;
                statusSetUpTime = PARIS_TIME;
            }
            if (isButtonDecrease() == 1) {
                worldTimeMode = 1;
                statusSetUpTime = NEW_YORK_TIME;
            }
            break;
    }
}