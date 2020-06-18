void main() {
     TRISD = 0; // PORTD COME OUTPUT
     LATD = 0; // RESET LATD PORT
     
     // TMR0ON | T08BIT | T0CS | T0SE | PSA(~prescaler on inverted) | T0PS<0:2>(prescaler set bits)
     T0CON = 0b11000111;// TIMER 0 CONTROL REGISTER.
     // GIEH PEIE TIM0IE INT0IE [...]
     INTCON = 0b10100000;
}

void interrupt(){
     if(INTCON.TMR0IF){
                       LATD++;
                       INTCON.TMR0IF = 0;
     }
}