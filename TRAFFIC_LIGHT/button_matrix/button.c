#include "button.h"

unsigned int key_code[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned char arrayMaskOutputOfKey [4] = {0x10,0x20,0x40,0x80};
unsigned char arrayMaskInputOfKey [4] = {0x01,0x02,0x04,0x08};

unsigned char buttonDelay = 20;

void init_key_matrix()
{
	TRIS_BUTTON = 0x0f; 
	PORT_BUTTON = 0xff;
}
void init_key_matrix_with_uart()
{
    TRIS_BUTTON = TRIS_BUTTON & 0b11001111; //RC5 Output
	PORT_BUTTON = 0xff;
}

void scan_key_matrix_with_uart()
{
	int i,j;
	for(i=0;i<2;i++)
	{
		PORT_BUTTON = ~arrayMaskOutputOfKey[i];
		for(j=0;j<MAX_COL;j++)
		{
			if((PORT_BUTTON & arrayMaskInputOfKey[j]) == 0)
				key_code[i*MAX_ROW+j] = key_code[i*MAX_ROW+j] + 1;
			else
				key_code[i*MAX_ROW+j] = 0;
		}
	}
}

unsigned char isButtonEnter(int index)
{
    if (key_code[index] == 1)
        return 1;
    else
        return 0;
}

unsigned char isButtonESCHold()
{
    if (key_code[0] == 5)
        return 1;
    else
        return 0;
}

unsigned char isConformHold()
{
    if (key_code[3] == 10)
        return 1;
    else
        return 0;
}

unsigned char isButtonIncrease()
{
    if (key_code[2] == 1 || (key_code[2] > 10 && key_code[2]%3 == 1))
        return 1;
    else
        return 0;
}

unsigned char isButtonDecrease()
{
    if (key_code[1] == 1 || (key_code[1] > 10 && key_code[1]%3 == 1))
        return 1;
    else
        return 0;
}

//ham nay de giup cac ban hieu ro viec quet ma tran phim
//Sau khi da hieu ve cach quet ma tran phim thi xoa di, hoac luu lai o mot file khac
void button_delay_ms(int value)
{
	int i,j;
	for(i=0;i<value;i++)
		for(j=0;j<238;j++);
}