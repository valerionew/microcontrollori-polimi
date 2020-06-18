
void main() {
     struct{
            unsigned int increment  :1;
     } flags;
  TRISC = 0;
  PORTC = 1;

  do {
    if (LATC  > 127) flags.increment = 1; // 1 = backwards
    if (LATC <= 1) flags.increment = 0; // 0 = forward
     PORTC = (flags.increment == 0) ? LATC << 1: LATC >> 1;
     Delay_ms(100);
  } while(1);          // Endless loop
}
