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
#include "setup.h"

//*************************************************
// Macros definition
//*************************************************


//*************************************************
// Global variables declaration
//*************************************************
bool timer0_flag = false;
bool tx_finish = false;
bool rx_finish = false;

const uint8_t array_size = 20;

char rx_buffer[array_size] = {};
char rx_char = '\0';
char rx_data[array_size] = {};
char * ptr_rx = rx_buffer;
char * ptr_tx;

int delay = 30000;

//*************************************************
// Functions' declaration
//*************************************************
void interrupt high_priority isr_high(void);
void interrupt low_priority isr_low(void);

void send(char * ptr_array);
void send_next();
void received();
void clean (char * ptr);
void read ();

void device_wakeup ();

//*************************************************
// Main
//*************************************************
void main(void)
{
    clock_config();
    interr_config();
    ports_config();
    comparator_config();
	serial_config();
    
	char data [array_size] = "HOLA MUNDO\n";
    SLEEP();
    
    while(1)
    {  
        LED2 = 1; // Debug
        for(int i=0; i<delay; i++) {} // Debug
          
        device_wakeup(); // Wake up signal for LoRa chip

        read(); // Waits for '\n' character
        
        LED2 = 0;
        for(int i=0; i<delay; i++) {}
        LED2 = 1;
        for(int i=0; i<delay; i++) {}
        
        send(rx_buffer);
        
        while(!tx_finish) {}

        LED2 = 0;
        for(int i=0; i<delay; i++) {} 
        
        SLEEP();
    }
	return;
}

//*************************************************
// Functions' definition
//*************************************************

void interrupt high_priority isr_high (void) // Interrupt service routine high priority
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
    
	if (PIR1bits.RCIF && PIE1bits.RCIE) // Reception interruption
	{
        rx_char = RCREG;
        if(rx_char == '\0') return;
        *ptr_rx = rx_char;
        ptr_rx ++;
        rx_finish = false;

        if(rx_char == '\n')
        {
            received();
        }
	}
    
    if (PIE2bits.CMIE && PIR2bits.CMIF) // Comparator interruption
	{
        PIR2bits.CMIF = 0;
    	LED1 = C1OUT;
    }
}

void send (char * ptr_array)
{
    ptr_tx = ptr_array;
	tx_finish = false; // End of transmission flag
	PIE1bits.TXIE = 1; // Enables EUSART Transmit Interrupt
}

void send_next()
{  
	if(*ptr_tx == '\0')
	{
    	tx_finish = true;
    	PIE1bits.TXIE = 0; // Disables EUSART Transmit Interrupt when find null char
	}
	else
	{
    	TXREG = *ptr_tx;
    	ptr_tx ++;
	}
}

void read ()
{
    ptr_rx = rx_buffer; // Re-initialize pointer
    rx_finish = false; // End of reception flag
    while(!rx_finish) {}
}

void received ()
{
    rx_finish = true; // End of reception flag
    ptr_rx = rx_buffer; // Re-initialize pointer
}

void clean (char * ptr)
{
    for (int i=0; i<sizeof(ptr); i++)
    {
        *ptr = '\0';
        ptr ++;
    }
}

void device_wakeup()
{
    interr = 1;
    for(int i=0; i<800; i++) {}
    interr = 0;
}