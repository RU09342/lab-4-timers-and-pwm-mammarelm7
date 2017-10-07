/*
Matt Mammarelli
9/18/17
ECE 09342-2
*/

//MSP430FR5994 Software Debouncing
//Timer B used to control the debouncing
//Timer A used to control speed of red LED


#include <msp430.h>

double toTicks(double); //converts frequency to ticks for ccr

int count; //sets the state of red led

void main(void) {

  //WDTCTL is watchdog timer control
  //WDTPW watchdog timer + password, is an interrupt
  //WDTHOLD watchdog timer +hold, if 1 watchdog timer is stopped
  // Stop watchdog timer

  WDTCTL = WDTPW | WDTHOLD;   // Disables the Watchdog Timer


  // Disable the GPIO power-on default high-impedance mode
  // to activate previously configured port settings
  //PM5CTL0 = Power mode 5 control register 0
  //LOCKLPM5 = Locks I/O pin, bit is reset by a power cycle
  //~LOCKLPM5=8'b0 and by anding PM5CTL0 it clears the register
  PM5CTL0 &= ~LOCKLPM5;




  //resistor enabled input P5.6
  P5REN |= BIT6;

  //Sets Resistor to pullup, 1.0 is low, has to use = to initialize
  P5OUT = BIT6;


  //Enables port 1.0 as output
  P1DIR = BIT0|BIT1;


  P5IE  |= BIT6;   // Enables the interrupt on port 5.6
  P5IES |= ~BIT6;  // sets as rising edge
  P5IFG &= ~BIT6;  // clear interrupt flag


  //TA0CTL = Timer A0 Control
  //TASSEL_2 Timer_A clock source select = 02 SMCLK 1MHz
  //MC_1 Up Mode

  TA0CTL = TASSEL_1 + MC_1;

  //Timer A0 in compare mode
  TA0CCTL0 = 0x0010;

  //initial rate the led blinks
  TA0CCR0 = 10000;




  count = 0;

  //enter LPM4 mode and enable global interrupt
  _BIS_SR(LPM4_bits + GIE);
}


//timer A interrupt
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void) {


    P5IE  |= BIT6; //enables interrupt on port 5.6
    P1OUT ^=BIT0;


}

//timer B interrupt debounce
#pragma vector=TIMER0_B0_VECTOR
__interrupt void Timer_B0 (void) {


    P5IE  |= BIT6; //enables interrupt on port 5.6
    P1OUT ^=BIT1; //green led lights when debounce is done


}




//button interrupt, ta0ccr0 will count from 0 to the final value when button is released
#pragma vector=PORT5_VECTOR
__interrupt void Port_5(void) {


    P5IE  &= ~BIT6; //disables interrupt on port 5.6, stops checking for rising edges


    //Start Timer B for Debounce, will wait 500 ms
    //TB0CTL = Timer B0 Control
    //TBSSEL_1 Timer_B clock source select = 01 ACLk =32khz
    //MC_1 Up Mode

    TB0CTL = TBSSEL_1 + MC_1;

    //Timer A0 in compare mode
    TB0CCTL0 = 0x0010;
    //delay for debounce
    TB0CCR0 = 500;




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
    P5IFG &= ~BIT6;
}


//converts frequency to ticks
double toTicks(double frequency){
    //f=ACLK/ticks
    //f=32k/ticks
    //ticks=32k/f
    return 32000/frequency;
}
