#ifndef __io_h__
#define __io_h__

void LCD_init();
void LCD_ClearScreen(void);
void LCD_WriteCommand (unsigned char Command);
void LCD_Cursor (unsigned char column);
void LCD_DisplayString(unsigned char column ,const unsigned char *string);
void LCD_DisplayString_NoClear(unsigned char column, const unsigned char *string);
void LCD_WriteData(unsigned char Data);
void delay_ms(int miliSec);
unsigned char* LCD_To_String(unsigned short num, unsigned char* string, unsigned char size);
#endif

