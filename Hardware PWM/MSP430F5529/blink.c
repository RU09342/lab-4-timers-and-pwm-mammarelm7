/*
Matt Mammarelli
9/18/17
ECE 09342-2
*/

//MSP430F5529 hardware PWM
//green led toggles when button pressed
//red led affected by PWM
//need p1sel, changed ledSetup()
//changed timerSetup() TA0CCTL1 = (OUTMOD_7);, removed Timer ISRS


#include <msp430.h>


void ledSetup();
void buttonSetup();
void timerSetup();


void main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer

    //init leds
    ledSetup();
    //init buttons
    buttonSetup();
    //init timer
    timerSetup();


    _BIS_SR(LPM4_bits + GIE);// Has to be LMP0 for G2
}

void ledSetup()
{
            //selects peripheral mode pwm for both red led
            P1SEL |= BIT0;
            //clears all other bits so they are not pwm
            P4SEL &= ~BIT7;
            //Disables button multiplexer
           P1SEL &= ~BIT1;


            //red,green led out
            P1DIR |= BIT0;
            P4DIR |= BIT7;


            //makes sure green led is off
            P4OUT &= ~(BIT7);

}

void buttonSetup()
{
    //resistor enabled,
       P1REN |= BIT1;

       //Sets Resistor to pullup, 1.0 is low, has to use = to initialize
       P1OUT = BIT1;


       P1IE |=BIT1; //enable interrupt
       P1IES |=BIT1; //sets as falling edge
       P1IFG &=~(BIT1);//clear interrupt flag
}

void timerSetup()
{
    // SMCLK, Up Mode (Counts to TA0CCR0)
    TA0CTL |= TASSEL_2 + MC_1;

    //sets cctl1 and to reset/set
    TA0CCTL1 = (OUTMOD_7);

    // PWM period, f=1MHz/1001 = 1khz
    TA0CCR0 = 1000;
    // TA0CCR1 PWM 50% duty cycle
    TA0CCR1 = 500;

}



// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) // function called when the port is interrupted (button pressed)
{
    if (!((P1IN & BIT1) == BIT1)){
            P4OUT^=BIT7; //toggle green led

            if (TA0CCR1<=1000){
                TA0CCR1 = TA0CCR1+100; //increments green led duty cycle

            }
            else {
                TA0CCR1 = 0; //resets duty cycle back to 0%
            }
        }

        //clears interrupt flag to exit interrupt
        P1IFG &= ~BIT1;
}


