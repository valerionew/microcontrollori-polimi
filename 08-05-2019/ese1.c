// step 0
// kitt 2.0
// contatore su lcd a 1 Hz fino a 255
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

volatile unsigned int millisecondsKitt = 0;
volatile unsigned int millisecondsCounter = 0;
volatile unsigned int secondsCounter = 0;

volatile unsigned int delayKitt_ms = 50;
volatile unsigned int millisecondsKeepalive = 0;
volatile int millimeters = 0;
short i = 0;

#define BUTTONS (PORTA & 0x3F)


volatile struct{
  short kittDir:1;
  short lcdUpdate:1;
  short counterEnable:1;
  short ultrasoundValid:1;
} flags;

void main() {
  char LCDBuffer[17];
  char LCDBuffer_time[7];
  short old_porta;
  short diff_porta;

  flags.kittDir = 0;
  flags.lcdUpdate = 0;
  flags.counterEnable = 1;


  // I/O init
  TRISD = 0;
  TRISA |= 0x3F;
  ANSELA = 0;

  // PIN setting
  TRISC |= 0b10111111;
  LATC |= 0b01000000;
  ANSELC &= ~(1<<2);

  // timer 1 setting
  T1CON =  0b00000011;

  // CCP peripheral settings
  flags.ultrasoundValid = 0;
  CCP1CON  |= 0b00000101; //configurazione di capture su rising edge
  CCPTMRS0 |= 0; // select timer 1 as clock source
  PIR1 &= ~( 1<<3 );
  PIE1 = 0b00000100; // enable CCP interurpt


  // timer0 init
  // TMR0ON | T08BIT | T0CS | T0SE | PSA(~prescaler on inverted) | T0PS<0:2>(prescaler set bits)
  T0CON = 0b11000111;// TIMER 0 CONTROL REGISTER.
  // GIEH PEIE TIM0IE INT0IE [...]
  INTCON |= 0b11100000;

  old_porta = BUTTONS;
    // lcd init
  Lcd_Init();
  Lcd_Cmd(_LCD_TURN_ON);
  Lcd_Cmd(_LCD_CLEAR);



  while(1){

    // polling
    diff_porta = old_porta ^ BUTTONS;
    if(diff_porta){  // xor per rising edge
      if((diff_porta & 0x01) && ~(old_porta & 0x01)){ // 1st button pressed + rising
        flags.counterEnable = 1; // start the counter
      }
      if((diff_porta & 0x02) && ~(old_porta & 0x02)){ // 2nd button pressed + rising
        flags.counterEnable = 0; // stop the counter
        flags.lcdUpdate = 1;
      }
      if((diff_porta & 0x04) && ~(old_porta & 0x04)){ // 3rd button pressed + rising
        flags.counterEnable = 0; // stop the counter
        secondsCounter = 0; // reset counter
        millisecondsCounter = 0; // reset counter
        flags.lcdUpdate = 1;
      }
      if((diff_porta & 0x08) && ~(old_porta & 0x08)){ // 4th button pressed + rising
        delayKitt_ms+=25; // incremet kitt delay
      }
      if((diff_porta & 0x10) && ~(old_porta & 0x10)){ // 5th button pressed + rising
        if(delayKitt_ms > 25){
          delayKitt_ms -= 25;
        }
      }
      if((diff_porta & 0x20) && ~(old_porta & 0x20)){ // 5th button pressed + rising
        Lcd_Cmd(_LCD_CLEAR); // clear line on rising edge
      }
    }
    old_porta = BUTTONS;

    if((BUTTONS & 0x20)  && ~(millisecondsKeepalive > 0)){ // 6th button pressed + timeout
      millisecondsKeepalive = 2000;
      flags.lcdUpdate = 1;
    }



    // updates
    if(millisecondsKitt > delayKitt_ms){
      LATD = ~((0xFF>>i) & (0xFF<<i));
      i = (flags.kittDir) ? i-1 : i+1;
      if(i < 1 || i > 3){
        flags.kittDir = ~flags.kittDir;
      }
      millisecondsKitt = 0;
    }

    if(flags.lcdUpdate){
    
      // opz 1 -> IntToStr ha larghezza fissa, bisogna tagliarla
/*      LCDBuffer[0] = '\0';
      IntToStr(secondsCounter/3600, LCDBuffer_time);
      strcat(LCDBuffer, LCDBuffer_time);
      strcat(LCDBuffer, ":");
      IntToStr(secondsCounter/60, LCDBuffer_time);
      strcat(LCDBuffer, LCDBuffer_time);
      strcat(LCDBuffer, ":");
      IntToStr(secondsCounter%60, LCDBuffer_time);
      strcat(LCDBuffer, LCDBuffer_time);
      strcat(LCDBuffer, ":");
      IntToStr(millisecondsCounter/10, LCDBuffer_time);
      strcat(LCDBuffer, LCDBuffer_time);    */
      
// opz 2
      LCDBuffer[0] = '\0';
      IntToStr(secondsCounter/3600, LCDBuffer_time);
      if(LCDBuffer_time[4] == ' ')
            LCDBuffer_time[4] = '0';
      strcat(LCDBuffer, Ltrim(LCDBuffer_time));
      strcat(LCDBuffer, ":");
      IntToStr(secondsCounter/60, LCDBuffer_time);
      if(LCDBuffer_time[4] == ' ')
            LCDBuffer_time[4] = '0';
      strcat(LCDBuffer, Ltrim(LCDBuffer_time));
      strcat(LCDBuffer, ":");
      IntToStr(secondsCounter%60, LCDBuffer_time);
      if(LCDBuffer_time[4] == ' ')
            LCDBuffer_time[4] = '0';
      strcat(LCDBuffer, Ltrim(LCDBuffer_time));
      strcat(LCDBuffer, ":");
      IntToStr((millisecondsCounter%1000)/10,LCDBuffer_time);
      if(LCDBuffer_time[4] == ' ')
            LCDBuffer_time[4] = '0';
      strcat(LCDBuffer, Ltrim(LCDBuffer_time));

      // opz 3 -> "demo limit" vogliono i soldi per usare le funzioni standard del C
      //sprintf(LCDBuffer, "%02d:%02d:%02d:%02d", secondsCounter/3600, secondsCounter/60, secondsCounter%60, millisecondsCounter/10);

      Lcd_Out(1,1, LCDBuffer);
      
      PORTC = 0;

      if(millisecondsKeepalive > 0){
        IntToStr(millimeters, LCDBuffer);
        Lcd_Out(2,1, LCDBuffer);
      }
      else{
        Lcd_Out(2,1, "Press RA5");
      }
    }

  }

}


void interrupt(){
  if(INTCON.TMR0IF){
    millisecondsKitt += 8;

    if(flags.counterEnable){
      millisecondsCounter += 8;
      if(millisecondsCounter > 1000){
        secondsCounter += 1;
        millisecondsCounter -= 1000;
        flags.lcdUpdate = 1;
      }
    }

    if (millisecondsKeepalive > 0) {
      millisecondsKeepalive -= 8;
    }

    INTCON.TMR0IF = 0;
  }
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

      flags.ultrasoundValid = 1; // the value can be displayed
    }
     CCP1CON ^= 1; // invert the edge
     PIR1.CCP1IF = 0; // clear the interrupt flag
  }
}

// step 1
// kitt con delay variabile
// solo polling della transizione L->H
// il reset torna il cronometro a zero e lo ferma


// step 2
// refresh dinamico del display
// uscita formattata
// distnza something something con l'ultrasuoni latchata con RA5
//

// step 3
// visualizzazione formattata hh:mm:ss:xx (con i centesimi)