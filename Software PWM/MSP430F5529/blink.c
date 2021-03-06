/*
Matt Mammarelli
9/18/17
ECE 09342-2
*/

//MSP430F5529 Software PWM
//green led toggles when button pressed
//red led affected by PWM
//ledSetup changed  P1SEL &= ~(BIT0); , timerSetup changed so TA0CCTL1 = CCIE instead of OUTMOD_7 , need interrupt isr vectors for this board

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
    //selects gpio mode for both leds
    P1SEL &= ~(BIT0);
    P4SEL &= ~(BIT0);

    //red led out
    P1DIR |= (BIT0);

    //green led out
    P4DIR |= (BIT7);

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

    //sets cctl1 and 0 to compare mode
    TA0CCTL1 = (CCIE);
    TA0CCTL0 = (CCIE);

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
                TA0CCR1 = TA0CCR1+100; //increments red led duty cycle

            }
            else {
                TA0CCR1 = 0; //resets duty cycle back to 0%
            }
        }

        //clears interrupt flag to exit interrupt
        P1IFG &= ~BIT1;
}


//Timer A interrupt vectors
#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer0_A1_ISR (void)
{
    if(TA0CCR1 != 1000)
    {
       P1OUT &= ~(BIT0); //turns off red led
    }
    TA0CCTL1 &= ~BIT0; //clears flag
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0_ISR (void)
{
    P1OUT |= (BIT0); //turns on red led
    TA0CCTL0 &= ~BIT0;  //clears flag
}
