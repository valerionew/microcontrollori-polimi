// Lcd module connections
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
// End Lcd module connections

unsigned short int binaryToThermo(unsigned short int binary);

volatile unsigned int adcsample;

struct {
  unsigned short int conversionValid:1;
} flags;

void main() {
  char LCDBuffer[16];

  // configure adc, doesnt enable it yet
  ADCON2 |= 0b10010110;
  ADCON1 |= 0b00000000;
  ADCON0 |= 0b00000001;

  // enable peripheral interrupts for ADC
  INTCON |= 0b11000000;
  PIE1   |= 0b01000000;

  // adc GO
  ADCON0 |= (1<<1);

  Lcd_Init();
  Lcd_Cmd(_LCD_TURN_ON);
  Lcd_Cmd(_LCD_CLEAR);

  while(1){

    if(flags.conversionValid){
      IntToStr(adcsample, LCDBuffer);
      Lcd_Out(1,1, "mea = ");
      Lcd_Out(1,7, LCDBuffer);
      flags.conversionValid = 0;
    }


  }


}

unsigned short int binaryToThermo(unsigned short int binary) {
  unsigned short int thermo;
  thermo = 0xFF >> (7-(binary>>5));
  return thermo;
}

void interrupt(){
  if(PIR1.ADIF){
    adcsample = ((unsigned int)ADRESH << 8) + ADRESL;
    flags.conversionValid = 1;
    PIR1.ADIF = 0;
  ADCON0 |= (1<<1);

  }
}