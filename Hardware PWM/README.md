# Lab 4: Hardware PWM
Similar to the Software PWM but uses the TA0CCTL1 = (OUTMOD_7) hardware and removed the timer isrs. The led setup for each of these boards is also different.

## Code Differences
For each of the boards, different timers were used depending on availablity for each board. Input buttons and led pinouts were also different.
The use of P1SEL vs P1SEL0 changed depending on the board.

### MSP430F5529
Use P1SEL;
Use TIMER A;

### MSP430FR2311
Use P1SEL0;
Use TIMER B;


### MSP430FR5994
Use P1SEL0;
Use TIMER A;


### MSP430FR6989
Use P1SEL0;
Use TIMER A;


### MSP430G2553
USE P1SEL;
Use TIMER A;



## MSP430F5529 


```c
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




```

## MSP430FR2311 


```c
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
                TB0CCR1 = TB0CCR1+100; //increments green led duty cycle

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

```

## MSP430FR5994 


```c
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
               P1OUT^=BIT1; //toggle green led

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


```

## MSP430FR6989 


```c
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
            TA0CCR1 = TA0CCR1+100; //increments green led duty cycle

        }
        else {
            TA0CCR1 = 0; //resets duty cycle back to 0%
        }
    }

    //clears interrupt flag to exit interrupt
    P1IFG &= ~BIT1;
}



```

## MSP430G2553 


```c
/*
Matt Mammarelli
9/18/17
ECE 09342-2
*/

//MSP430G2553 Hardware PWM
//green led toggles when button pressed
//red led affected by PWM
//need p1sel, changed ledSetup()
//changed timerSetup() TA0CCTL1 = (OUTMOD_7);, removed Timer ISRS

#include <msp430.h>

void ledSetup();
void buttonSetup();
void timerSetup();


void main(void){

    // Stop watchdog timer
    WDTCTL = WDTPW + WDTHOLD;

    ledSetup(); //init leds
    buttonSetup(); //init buttons
    timerSetup(); //init timer

    _BIS_SR(LPM0_bits + GIE);       // Has to be LMP0 for G2

}

void ledSetup(){



    //selects peripheral mode for red led
          P1SEL |= (BIT0);
          //non pwm bits cleared
          P1SEL &= ~(BIT6);
          //button multiplexor disabled
          P1SEL &= ~BIT3;
          //red led out
          P1DIR |= (BIT0);

          //green led out
          P1DIR |= (BIT6);

          //makes sure green led is off
          P1OUT &= ~(BIT6);


}
void buttonSetup(){

    //resistor enabled, input 1.3
    P1REN |= BIT3;

    //Sets Resistor to pullup, 1.0 is low, has to use = to initialize
    P1OUT = BIT3;


    P1IE |=BIT3; //enable interrupt on port 1.3
    P1IES |=BIT3; //sets as falling edge
    P1IFG &=~(BIT3);//clear interrupt flag

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

    if (!((P1IN & BIT3) == BIT3)){
        P1OUT^=BIT6; //toggle green led

        if (TA0CCR1<=1000){
            TA0CCR1 = TA0CCR1+100; //increments green led duty cycle

        }
        else {
            TA0CCR1 = 0; //resets duty cycle back to 0%
        }
    }

    //clears interrupt flag to exit interrupt
    P1IFG &= ~BIT3;
}




```

