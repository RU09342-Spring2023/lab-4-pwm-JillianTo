#include <msp430.h> 


/**
 * main.c
 */

char CW = 0;
char CCW = 0;

void btnInit();
void servoInit();
void timerInit();

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	btnInit();
	servoInit();
	timerInit();

    PM5CTL0 &= ~LOCKLPM5; // disable GPIO power-on default high-impedance mode to activate previously configured port settings

    __bis_SR_register(LPM3_bits | GIE); // enter LPM3, enable interrupts
    __no_operation(); // for debugger

}

void btnInit() {

    P2DIR &= ~BIT3; // set P2.3 to input
    P2REN |= BIT3; // enable P2.3 resistor
    P2OUT |= BIT3; // set P2.3 resistor to pull-up
    P2IES |= BIT3; // P2.3 High -> Low edge
    P2IE |= BIT3; // P2.3 interrupt enable
    P2IFG &= ~BIT3; // clear P2.3 interrupt flag

    P4DIR &= ~BIT1; // set P4.1 to input
    P4REN |= BIT1; // enable P4.1 resistor
    P4OUT |= BIT1; // set P4.1 resistor to pull-up
    P4IES |= BIT1; // P4.1 High -> Low edge
    P4IE |= BIT1; // P4.1 interrupt mode
    P4IFG &= ~BIT1; // clear P4.1 interrupt flag

}

void servoInit() {
    P6OUT &= ~BIT3; // reset P6.3 output
    P6DIR |= BIT3; // set P6.3 to output
    P6SEL0 |= BIT3; // PWM mode for P6.3
    P6SEL1 &= ~BIT3; // PWM mode for P6.3
}

void timerInit() {
    TB3CCR0 = 23260-1; // PWM period
    TB3CTL = TBSSEL_2 | MC_1; // SMCLK, up mode
    TB3CCTL4 = OUTMOD_7; // CCR4 reset/set
}

// port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void) {
    P2IFG &= ~BIT3; // clear P2.3 IFG
    //P2IES ^= BIT3; // switch edge type
    for(TB3CCR4 = 350; TB3CCR4 < 2600; TB3CCR4++) { // rotate servo 190 degrees CW
        _delay_cycles(500); // speed of rotation
    }
}

// port 4 interrupt service routine
#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void) {
    P4IFG &= ~BIT1; // clear P2.3 IFG
    //P4IES ^= BIT1; // switch edge type
    for(TB3CCR4 = 2600; TB3CCR4 > 500; TB3CCR4--) { // rotate servo CCW, less than 180 degrees because the motor gets stuck
        _delay_cycles(500); // speed of rotation
    }
}
