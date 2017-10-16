/*
Matt Mammarelli
9/18/17
ECE 09342-2
*/

//MSP430FR5994 Software PWM Extra Linear Brightness
//green led toggles when button pressed
//red led affected by PWM
//need p1sel0
//The amount of times the button is pressed will be counted.
//Eyes see logarithmically so the increments of the TA0CCR1 will increase exponentially so the light brightness appears to change linearly


#include <msp430.h>

void ledSetup();
void buttonSetup();
void timerSetup();
int btnCount=0;//button press count

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



    _BIS_SR(LPM0_bits + GIE);       //Enter low power mode



}




void ledSetup(){

    //selects gpio mode for both leds
        P1SEL0 &= ~(BIT0|BIT1);


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

        //sets cctl1 and 0 to compare mode
        TA0CCTL1 = (CCIE);
        TA0CCTL0 = (CCIE);

        // PWM period, f=1MHz/1001 = 1khz
        TA0CCR0 = 1000;
        // TA0CCR1 PWM 50% duty cycle
        TA0CCR1 = 500;


}

//button interrupt
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



    if (!((P5IN & BIT6) == BIT6)){




               if (TA0CCR1<=1000){

               //checks button count to see how much the ta0ccr1 should increase
               switch (btnCount){

               case 0:   {
                                  TA0CCR1 = TA0CCR1+30; //increments red led duty cycle
                                  btnCount++;
                                  break;
               }
               case 1:   {
                                  TA0CCR1 = TA0CCR1+40; //increments red led duty cycle
                                  btnCount++;
                                  break;
               }
               case 2:   {
                                  TA0CCR1 = TA0CCR1+40; //increments red led duty cycle
                                  btnCount++;
                                  break;
               }
               case 3:   {
                                  TA0CCR1 = TA0CCR1+40; //increments red led duty cycle
                                  btnCount++;
                                  break;
               }
               case 4:   {
                                  TA0CCR1 = TA0CCR1+50; //increments red led duty cycle
                                  btnCount++;
                                  break;
              }
               case 5:   {
                                  TA0CCR1 = TA0CCR1+50; //increments red led duty cycle
                                  btnCount++;
                                  break;
               }
               case 6:   {
                                  TA0CCR1 = TA0CCR1+100; //increments red led duty cycle
                                  btnCount++;
                                  break;
               }
               case 7:   {
                                  TA0CCR1 = TA0CCR1+160; //increments red led duty cycle
                                  btnCount++;
                                  break;
               }
               case 8:   {
                                  TA0CCR1 = TA0CCR1+220; //increments red led duty cycle
                                  btnCount++;
                                  break;
               }
               case 9:   {
                                  TA0CCR1 = TA0CCR1+270; //increments red led duty cycle
                                  btnCount++;
                                  break;
               }

               default:btnCount=0;



               }




               }
               else {
                   TA0CCR1 = 0; //resets duty cycle back to 0%
               }
           }

           //clears interrupt flag to exit interrupt
           P5IFG &= ~BIT6;
}


//Timer A interrupt vectors
#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer0_A1_ISR (void)
{
    P5IE  |= BIT6; //enables interrupt on port 5.6
    if(TA0CCR1 != 1000)
    {
       P1OUT &= ~(BIT0); //turns off red led
    }
    TA0CCTL1 &= ~BIT0; //clears flag
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0_ISR (void)
{
    P5IE  |= BIT6; //enables interrupt on port 5.6
    P1OUT |= (BIT0); //turns on red led
    TA0CCTL0 &= ~BIT0;  //clears flag
}

//timer B interrupt debounce
#pragma vector=TIMER0_B0_VECTOR
__interrupt void Timer_B0 (void) {


    P5IE  |= BIT6; //enables interrupt on port 5.6
    P1OUT ^=BIT1; //green led lights when debounce is done


}

