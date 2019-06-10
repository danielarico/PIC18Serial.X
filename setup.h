/* 
 * File:   setup.h
 * Author: daniela
 *
 * Created on June 10, 2019, 10:31 AM
 */

//*************************************************
// Macros definition
//*************************************************
#define LED1 LATBbits.LATB0
#define LED2 LATBbits.LATB1
#define interr LATBbits.LATB2

//*************************************************
// Global variables declaration
//*************************************************
const uint16_t timer0_start = 3036; // For prescaler 1:16

//*************************************************
// Functions' declaration
//*************************************************
void clock_config();
void interr_config();
void ports_config();
void comparator_config();
void serial_config();
void timer0_config();

//*************************************************
// Functions' definition
//*************************************************
void clock_config ()
{
    OSCCONbits.IRCF = 7; // Internal Oscillator Frequency (8 MHz)
}

void interr_config ()
{
    RCONbits.IPEN = 1; // Interrupt priority enable
    INTCONbits.PEIE = 1; // Enable low priority interrupts
    INTCONbits.GIE = 1; // Enable high priority interrupts
}

void ports_config()
{
    TRISB = 0; // Output
    
    LED1 = 0; // Initialize in 0
    LED2 = 0; // Initialize in 0
    interr = 0;
}

void comparator_config ()
{
    // 4 inputs multiplexed to 2 comparators (CM2:CM0 = 110)
    CMCONbits.CM2 = 1;
    CMCONbits.CM1 = 1;
    CMCONbits.CM0 = 0;
    //-----------------------

    // For CVref = 2.5V (CVR3:CVR0 = 1100 = 12)
    CVRCONbits.CVR3 = 1;
    CVRCONbits.CVR2 = 1;
    CVRCONbits.CVR1 = 0;
    CVRCONbits.CVR0 = 0;
    //-----------------------
    
    CMCONbits.CIS = 0; // C1 Vin- connects to RA0; C2 Vin- to RA1
    PIE2bits.CMIE = 1; // Enables comparator interrupts

    CVRCONbits.CVREN = 1; // Comparator Voltage Reference enabled
    CVRCONbits.CVRR = 1; // Comparator Vref range selection (low range)
    CVRCONbits.CVRSS = 0; // Comparator ref source CVrsrc = Vdd - Vss

    CVRCONbits.CVROE = 1; // CVref voltage is also output on the RA2 pin
}

void serial_config ()
{
	// To set up an Asynchronous Transmission: Datasheet p. 247
 	// To set up an Asynchronous Reception: Datasheet p. 250
    
	TXSTAbits.BRGH = 0; // Low speed transmission
	BAUDCONbits.BRG16 = 0; // 8-bit Baud Rate Generator (SPBRG only)
	TXSTAbits.SYNC = 0; // EUSART Mode: Asynchronous mode
	RCSTAbits.SPEN = 1; // Serial port enabled
    SPBRG = 12; // To 9600 baud rate approximately BR = 8MHz/(64*(12+1))
    
 	// Transmission ------------
	BAUDCONbits.TXCKP = 0; // TX Data is inverted
	PIE1bits.TXIE = 0; // Disables EUSART Transmit Interrupt
	TXSTAbits.TX9 = 0; // Selects 8-bit transmission
	TXSTAbits.TXEN = 1; // Transmit disabled
	IPR1bits.TXIP = 1; // High priority
    
	// Reception -------------
	RCSTAbits.RX9 = 0; // Selects 8-bit reception
	PIE1bits.RCIE = 1; // Enables EUSART Receive Interrupt
	IPR1bits.RCIP = 1; // High priority
    RCSTAbits.CREN = 1; // Enables reception
}

void timer0_config ()
{
	T0CONbits.T08BIT = 0; // 16 bits
	T0CONbits.T0CS = 0; // Internal instruction cycle clock (8 MHz)
	T0CONbits.T0SE = 0; // Increment in rising edge (low to high)
	T0CONbits.PSA = 0; // Timer0 prescaler is assigned
	T0CONbits.T0PS = 3; // Select prescaler (1:256)
	INTCONbits.TMR0IF = 0; // Disable timer0 overflow flag
	INTCON2bits.TMR0IP = 1; // High priority timer0 interrupt
	TMR0 = timer0_start; // To count 0.5s
	T0CONbits.TMR0ON = 1; // Start timer
	INTCONbits.TMR0IE = 1; // Enable timer0 interrupts
}


