
// usiamo il capture su CCP1
// timer1 a fosc/4, in free running
// troviamo nel PIR1 (CCP1) le config
// necessita di pll
// un colpo di fosc/4 è 125 ns  -> shiftiamo



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

volatile unsigned int millimeters;

struct {
  volatile unsigned char validmeasure : 1;
} flags;

void main() {

    char textbuffer[7];

    // PIN setting
    TRISC = 0b10111111;
    LATC = 0b01000000;
    ANSELC &= ~(1<<2);

    // timer 1 setting
    T1CON =  0b0000011;

    // CCP peripheral settings
    flags.validmeasure = 0;
    CCP1CON  = 0b00000101; //configurazione di capture su rising edge
    CCPTMRS0 = 0; // select timer 1 as clock source
    INTCON = 0b11000000;  // enable interrupt global e peripheral
    PIR1 = 0;
    PIE1 = 0b00000100; // enable CCP interurpt

    // LCD SET
    Lcd_Init();
    Lcd_Cmd(_LCD_TURN_ON);
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Out(1,1, "hello world");

    while(1){
      if(flags.validmeasure){
        IntToStr(millimeters, textbuffer);
        Lcd_Out(2,1, textbuffer);
        flags.validmeasure = 0;
      }

    }


}

void interrupt(){
    if(PIR1.CCP1IF){
      if(CCP1CON.CCP1M0){
        // if true, lowest bit is set to 1: interrupt was on rising edge
        // we expect to start conversion here
        millimeters = (CCPR1H << 8) + CCPR1L;
      }
      else {
        // we are terminating the conversion
        millimeters = (CCPR1H << 8) + CCPR1L - millimeters;
        millimeters = millimeters >> 3; // shift to convert from fosc/4 ticks in mm

        flags.validmeasure = 1; // the value can be displayed
      }
       CCP1CON ^= 1; // invert the edge
       PIR1.CCP1IF = 0; // clear the interrupt flag
    }

}
