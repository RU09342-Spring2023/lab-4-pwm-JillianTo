#include <msp430.h> 


/**
 * main.c
 * 
 * Created on: Feb 22, 2023
 * Author: Jillian To
 * 
 */

#define RG 0
#define GB 1
#define RB 2

char state = RG;
unsigned short period = 1000;

void ledInit();
void timerB3Init();
void setRDC(unsigned short dutyCycle);
void setGDC(unsigned short dutyCycle);
void setBDC(unsigned short dutyCycle);

int main(void)
{

    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer

	ledInit();
	timerB3Init();

	PM5CTL0 &= ~LOCKLPM5; // disable the GPIO power-on default high-impedance mode to activate previously configured port settings

	TB3CCR1 = 0; // start red on
	TB3CCR2 = 1000; // start green off
	TB3CCR3 = 1000; // start blue off

	while(1) {
	    switch(state) {
	    case RG: // decrease red, increase green
	        while(TB3CCR1 < period) { // while red less than lowest duty cycle, where higher CCR value is less on time
	            TB3CCR1++; // decrease red, increment CCR1 to decrease on time
	            TB3CCR2--; // increase green, decrement CCR2 to increase on time
	            __delay_cycles(3000); // value determines time to change color
	        }
	        state = GB; // change to green + blue state
	        break;
	    case GB: // decrease green, increase blue
            while(TB3CCR2 < period) { // while green less than lowest duty cycle
                TB3CCR2++; // decrease green
                TB3CCR3--; // increase blue
                __delay_cycles(3000); // color change time
            }
            state = RB; // change to red + blue state
            break;
	    case RB: // decrease blue, increase red
            while(TB3CCR3 < period) { // while blue less than lowest duty cycle
                TB3CCR3++; // decrease blue
                TB3CCR1--; // increase red
                __delay_cycles(3000); // color change time
            }
            state = RG; // change state to red + green
            break;
        }
	}

}

void ledInit() {
	P6OUT &= ~(BIT0+BIT1+BIT2); // reset P6.0, P6.1, and P6.2
	P6DIR |= (BIT0+BIT1+BIT2); // set P6.0, P6.1, and P6.2 to input
	P6SEL0 |= (BIT0+BIT1+BIT2); // PWM for P6.0, P6.1, and P6.2
	P6SEL1 &= ~(BIT0+BIT1+BIT2); // PWM for P6.0, P6.1, and P6.2
}

void timerB3Init() {
    TB3CCR0 = period-1;
    TB3CTL = TBSSEL__SMCLK | MC__UP | TBCLR; // SMCLK, up mode, clear TBR
    TB3CCTL1 = OUTMOD_7; // CCR1 reset/set
    TB3CCTL2 = OUTMOD_7; // CCR2 reset/set
    TB3CCTL3 = OUTMOD_7; // CCR3 reset/set
}

