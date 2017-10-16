# Lab 4: Software Debouncing
One timer will control the debouncing while the other will control the speed of the red led.
When the button is pressed, the button interrupt fires. 
Then the interrupt flag for the button is disabled to prevent multiple reads.  
A Timer in Up mode is then created to simulate a delay that will wait a small amount of time so mulitiple button inputs aren't detected.
When the interrupt for this timer fires, the button flag is enabled and the green led is toggled.
Then back inside the button interrupt, a variable count checks how many times the button is pressed.
Depending on the number of presses, the speed of the led will change since the TA0CCR0 register is changed. 
The TAR register will count up to the TA0CCR0 register since we chose up mode so the higher the TA0CCR0 register, the longer the blink.

## Code Differences
For each of the boards, different timers were used depending on availablity for each board.

### MSP430F5529
Debounce: Timer B,
LED Speed: Timer A

### MSP430FR2311
Debounce: Timer B1, 
LED Speed: Timer B

### MSP430FR5994
Debounce: Timer B,
LED Speed: Timer A

### MSP430FR6989
Debounce: Timer B,
LED Speed: Timer A

### MSP430G2553
Debounce: Timer A1,
LED Speed: Timer A

## MSP430F5529 


```c
/*
Matt Mammarelli
9/18/17
ECE 09342-2
*/

//MSP430F5529 Software Debouncing
//Timer B used to control the debouncing
//Timer A used to control speed of red LED


#include <msp430.h>

int count; //sets the state of red led

double toTicks(double); //converts frequency to ticks for ccr

void main(void)
{
    //WDTCTL is watchdog timer control
    //WDTPW watchdog timer + password, is an interrupt
    //WDTHOLD watchdog timer +hold, if 1 watchdog timer is stopped
    // Stop watchdog timer
	WDTCTL = WDTPW | WDTHOLD;

	
	 //resistor enabled, p1.1
	 P1REN |= BIT1;

	 //Sets Resistor to pullup, 1.0 is low, has to use = to initialize
	 P1OUT = BIT1;

	 //Enables port 1.0 as output
	 P1DIR = BIT0;
	 //Enable port 4.7 output
	 P4DIR = BIT7;


	 P1IE |=BIT1; //enable interrupt on port 1.1
	 P1IES |=BIT1; //sets as falling edge
	 P1IFG &=~(BIT1);//clear interrupt flag


	   //TA0CTL = Timer A0 Control
	   //TASSEL_1 Timer_A clock source select = 01 ACLK 32k
	   //MC_1 Up Mode

	   TA0CTL = TASSEL_1 + MC_1;

	   //Timer A0 in compare mode
	   TA0CCTL0 = 0x0010;

	   //initial rate the led blinks, 10Hz
	   TA0CCR0 = 10000;



	   count = 0;

	 //enter LPM4 mode and enable global interrupt
	 _BIS_SR(LPM4_bits + GIE);
}

//Port 1 ISR
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TimerA(void) //double __
{


    P1IE  |= BIT1; //enables interrupt on port 5.6
    P1OUT ^=BIT0;


}

//timer B interrupt debounce
#pragma vector=TIMER0_B0_VECTOR
__interrupt void Timer_B0 (void) {


    P1IE  |= BIT1; //enables interrupt on port 5.6
    P4OUT ^=BIT7; //green led lights when debounce is done


}

//button interrupt, ta0ccr0 will count from 0 to the final value when button is released
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {
    P1IE  &= ~BIT1; //disables interrupt on port 5.6, stops checking for rising edges


       //Start Timer B for Debounce, will wait 500 ms
       //TB0CTL = Timer B0 Control
       //TBSSEL_1 Timer_B clock source select = 01 ACLk =32khz
       //MC_1 Up Mode

       TB0CTL = TBSSEL_1 + MC_1;

       //Timer A0 in compare mode
       TB0CCTL0 = 0x0010;
       //delay for debounce
       TB0CCR0 = 200;




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
       P1IFG &= ~BIT1;
}


//converts frequency to ticks
double toTicks(double frequency){
    //f=ACLK/ticks
    //f=32k/ticks
    //ticks=32k/f
    return 32000/frequency;
}



```

## MSP430FR2311 


