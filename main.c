/*
 * File:   main.c
 * Author: daniela
 *
 * Created on May 14, 2019, 6:17 PM
 */

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "config.h"

//*************************************************
// Global variables declaration
//*************************************************
bool timer0_flag = false;
bool tx_finish = false;
const unsigned uint16_t timer0_start = 3036; // For prescaler 1:16
const uint8_t array_size = 20;
char tx_buffer[array_size];
char rx_buffer[array_size];
char * ptr_tx_buffer = tx_buffer;
char * ptr_rx_buffer = rx_buffer;

//*************************************************
// Functions' declaration
//*************************************************
void interrupt high_priority isr_high(void);
void interrupt low_priority isr_low(void);
void serial_config ();
void init_timer0(void);
void send(char * ptr_array);
void send_next();
void read();

//*************************************************
// Main
//*************************************************
void main(void)
{
	OSCCONbits.IRCF = 7; // Internal Oscillator Frequency (8 MHz)
    
	RCONbits.IPEN = 1; // Interrupt priority enable
	INTCONbits.PEIE = 1; // Enable low priority interrupts
	INTCONbits.GIE = 1; // Enable high priority interrupts
    
	// Ports configuration --------------
	TRISAbits.TRISA0 = 0; // Output
	LATAbits.LATA0 = 0; // Initialize in 0

	serial_config();
    
    
	char data [array_size] = "HOLA MUNDO";
	char arr [array_size] = "HELLO WORLD";
    
	send(data);
	while(!tx_finish) {}

	send(arr);
    
	while(!tx_finish) {}
    
    
	while(1) {}
    
	return;
}

//*************************************************
// Functions' definition
//*************************************************

void interrupt high_priority isr_high(void) // Interrupt service routine high priority
{
	if (INTCONbits.TMR0IF && INTCONbits.TMR0IE) // Timer0 interruption
	{
    	INTCONbits.TMR0IF = 0;
    	TMR0 = timer0_start;
    	timer0_flag = true;
	}
    
	if (PIR1bits.TXIF && PIE1bits.TXIE) // Transmission interruption
	{
    	send_next();
	}
    
    /*
	if (PIR1bits.RCIF && PIE1bits.RCIE) // Transmission interruption
	{
    	read();
	}*/
}

void init_timer0 (void)
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

void serial_config ()
{
	/* To set up an Asynchronous Transmission:
 	*
 	* 1.Initialize the SPBRGH:SPBRG registers for the appropriate baud rate. Set or
 	* clear the BRGH and BRG16 bits, as required, to achieve the desired baud rate.
 	*
 	* 2. Enable the asynchronous serial port by clearing bit SYNC and setting bit SPEN.
 	*
 	* 3. If the signal from the TX pin is to be inverted, set the TXCKP bit.
 	*
 	* 4. If interrupts are desired, set enable bit TXIE.
 	*
 	* 5. If 9-bit transmission is desired, set transmit bit TX9. Can be used as address/data bit.
 	*
 	* 6. Enable the transmission by setting bit TXEN which will also set bit TXIF.
 	*
 	* 7. If 9-bit transmission is selected, the ninth bit should be loaded in bit TX9D.
 	*
 	* 8. Load data to the TXREG register (starts transmission).
 	*
 	* 9. If using interrupts, ensure that the GIE and PEIE bits in the INTCON register (INTCON<7:6>) are set.
 	*/
	TXSTAbits.BRGH = 0; // Low speed transmission
	BAUDCONbits.BRG16 = 0; // 8-bit Baud Rate Generator (SPBRG only)
	SPBRG = 12; // To 9600 baud rate approximately BR = 8MHz/(64*(12+1))
	TXSTAbits.SYNC = 0; // EUSART Mode: Asynchronous mode
	RCSTAbits.SPEN = 1; // Serial port enabled
    
 	// Transmission ------------
	BAUDCONbits.TXCKP = 0; // TX Data is inverted
	PIE1bits.TXIE = 0; // Disables EUSART Transmit Interrupt
	TXSTAbits.TX9 = 0; // Selects 8-bit transmission
	TXSTAbits.TXEN = 1; // Transmit enabled
	IPR1bits.TXIP = 1; // High priority
    
	// Reception -------------
	RCSTAbits.RX9 = 0; // Selects 8-bit reception
	PIE1bits.RCIE = 1; // Enables EUSART Receive Interrupt
	RCSTAbits.CREN = 1; // Enables reception
	BAUDCONbits.RCIDL = 1; // Receive operation is idle (0=active)
	IPR1bits.RCIP = 1; // High priority
    
}

void send (char * ptr_array)
{
	tx_finish = 0;
	char add[2] = "\n\0";
	strcat(ptr_array, add);
	ptr_tx_buffer = ptr_array;
	PIE1bits.TXIE = 1; // Enables EUSART Transmit Interrupt
}

void send_next()
{  
	if(*ptr_tx_buffer == '\0')
	{
    	tx_finish = 1;
    	PIE1bits.TXIE = 0; // Disables EUSART Transmit Interrupt when find null char
	}
	else
	{
    	TXREG = *ptr_tx_buffer;
    	ptr_tx_buffer++;
	}
}

/*
void read()
{
	rx_buffer[0] = RCREG;
	send(rx_buffer);
}*/
