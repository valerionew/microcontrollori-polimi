

short unsigned tempo = 100;

void main() {
     struct{
            unsigned int increment  :1;
     } flags;
  TRISC = 0; // SET AS OUTPUT ( TRISTATE C = 0)
  PORTC = 1; // PUT THE 1 TO PLAY PINGPONG WITH IN THE REGISTER
  TRISB.RB6 = 1;
  TRISB.RB7 = 1;


  // interrupt init
  IOCB = 0b11000000; //    IOCB7; IOCB7; IOCB7; IOCB7; NC; NC; NC; NC;
  INTCON  = 0b10001000; // General IEn; Peripheral IEn; TMR0 IEn; INT0 IEn; Reg B IEen; TMR0 IFlag; INT0 IFlag; RB IFlag;



  do {
    if (LATC  > 127) flags.increment = 1; // 1 = backwards
    if (LATC <= 1) flags.increment = 0; // 0 = forward
     LATC = (flags.increment == 0) ? LATC << 1: LATC >> 1;
     Vdelay_ms(tempo+1);
  } while(1);          // Endless loop
}

void interrupt(){
     if(INTCON & 1){
               if(PORTB.RB6) tempo += 10;
               if(PORTB.RB7) tempo -= 10;
     INTCON &= ~(1);
     }

}