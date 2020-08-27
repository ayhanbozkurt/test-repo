#include <msp430.h> 
#include "isr.h"

const unsigned char LookUp[16]={"0123456789ABCDEF"};

int main(void)
{
    int k;
    unsigned char lbyte, hbyte, tbyte;

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    //_BIS_SR(LPM4_bits + GIE);
    //SR |= GIE;
    //__enable_interrupt();
    // __bis_SR_register(GIE);

    P3SEL |= 0x30;                             // P3.4,5 = USCI_A0 TXD/RXD
    UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 228;                              // 1MHz 115200 (see User's Guide)
    UCA0BR1 = 0;                              // 1MHz 115200
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**

    UCA0TXBUF = 'R';

    TA0CCTL0 = CCIE;
    //TA0CTL &= ~TAIFG;
    TA0CTL = TASSEL_2 + ID_3 + MC_2 + TACLR + TAIE;           // SMCLK, 1/8, continuous mode, , enable interrupts
    TA0CCR0 = 50000;

    k=0;
    while(k<3) {
        if (j==1){
            k++;
            j=0;
        }
    }


    //P9MAP1 = PM_UCB2SDA;

    UCB2CTL1 |= UCSWRST;              // **Put state machine in reset**
    P9SEL |= 0x06;          // P9.1 UCB2SDA, P9.2 UCB2SCL
    //P9DIR |= 0x06;
    UCB2CTL0 |= (UCMST + UCMODE_3 + UCSYNC);    // Master Mode | I2C mode
    //UCB2CTL0 |= (UCMODE_3);
    //UCB2CTL0 |= (UCMST);
    UCB2CTL1 |= (UCSSEL_3);             // SMCLK
    UCB2I2CSA = 0x004A; // 0x4A:TMP101-1  0x48:TMP101-2
    UCB2BR0 = 250;
    UCB2BR1 = 2;                      // 25MHz/250=100kHz
    UCB2CTL1 &= ~(UCSWRST);             // **Initialize USCI state machine**    */

    P8SEL &= 0xE7;  // P8 bits 3 and 4 digital I/O
    P8DIR |= 0x18;  // P8 bits 3 and 4 OUPUT
    //P8OUT |= (BIT3 + BIT4);
    P8OUT |= BIT3;
    P8OUT &= ~BIT4;
    //P8OUT |= BIT4;

    k=0;

    while(1){
        if (j==1){
            P8OUT ^= BIT4;
            j=0;

            if (k==0){
                UCB2I2CSA = 0x004A;
                k=1;
            }
            else{
                UCB2I2CSA = 0x0048;
                k=0;
            }

            UCB2IFG &= ~UCTXIFG;
            //UCB2CTL1 |= UCTR;
            //UCB2CTL1 |= UCTXSTT;


            UCB2CTL1 |= (UCTR + UCTXSTT);

            //while(!(UCB2IFG&UCTXIFG));
            UCB2TXBUF = 0;

            //while(UCB2CTL1&UCTXSTT);
            //P8OUT &= ~BIT3;

            //UCB2TXBUF = 0;  // select register 0
            while(!(UCB2IFG & UCTXIFG));

            P8OUT &= ~BIT3;

            UCB2IFG &= ~UCRXIFG;
            UCB2CTL1 &= ~UCTR; // read mode
            UCB2CTL1 |= UCTXSTT; // re-start
            //while(UCB2CTL1&UCTXSTT);
            while(!(UCB2IFG&UCRXIFG));
            UCB2CTL1 |= UCTXSTP;
            hbyte = UCB2RXBUF;
            while(!(UCB2IFG&UCRXIFG));
            //UCB2CTL1 |= UCTXSTP;
            lbyte = UCB2RXBUF;
            //tbyte = UCB2RXBUF;

          //  UCA0IFG &= ~UCTXIFG;
 //           UCA0TXBUF = '0';
 //           while(!(UCA0IFG&UCTXIFG));
 //           UCA0TXBUF = 'x';
 //           while(!(UCA0IFG&UCTXIFG));
            tbyte = hbyte/10;
            UCA0TXBUF = LookUp[tbyte];
            while(!(UCA0IFG&UCTXIFG));
            tbyte = hbyte%10;
            UCA0TXBUF = LookUp[tbyte];
            while(!(UCA0IFG&UCTXIFG));
            UCA0TXBUF = '.';
            while(!(UCA0IFG&UCTXIFG));
            //tbyte = (lbyte>>5+lbyte>>7);
            tbyte = 5*(lbyte>>7);
            UCA0TXBUF = LookUp[tbyte];
            while(!(UCA0IFG&UCTXIFG));
            //UCA0TXBUF = (k) ? ' ' : 10 ;
            if (k==1){
                UCA0TXBUF = ' ';
            }
            else{
                UCA0TXBUF = 13;
                while(!(UCA0IFG&UCTXIFG));
                UCA0TXBUF = 10;
            }

        }
    }

}

