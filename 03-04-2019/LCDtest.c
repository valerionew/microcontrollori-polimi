

sbit LCD_RS at LATB4_bit;
sbit LCD_EN at LATB5_bit;
sbit LCD_D4 at LATB0_bit;
sbit LCD_D5 at LATB1_bit;
sbit LCD_D6 at LATB2_bit;
sbit LCD_D7 at LATB3_bit;

sbit LCD_RS_Direction at TRISB4_bit;
sbit LCD_EN_Direction at TRISB5_bit;
sbit LCD_D4_Direction at TRISB0_bit;
sbit LCD_D5_Direction at TRISB1_bit;
sbit LCD_D6_Direction at TRISB2_bit;
sbit LCD_D7_Direction at TRISB3_bit;

#include <stdio.h>
#ifndef NULL
        #pragma warning "non ho incluso stdio perchè sono un ide stupido che vuole i soldi"
#endif

void main(){
    unsigned short int mionumero = 59;
    char bufferino[17];
    
    
    Lcd_Init();
    Lcd_Cmd(_LCD_TURN_ON);
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Out(1,1, bufferino);



}