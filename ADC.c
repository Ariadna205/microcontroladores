#include "ADC.h"
#include <xc.h>
#include <pic16f887.h>
#include "config.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "LCD.h"
#include "KEYPAD.h"
#define _XTAL_FREQ 8000000 

void adc_init(void){
    
    ADCON1bits.ADFM = 1; 
    ADCON1bits.VCFG0 = 0; 
    ADCON1bits.VCFG1 = 0;
    ADCON0bits.ADCS = 0b01; 
    ADRESH=0;		
    ADRESL=0;   
}


int adc_read(int channel){
    
    ADCON0bits.CHS = (0x0F & channel); //  Selecciona el Canal Analógico AN0.
    ADCON0bits.ADON = 1; //  Habilita el Módulo AD.
    __delay_us(30);
    ADCON0bits.GO_DONE = 1; //  Inicia la COnversió AD.
    while (ADCON0bits.GO_DONE); //  Espera a que termine la conversión AD.
    int value_adc = ADRESH; //  Lectura de valor AD.
    value_adc = (value_adc << 8) + ADRESL;
    return value_adc;
}

int read_temperatura(void) {
    int temperatura = adc_read(0);  
    int valor_adc = 1023 - (int)temperatura; 
    float celsius = (float)(valor_adc * 0.04058);  
    return (int)celsius;  
}


int read_fotoresistor(void) {
    int luz = adc_read(1);  
    int luzPorcentaje = (luz * 100) / 1023;  
    return luzPorcentaje;  
}

