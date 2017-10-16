/*
Matt Mammarelli
9/18/17
ECE 09342-2
*/

//MSP430FR6989 Hardware PWM
//green led toggles when button pressed
//red led affected by PWM
//need p1sel0, changed ledSetup()
//changed timerSetup() TA0CCTL1 = (OUTMOD_7);, removed Timer ISRS
//DEFINITELY WORKS

#include <msp430.h>

void ledSetup();
void buttonSetup();
void timerSetup();


void main(void){

    // Stop watchdog timer
    WDTCTL = WDTPW + WDTHOLD;

    // Disable the GPIO power-on default high-impedance mode
        // to activate previously configured port settings
        //PM5CTL0 = Power mode 5 control register 0
        //LOCKLPM5 = Locks I/O pin, bit is reset by a power cycle
        //~LOCKLPM5=8'b0 and by anding PM5CTL0 it clears the register
        PM5CTL0 &= ~LOCKLPM5;

    ledSetup(); //init leds
    buttonSetup(); //init buttons
    timerSetup(); //init timer

    _BIS_SR(LPM0_bits + GIE);       // Has to be LMP0 for G2


}
void ledSetup(){




    //selects peripheral mode for red led
       P1SEL0 |= (BIT0);
       //non pwm bits cleared
       P9SEL0 &= ~(BIT7);
       //button multiplexor disabled
       P1SEL0 &= ~BIT1;
       //red led out
       P1DIR |= (BIT0);

       //green led out
       P9DIR |= (BIT7);

       //makes sure green led is off
       P9OUT &= ~(BIT7);


}
void buttonSetup(){

    //resistor enabled, input 1.1
    P1REN |= BIT1;

    //Sets Resistor to pullup, 1.0 is low, has to use = to initialize
    P1OUT = BIT1;


    P1IE |=BIT1; //enable interrupt on port 1.1
    P1IES |=BIT1; //sets as falling edge
    P1IFG &=~(BIT1);//clear interrupt flag

}
void timerSetup(){

    // SMCLK, Up Mode (Counts to TA0CCR0)
            TA0CTL |= TASSEL_2 + MC_1;

            //sets cctl1 and to reset/set
            TA0CCTL1 = (OUTMOD_7);

            // PWM period, f=1MHz/1001 = 1khz
            TA0CCR0 = 1000;
            // TA0CCR1 PWM 50% duty cycle
            TA0CCR1 = 500;



}

//button interrupt
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {

    if (!((P1IN & BIT1) == BIT1)){
        P9OUT^=BIT7; //toggle red led

        if (TA0CCR1<=1000){
            TA0CCR1 = TA0CCR1+100; //increments red led duty cycle

        }
        else {
            TA0CCR1 = 0; //resets duty cycle back to 0%
        }
    }

    //clears interrupt flag to exit interrupt
    P1IFG &= ~BIT1;
}




