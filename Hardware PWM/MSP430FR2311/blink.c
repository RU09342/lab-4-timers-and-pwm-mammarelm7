/*
Matt Mammarelli
9/18/17
ECE 09342-2
*/

//MSP430FR2311 Hardware PWM
//green led toggles when button pressed
//red led affected by PWM
//need p1sel0, changed ledSetup()
//changed timerSetup() TA0CCTL1 = (OUTMOD_7);, removed Timer ISRS

#include <msp430.h>


void ledSetup();
void buttonSetup();
void timerSetup();


void main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer


    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    //PM5CTL0 = Power mode 5 control register 0
    //LOCKLPM5 = Locks I/O pin, bit is reset by a power cycle
    //~LOCKLPM5=8'b0 and by anding PM5CTL0 it clears the register
    PM5CTL0 &= ~LOCKLPM5;

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
    //selects peripheral mode for red leds
    P1SEL0 |= (BIT0);
    //non pwm bits cleared
    P2SEL0 &= ~(BIT0);
    //button multiplexor disabled
    P1SEL0 &= ~BIT1;
    //red led out
    P1DIR |= (BIT0);

    //green led out
    P2DIR |= (BIT0);

    //makes sure green led is off
    P2OUT &= ~(BIT0);

}

void buttonSetup()
{
    //resistor enabled,
       P1REN |= BIT1;

       //Sets Resistor to pullup, 1.0 is low, has to use = to initialize
       P1OUT = BIT1;


       P1IE |=BIT1; //enable interrupt on port 1.3
       P1IES |=BIT1; //sets as falling edge
       P1IFG &=~(BIT1);//clear interrupt flag
}

void timerSetup()
{
    // SMCLK, Up Mode (Counts to TA0CCR0)
    TB0CTL |= TBSSEL_2 + MC_1;

    //sets cctl1 and 0 to compare mode

    TB0CCTL1 = (OUTMOD_7);

    // PWM period, f=1MHz/1001 = 1khz
    TB0CCR0 = 1000;
    // TA0CCR1 PWM 50% duty cycle
    TB0CCR1 = 500;

}



// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) // function called when the port is interrupted (button pressed)
{
    if (!((P1IN & BIT1) == BIT1)){
            P2OUT^=BIT0; //toggle green led

            if (TB0CCR1<=1000){
                TB0CCR1 = TB0CCR1+100; //increments red led duty cycle

            }
            else {
                TB0CCR1 = 0; //resets duty cycle back to 0%
            }
        }

        //clears interrupt flag to exit interrupt
        P2IFG &= ~BIT0;
}


//Timer A interrupt vectors
#pragma vector=TIMER0_B1_VECTOR
__interrupt void Timer0_B1_ISR (void)
{
    if(TB0CCR1 != 1000)
    {
       P1OUT &= ~(BIT0); //turns off red led
    }
    TB0CCTL1 &= ~BIT0; //clears flag
}

#pragma vector=TIMER0_B0_VECTOR
__interrupt void Timer0_B0_ISR (void)
{
    P1OUT |= (BIT0); //turns on red led
    TB0CCTL0 &= ~BIT0;  //clears flag
}
