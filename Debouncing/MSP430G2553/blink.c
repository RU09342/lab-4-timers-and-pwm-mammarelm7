/*
Matt Mammarelli
9/18/17
ECE 09342-2
*/

//MSP430G2553 Software Debouncing
//used LMP0 instead of LMP4
//Timer A1 used to control the debouncing
//Timer A used to control speed of red LED
//Doesnt work as reliably as others

#include <msp430.h>

double toTicks(double); //converts a frequency in Hz to ticks used in ccr register
int count; //sets the state of red led

void main(void)
{
    //WDTCTL is watchdog timer control
    //WDTPW watchdog timer + password, is an interrupt
    //WDTHOLD watchdog timer +hold, if 1 watchdog timer is stopped
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

  
       //resistor enabled, input 1.3
       P1REN |= BIT3;

       //Sets Resistor to pullup, 1.0 is low, has to use = to initialize
       P1OUT = BIT3;

       //Enables port 1.0,1.6 as output, any bits that are 0 are treated as inputs
       P1DIR =BIT0|BIT6;



       P1IE |=BIT3; //enable interrupt on port 1.3
       P1IES |=BIT3; //sets as falling edge
       P1IFG &=~(BIT3);//clear interrupt flag


       //TA0CTL = Timer A0 Control
        //TASSEL_1 Timer_A clock source select = 01 ACLK 32k
        //MC_1 Up Mode

        TA0CTL = TASSEL_1 + MC_1;

        //Timer A0 in compare mode
        TA0CCTL0 = 0x0010;

        //initial rate the led blinks
          TA0CCR0 = 10000;


          count = 0;


       //enter LPM0 mode and enable global interrupt
       _BIS_SR(LPM0_bits + GIE);

}

//timer A interrupt
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A(void) //double __
{

    P1IE  |= BIT3; //enables interrupt on port 1.3
    P1OUT^=BIT0;


}


//timer A1 interrupt debounce
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer_A1 (void) {


    P1IE  |= BIT3; //enables interrupt on port 1.3
    P1OUT ^=BIT6; //green led lights when debounce is done


}

//button interrupt, ta0ccr0 will count from 0 to the final value when button is released
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {
    P1IE  &= ~BIT3; //disables interrupt on port 1.3, stops checking for rising edges


       //Start Timer B for Debounce, will wait 500 ticks
       //TA1CTL = Timer1 Control
       //TASSEL_1 Timer1 clock source select = 01 ACLk =32khz
       //MC_2 Continuous Mode makes changing state more reliable

       TA1CTL = TASSEL_1 + MC_2;

       //Timer A0 in compare mode
       TA1CCTL0 = 0x0010;
       //delay for debounce
       TA1CCR0 = 500;




       //changes states of the red led

       count = count+1;

       switch(count){
       case 1:{

           TA0CCR0 = 40000;
           break;
       }
       case 2:{
           TA0CCR0 = 60000;
           break;
       }
       default:{
           TA0CCR0 = 10000;
           count=0;
       }
       }



    //clears interrupt flag to exit interrupt
    P1IFG &= ~BIT3;
}

//converts frequency to ticks
double toTicks(double frequency){
    //f=ACLK/ticks
    //f=32k/ticks
    //ticks=32k/f
    return 32000/frequency;
}
