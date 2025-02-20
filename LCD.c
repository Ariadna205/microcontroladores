#include <xc.h>
#include "LCD.h"
#include "config.h"
#include <pic16f887.h>
#include <stdio.h>
#include <string.h>
#include "KEYPAD.h"


/*F**************************************************************************
* NAME: LCD_Init
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Initialize LCD 16x2
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void LCD_Init(void)
{
    TRISC = 0x00; // Configura el puerto como salida.
    RS = 0;
    EN = 0;
    ldata = 0xFF; // Prueba enviando datos altos
    TRISC0 = 0; // RS como salida.
    TRISC1 = 0; // EN como salida.
    TRISC4 = 0; // D4 como salida.
    TRISC5 = 0; // D5 como salida.
    TRISC6 = 0; // D6 como salida.
    TRISC7 = 0; // D7 como salida.
          
    LCD_Port = 0;         /*PORT as Output Port*/
    __delay_ms(15);          /* 15 ms, Power-On delay*/
    LCD_Command(0x02);    /*send for initialization of LCD with nibble method */
    LCD_Command(0x28);    /*use 2 line and initialize 5*7 matrix in (4-bit mode)*/
    LCD_Command(0x01);    /*clear display screen*/
    LCD_Command(0x0c);    /*display on cursor off*/
    LCD_Command(0x06);    /*increment cursor (shift cursor to right)*/	
}

/*F**************************************************************************
* NAME: LCD_Command
*----------------------------------------------------------------------------
* PARAMS:   
* cmd: Command to execute in LCD (example: 0x01 clear display screen, 0x0C display On, Cursor Off)
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Send Command to LCD 16x2
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void LCD_Command(unsigned char cmd )
{
	ldata = (ldata & 0x0f) |(0xF0 & cmd);  /*Send higher nibble of command first to PORT*/ 
	RS = 0;  /*Command Register is selected i.e.RS=0*/ 
	EN = 1;  /*High-to-low pulse on Enable pin to latch data*/ 
	NOP();
	EN = 0;
	__delay_ms(1);
    ldata = (unsigned char)((ldata & 0x0f) | (cmd<<4));  /*Send lower nibble of command to PORT */
	EN = 1;
	NOP();
	EN = 0;
	__delay_ms(3);
}

/*F**************************************************************************
* NAME: LCD_Char
*----------------------------------------------------------------------------
* PARAMS:   
* dat: character to display in LCD
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Display Character to LCD 16x2
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/

void LCD_Char(unsigned char dat)
{
	ldata = (ldata & 0x0f) | (0xF0 & dat);  /*Send higher nibble of data first to PORT*/
	RS = 1;  /*Data Register is selected*/
	EN = 1;  /*High-to-low pulse on Enable pin to latch data*/
	NOP();
	EN = 0;
	__delay_ms(1);
    ldata = (unsigned char)((ldata & 0x0f) | (dat<<4));  /*Send lower nibble of data to PORT*/
	EN = 1;  /*High-to-low pulse on Enable pin to latch data*/
	NOP();
	EN = 0;
	__delay_ms(3);
}

void LCD_String(const char *msg)
{
	while((*msg)!=0)
	{		
	  LCD_Char(*msg);
	  msg++;	
    }
}


void LCD_String_xy(char row,char pos,const char *msg)
{
    char location=0;
    if(row<=1)
    {
        location=(0x80) | ((pos) & 0x0f);  /*Print message on 1st row and desired location*/
        LCD_Command(location);
    }
    else
    {
        location=(0xC0) | ((pos) & 0x0f);  /*Print message on 2nd row and desired location*/
        LCD_Command(location);    
    }  
    

    LCD_String(msg);

}

/*F**************************************************************************
* NAME: LCD_Clear
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Clear the Display Screen
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void LCD_Clear(void)
{
   	LCD_Command(0x01);
    __delay_ms(3);
}



