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

volatile unsigned int potSample;
volatile unsigned short distanceSample;

struct {
  unsigned short int potValid:1;
  unsigned short int distanceValid:1;
} flags;

void main() {
  char LCDBuffer[7];
  flags.potValid = 0;
  flags.distanceValid = 0;


  // configure adc, doesnt enable it yet
  ADCON2 |= 0b00010110;
  ADCON1 |= 0b00000000;
  ADCON0 |= 0b00111101;

  // enable peripheral interrupts for ADC
  INTCON |= 0b11000000;
  PIE1   |= 0b01000000;

  // adc GO on an0 first
  ADCON0 |= (1<<1);

  // configure PORTD
  TRISD = 0; //port D as output

  // configure PORTC for ultrasoud
  TRISC = 0b10111111;
  LATC = 0b01000000;
  ANSELC &= ~(1<<2);

  Lcd_Init();
  Lcd_Cmd(_LCD_TURN_ON);
  Lcd_Cmd(_LCD_CLEAR);

  while(1){

    if(flags.potValid){
      flags.potValid = 0;
      IntToStr(potSample, LCDBuffer);
      Lcd_Out(1,1, "pot = ");
      Lcd_Out(1,7, LCDBuffer);
    }
    if(flags.distanceValid){
      flags.distanceValid = 0;
      IntToStr(distanceSample*20, LCDBuffer);
      Lcd_Out(2,1, "dist = ");
      Lcd_Out(2,7, LCDBuffer);
      Lcd_Out(2,14, "mm");

      if(distanceSample > 32){
        distanceSample = 255;
      }
      else{
        distanceSample = distanceSample << 3; 
      }

      PORTD = binaryToThermo(distanceSample);
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
    if((ADCON0)>> 2 == 0){
      potSample = ((unsigned int)ADRESH << 8) + ADRESL;
      flags.potValid = 1;
      PIR1.ADIF = 0;
      ADCON2.ADFM = 0;
      ADCON0 = 0b00111101; // change to an15
      ADCON0 |= (1<<1);
    }
    else if((ADCON0)>> 2 == 15){
      distanceSample = ADRESH;
      flags.distanceValid = 1;
      PIR1.ADIF = 0;
      ADCON2.ADFM = 1;
      ADCON0 = 0b00000001; // change to an0
      ADCON0 |= (1<<1);
    }
  }
}