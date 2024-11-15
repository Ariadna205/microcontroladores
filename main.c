#include <xc.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "LCD.h"
#include "KEYPAD.h"
#include "ADC.h"
#include <math.h>
#include "monitoreo_sensores.h"
#include <pic16f887.h>
#define _XTAL_FREQ 8000000
#define LED_ROJO   PORTDbits.RD7
#define LED_AZUL   PORTDbits.RD6
#define LED_VERDE  PORTDbits.RD5
#define BUZZER_PIN PORTDbits.RD4

    

typedef enum {
    inicio_sistema,
    sistema_seguridad,
    sistema_bloqueado,
    sistema_monitoreo_ambiental,
    sistema_monitoreo_puertas,
    sistema_alarma,
    sistema_alerta
} Estado;

Estado estado_actual = inicio_sistema;


const char password[5] = "12345";  
char pass_user[8];       
unsigned char ingresos = 0;
int intentos = 0;
volatile unsigned int ms_counter = 0;  
unsigned int timeout_ms = 0;    

int temp_celsius;
int intensidad_luz;
int sensor_hall;
int valor_evento;

void tiempo_init(void);
void __interrupt() ISR(void);
void funcion_inicio(void);
void funcion_seguridad(void);
void funcion_bloqueado(void);
void funcion_monitoreo_ambiental(void);
void funcion_monitoreo_puertas(void);
void funcion_alarma(void);
void funcion_alerta(void);

void main() {
    OSCCON = 0x71; 
    ANSELH = 0x00;
    TRISD = 0x00;   
    TRISC = 0x00;   
    TRISA0 = 1;     
    TRISA1 = 1;
    TRISA2 = 1;
    TRISA3 = 1;
    PORTD = 0; 
    ANSELbits.ANS1 = 0;  
    ANSELbits.ANS2 = 0;  
    
   
    LCD_Init();
    adc_init();
    keypad_Init();
    sensores_init();
    read_hall();
    read_movimiento();
    read_temperatura();
    read_fotoresistor();  
    
 
    while (1) {
        switch (estado_actual) {
            case inicio_sistema:
                funcion_inicio();
                break;
            case sistema_seguridad:
                funcion_seguridad();
                break;
            case sistema_bloqueado:
                funcion_bloqueado();
                break;
            case sistema_monitoreo_ambiental:
                funcion_monitoreo_ambiental();
                break;
            case sistema_monitoreo_puertas:
                funcion_monitoreo_puertas();
                break;
            case sistema_alarma:
                funcion_alarma();
                break;
            case sistema_alerta:
                funcion_alerta();
                break;
        }
    }
}


//void tiempo_init(void) {
  //  OPTION_REGbits.T0CS = 0;
    //OPTION_REGbits.PSA = 0;
    //OPTION_REGbits.PS = 0b111;
    //TMR0 = 256 - (8000000 / 4 / 1000 / 256);
    //INTCONbits.TMR0IE = 1;
    //INTCONbits.TMR0IF = 0;
    //INTCONbits.GIE = 1;
//}


void __interrupt() ISR(void) {
    if (INTCONbits.TMR0IF) {
        TMR0 = 256 - (8000000 / 4 / 1000 / 256);
        ms_counter++;
        INTCONbits.TMR0IF = 0;
    }
}

void funcion_inicio(void) {
    LCD_Init(); 
    LCD_String_xy(0, 0, "BIENVENIDO");
    __delay_ms(2000);
    LCD_Clear();
    LCD_String("Seguridad");
    LED_VERDE = 0;
    LED_ROJO = 0;
    LED_AZUL = 0;

    estado_actual = sistema_seguridad;
    ms_counter = 0;
}


