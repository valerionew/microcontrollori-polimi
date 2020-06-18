/*
*     CONSEGNA:
*     grabbare da piazza il kittcar con pulsante
*     levare la delay bloccante
*     sostituirla con tim0int
*
*
*     HARDERCORE
*         kitcar su display :((((((
*         SAD CHALLENGEEEEEEEE
*
*
*/

unsigned int kitt_delay = 0;

const char character[] = {0,10,10,0,17,17,14,0};

void CustomChar(char pos_row, char pos_char) {
  char i;
    Lcd_Cmd(64);
    for (i = 0; i<=7; i++) Lcd_Chr_CP(character[i]);
    Lcd_Cmd(_LCD_RETURN_HOME);
    Lcd_Chr(pos_row, pos_char, 0);
}


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


struct {
  volatile unsigned char updatekitt : 1;

} flags;
/*
Reproduce the "Kitt Car effect" on PORTC
*/

void main() {

  short unsigned int dirfwd = 0;
  short unsigned int pos = 1;

  ANSELB = 0;
  TRISB = 255;
  IOCB = 0b11000000;

  TRISC = 0;
  T0CON = 0b11000111;
  INTCON = 0b10101000;        // Set IOCB and GIE

  LATC = 1;
  Lcd_Init();
  Lcd_Cmd(_LCD_TURN_ON);
  Lcd_Cmd(_LCD_CLEAR);


  while(1){
    if(flags.updatekitt){
      flags.updatekitt = 0;
      if(dirfwd){
              pos++;
      }
      else{
           pos--;
      }
      if(pos > 16 || pos < 1)
             dirfwd = !dirfwd;
      Lcd_Cmd(_LCD_CLEAR);
      CustomChar(1, pos);
      


    }
  }
}



void interrupt(){        //ISR

  if(INTCON.RBIF){

    if(PORTB.RB6)
    kitt_delay -= 10;
    else if(PORTB.RB7)
    kitt_delay += 10;

    if(kitt_delay < 10)
    kitt_delay = 10;
    else if(kitt_delay > 255)
    kitt_delay = 255;

    INTCON.RBIF = 0;

  }
  if(INTCON.TMR0IF){
    if (
    flags.updatekitt = 1;
    if(
    INTCON.TMR0IF = 0;
  }
}