

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

volatile int ccp_millimeters;
volatile int ADC_millimeters;
volatile unsigned char lcd_update = 0;

struct {
  volatile unsigned char validmeasure : 1;
  volatile unsigned char validconversion : 1;
} flags;

void main() {

    char LCDBuffer[7];

    // PIN setting
    TRISC |= 0b10111111;
    LATC |= 0b01000000;
    ANSELC &= ~( (1<<2) | (1<<3) );


    // ADC settings
    ADCON2 |= 0b10110110; // right just, res, s&h time,
    ADCON1 |= 0;  // all default
    ADCON0 |= 0b00111101; // select channel, enable and start

    // ADC interrupt settings
    flags.validconversion = 0;
    PIR1 &= ~( 1<<6 ); // clear flag
    PIE1 |= 0b01000000; // enable ADC peripheral interurpt

    // timer 1 setting
    T1CON |= 0b00000011;
    PIE1 |= 0b00000001; // enable timer 1 overflow interrupt for display refresh

    // CCP peripheral settings
    flags.validmeasure = 0;
    CCP1CON  |= 0b00000101; //configurazione di capture su rising edge
    CCPTMRS0 |= 0; // select timer 1 as clock source

    PIR1 &= ~( 1<<3 );
    PIE1 |= 0b00000100; // enable CCP interurpt

    INTCON |= 0b11000000;  // enable interrupt global e peripheral
    ADCON0 |= 0b00000010;
    // LCD SET
    Lcd_Init();
    Lcd_Cmd(_LCD_TURN_ON);
    Lcd_Cmd(_LCD_CLEAR);
    while(1){
      if(lcd_update > 50){
        if(flags.validmeasure){
          IntToStr(ccp_millimeters, LCDBuffer);
          Lcd_Out(1,1, "CCP = ");
          Lcd_Out(1,6, LCDBuffer);
          flags.validmeasure = 0;
        }
        if(flags.validconversion){
          IntToStr(adc_millimeters, LCDBuffer);
          Lcd_Out(2,1, "ADC = ");
          Lcd_Out(2,6, LCDBuffer);
          ADCON0 |= 0b00000010;
          flags.validconversion = 0;
        }
        lcd_update = 0;
      }
    }


}

void interrupt(){
    if(PIR1.CCP1IF){
      if(CCP1CON.CCP1M0){
        // if true, lowest bit is set to 1: interrupt was on rising edge
        // we expect to start conversion here
        ccp_millimeters = (CCPR1H << 8) + CCPR1L;
      }
      else {
        // we are terminating the conversion
        ccp_millimeters = (CCPR1H << 8) + CCPR1L - ccp_millimeters;
        ccp_millimeters = ccp_millimeters >> 3; // shift to convert from fosc/4 ticks in mm

        flags.validmeasure = 1; // the value can be displayed
      }
       CCP1CON ^= 1; // invert the edge
       PIR1.CCP1IF = 0; // clear the interrupt flag
    }
    if(PIR1.ADIF){
      ADC_millimeters = ( (ADRESH << 8) | (ADRESL) ) * 5;
      flags.validconversion = 1;
      PIR1.ADIF = 0;
    }
    if(PIR1.TMR1IF){
      lcd_update++;
      PIR1.TMR1IF = 0;
    }

}