```c
/*
Matt Mammarelli
9/18/17
ECE 09342-2
*/

//MSP430FR2311 Software Debouncing
//Doesn't see timer A so used Timer B
//Timer B1 used to control the debouncing
//Timer B used to control speed of red LED
//Doesnt work as reliably as others, better than g2


#include <msp430.h>


double toTicks(double); //converts a frequency in Hz to ticks used in ccr register

int count; //sets the state of red led

void main(void) {

    //WDTCTL is watchdog timer control
    //WDTPW watchdog timer + password, is an interrupt
    //WDTHOLD watchdog timer +hold, if 1 watchdog timer is stopped
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    //PM5CTL0 = Power mode 5 control register 0
    //LOCKLPM5 = Locks I/O pin, bit is reset by a power cycle
    //~LOCKLPM5=8'b0 and by anding PM5CTL0 it clears the register
    PM5CTL0 &= ~LOCKLPM5;


        //resistor enabled
        P1REN |= BIT1;

        //Sets Resistor to pullup, 1.0 is low, has to use = to initialize
        P1OUT = BIT1;

        //Enables port 1.0 as output
        P1DIR = BIT0;

        P2DIR = BIT0; //p2.0 green

        P1IE |=BIT1; //enable interrupt on port 1.1
        P1IES |=BIT1; //sets as falling edge
        P1IFG &=~(BIT1);//clear interrupt flag



        //TA0CTL = Timer A0 Control
        //TASSEL_1 Timer_A clock source select = 01 ACLK 32k
        //MC_1 Up Mode

        TB0CTL = TBSSEL_1 + MC_1;

        //Timer A0 in compare mode
        TB0CCTL0 = 0x0010;

        //initial rate the led blinks
        TB0CCR0 = 10000;


         count = 0;

        //enter LPM4 mode and enable global interrupt
        _BIS_SR(LPM4_bits + GIE);


}

//timer b interrupt
#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer_B(void) //double __
{

        P1IE  |= BIT1; //enables interrupt on port 1.1
        P1OUT^=BIT0;


}

//timer A1 interrupt debounce
#pragma vector=TIMER1_B0_VECTOR
__interrupt void Timer_B1 (void) {


    P1IE  |= BIT1; //enables interrupt on port 1.1
    P2OUT ^=BIT0; //green led lights when debounce is done


}


//button interrupt, ta0ccr0 will count from 0 to the final value when button is released
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {

    P1IE  &= ~BIT1; //disables interrupt on port 1.1, stops checking for rising edges


           //Start Timer B for Debounce, will wait 500 ticks
           //TA1CTL = Timer1 Control
           //TASSEL_1 Timer1 clock source select = 01 ACLk =32khz
           //MC_1 Up Mode

           TB1CTL = TBSSEL_1 + MC_1;

           //Timer A0 in compare mode
           TB1CCTL0 = 0x0010;
           //delay for debounce
           TB1CCR0 = 500;




           //changes states of the red led

           count = count+1;

           switch(count){
           case 1:{

               TB0CCR0 = 40000;
               break;
           }
           case 2:{
               TB0CCR0 = 60000;
               break;
           }
           default:{
               TB0CCR0 = 10000;
               count=0;
           }
           }


    //clears interrupt flag to exit interrupt
    P1IFG &= ~BIT1;
}


//converts frequency to ticks
double toTicks(double frequency){
    //f=ACLK/ticks
    //f=32k/ticks
    //ticks=32k/f
    return 32000/frequency;
}



```

## MSP430FR5994 


```c
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


```

## MSP430FR6989 


```c
/*
Matt Mammarelli
9/18/17
ECE 09342-2
*/

//MSP430FR6989 Software Debouncing
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
    WDTCTL = WDTPW | WDTHOLD;

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    //PM5CTL0 = Power mode 5 control register 0
    //LOCKLPM5 = Locks I/O pin, bit is reset by a power cycle
    //~LOCKLPM5=8'b0 and by anding PM5CTL0 it clears the register
    PM5CTL0 &= ~LOCKLPM5;


        //resistor enabled
        P1REN |= BIT1;

        //Sets Resistor to pullup, 1.0 is low, has to use = to initialize
        P1OUT = BIT1;

        //Enables port 1.0 as output, has to use = to initialize
        P1DIR = BIT0;

        P9DIR = BIT7; //green led



        P1IE |=BIT1; //enable interrupt on port 1.1
        P1IES |=BIT1; //sets as falling edge
        P1IFG &=~(BIT1);//clear interrupt flag


        //TA0CTL = Timer A0 Control
          //TASSEL_1 Timer_A clock source select = 01 ACLK 32k
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
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A(void) //double __
{

    P1IE  |= BIT1; //enables interrupt on port 5.6
    P1OUT ^=BIT0;

}


//timer B interrupt debounce
#pragma vector=TIMER0_B0_VECTOR
__interrupt void Timer_B0 (void) {


    P1IE  |= BIT1; //enables interrupt on port 5.6
    P9OUT ^=BIT7; //green led lights when debounce is done


}

//button interrupt, ta0ccr0 will count from 0 to the final value when button is released
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {


    P1IE  &= ~BIT1; //disables interrupt on port 1.1, stops checking for rising edges


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
    P1IFG &= ~BIT1;
}

//converts frequency to ticks
double toTicks(double frequency){
    //f=ACLK/ticks
    //f=32k/ticks
    //ticks=32k/f
    return 32000/frequency;
}



```

## MSP430G2553 


```c
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


```

