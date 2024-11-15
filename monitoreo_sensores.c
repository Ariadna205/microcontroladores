#include <xc.h>
#include "monitoreo_sensores.h" 
#include <pic16f887.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "LCD.h"
#include "KEYPAD.h"
#define _XTAL_FREQ 8000000


void sensores_init(void) {
    
    OSCCON = 0x71; 
    ANSELH = 0x00;
    TRISD = 0x00;   
    TRISC = 0x00;   
    TRISA0 = 1;     
    TRISA1 = 1;     
    PORTD = 0; 
    ANSELbits.ANS1 = 0;  
    ANSELbits.ANS2 = 0;  
    TRISA1 = 1;          
    TRISA2 = 1; 
    TRISA3 = 1;
}

int read_hall(void) {
    if (PORTAbits.RA3 == 1) { 
        __delay_us(30);
        return 1;  
    } else {
        return 0;  
    }
}

int read_movimiento(void) {
    if (PORTDbits.RD2 == 1) { 
        __delay_us(30);
        return 1;  
    } else {
        return 0;  
    }
}