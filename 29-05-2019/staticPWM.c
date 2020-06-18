

  short unsigned int dir = 0;
void main() {

  // disable the tris for the specific
  TRISE |= (1 << 2);

  T2CON = 0b00000100;
  PR2 = 0xFF;
  CCP5CON = 0b00001100;
  CCPTMRS1 = 0;
  CCPR5L = 64; // target timing: 0-> 0 duty 255-> 100% duty

  
  TRISE &= ~(1 << 2);

  while(1){
    if(!dir){
      CCPR5L++;
    }
    else{
      CCPR5L--;
    }

    if(CCPR5L == 255 || CCPR5L == 0){
      dir = !dir;
    }

    delay_ms(4);

  }
}