void funcion_seguridad(void) {
    char key = '0';  
    int intentos = 0;
    

    if (intentos != 0) {
        LCD_Clear();
        LCD_String_xy(0, 0, "Ingresar Clave:");
        LCD_Command(0xC0);

        timeout_ms = 100;
        do {
            key = keypad_getkey();
            if (key != 0) {
                LCD_Char('*');
                pass_user[ingresos++] = key;
            }

            while (ms_counter < timeout_ms);
            ms_counter = 0;
        } while (ingresos < 4);
        pass_user[ingresos] = '\0';

        if (strncmp(pass_user, password, 4) == 0) {
            LCD_Clear();
            LCD_String_xy(0, 0, "Clave Correcta");

            for (int i = 0; i < 3; i++) {
                LED_VERDE = 1;
                timeout_ms = 500;
                while (ms_counter < timeout_ms);
                ms_counter = 0;

                LED_VERDE = 0;
                timeout_ms = 500;
                while (ms_counter < timeout_ms);
                ms_counter = 0;
            }

            intentos = 3;
            estado_actual = sistema_monitoreo_ambiental;

        } else {
            LCD_Clear();
            LCD_String_xy(0, 0, "CLAVE INCORRECTA");
            LCD_Command(0xC0);
            for (int i = 0; i < 2; i++) {
                LED_AZUL = 1;
                timeout_ms = 300;
                while (ms_counter < timeout_ms);
                ms_counter = 0;

                LED_AZUL = 0;
                timeout_ms = 700;
                while (ms_counter < timeout_ms);
                ms_counter = 0;
            }

            intentos--;
        }
    } else {
        estado_actual = sistema_bloqueado;
    }
    timeout_ms = 2000;
    while (ms_counter < timeout_ms);
    ms_counter = 0;
    LCD_Clear();
}


void funcion_bloqueado(void) {
    LCD_Clear();
    LCD_String("SISTEMA BLOQUEADO");
    LED_ROJO = 1;
    BUZZER_PIN = 1;

    timeout_ms = 10000;
    while (ms_counter < timeout_ms);
    ms_counter = 0;

    estado_actual = sistema_seguridad;
    intentos = 3;
    LED_ROJO = 0;
    BUZZER_PIN = 0;
}

void funcion_monitoreo_ambiental(void) {
    temp_celsius = read_temperatura();
    intensidad_luz = read_fotoresistor();

    LCD_Clear();
    char buffer[17];
    sprintf(buffer, "T: %dC L: %dLx", temp_celsius, intensidad_luz);
    LCD_String_xy(0, 0, buffer);

    LED_VERDE = 1;
    timeout_ms = 5000;

    if (temp_celsius > 30 || intensidad_luz > 800) {
        estado_actual = sistema_alarma;
        ms_counter = 0;
        return;
    }

    if (ms_counter >= timeout_ms) {
        estado_actual = sistema_monitoreo_puertas;
        ms_counter = 0;
    }
}

void funcion_monitoreo_puertas(void) {
    sensor_hall = read_hall();
    valor_evento = read_movimiento();

    LCD_Clear();
    char buffer[17];
    sprintf(buffer, "H: %d O: %d", sensor_hall, valor_evento);
    LCD_String_xy(0, 0, buffer);

    timeout_ms = 5000;

    if (sensor_hall > 0) {
        estado_actual = sistema_alerta;
        ms_counter = 0;
    } else if (ms_counter >= timeout_ms) {
        estado_actual = sistema_monitoreo_ambiental;
        ms_counter = 0;
    }
}


void funcion_alarma(void) {
    LED_AZUL = 1;
    LCD_Clear();
    LCD_String_xy(0, 0, "ALARMAAA");
    LCD_Command(0xC0);
    timeout_ms = 5000;

    if (ms_counter >= timeout_ms) {
        estado_actual = sistema_monitoreo_ambiental;
        ms_counter = 0;
        LED_AZUL = 0;
    }
}


void funcion_alerta(void) {
    LED_ROJO = 1;
    BUZZER_PIN = 1;
    LCD_Clear();
    LCD_String_xy(0, 0, "ALERTA");
    LCD_Command(0xC0);
    timeout_ms = 5000;

    if (ms_counter >= timeout_ms) {
        estado_actual = sistema_monitoreo_ambiental;
        ms_counter = 0;
        LED_ROJO = 0;
        BUZZER_PIN = 0;
    }
}