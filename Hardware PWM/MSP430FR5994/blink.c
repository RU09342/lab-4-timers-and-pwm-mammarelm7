/*
Matt Mammarelli
9/18/17
ECE 09342-2
*/

//MSP430FR5994 Hardware PWM
//green led toggles when button pressed
//red led affected by PWM
//need p1sel0, changed ledSetup() P1SEL0 |= BIT0;, P1SEL0 &= ~BIT1, P5SEL0 &= ~BIT6;
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

    _BIS_SR(LPM4_bits + GIE);       //Enter low power mode

}




void ledSetup(){

        //selects peripheral mode pwm for both red led
        P1SEL0 |= BIT0;
        //clears all other bits so they are not pwm
        P1SEL0 &= ~BIT1;
        //Disables button multiplexer
        P5SEL0 &= ~BIT6;


        //red,green led out
        P1DIR |= (BIT0|BIT1);


        //makes sure green led is off
        P1OUT &= ~(BIT1);



}
void buttonSetup(){

    //resistor enabled input P5.6
       P5REN |= BIT6;

       //Sets Resistor to pullup, 1.0 is low, has to use = to initialize
       P1OUT = BIT6;


       P5IE |=BIT6; //enable interrupt on port 5.6
       P5IES |=BIT6; //sets as falling edge
       P5IFG &=~(BIT6);//clear interrupt flag

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
#pragma vector=PORT5_VECTOR
__interrupt void Port_5(void) {

    if (!((P5IN & BIT6) == BIT6)){
               P1OUT^=BIT1; //toggle red led

               if (TA0CCR1<=1000){
                   TA0CCR1 = TA0CCR1+100; //increments green led duty cycle

               }
               else {
                   TA0CCR1 = 0; //resets duty cycle back to 0%
               }
           }

           //clears interrupt flag to exit interrupt
           P5IFG &= ~BIT6;
}




