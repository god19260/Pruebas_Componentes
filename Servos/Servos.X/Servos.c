/*
 * File:   Servos.c
 * Author: Fredy 
 *
 */


#include <xc.h>
#define _XTAL_FREQ 8000000
// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_CLKOUT// Oscillator Selection bits (RC oscillator: 
                // CLKOUT function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR 
                                // pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code 
                                // protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code 
                                // protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR enabled)
#pragma config IESO = OFF       // Internal External Switchover bit 
                                //(Internal/External Switchover mode is enabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit 
                                // (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = OFF         // Low Voltage Programming Enable bit (RB3/PGM 
                       // pin has PGM function, low voltage programming enabled)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out 
                                // Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable 
                                // bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

//------------------------------------------------------------------------------
//********************* Declaraciones de variables *****************************
unsigned char Valor_TMR0 = 100;
unsigned char Contador_Servos;
unsigned char Valor_Servo_1;
unsigned char Valor_Servo_2;
unsigned char Valor_Servo_3;
unsigned char ADRESH_Servo_1;
unsigned char ADRESH_Servo_2;
unsigned char ADRESH_Servo_3;
//------------------------------------------------------------------------------
//***************************** Prototipos *************************************

//------------------------------------------------------------------------------
//*************************** Interrupciones ***********************************
void __interrupt() isr (void){
    // Interrupcion del timer0
    if (T0IF == 1){
        // Interrupcion cada 20ms: tmr0 100, prescaler 256, 8MHz de oscilador
        T0IF = 0;
        TMR0 = Valor_TMR0;
        // PWM
        Contador_Servos = 0;
        // SERVO 1
        RD0 = 1;
        while (Contador_Servos <= Valor_Servo_1){ // max 217, min 42 
            Contador_Servos++; // inicia en 16.5us, tiempo 5.5 us (char)
            Contador_Servos++; // inicia en 22.0us, tiempo 8 us (int)        
            Contador_Servos--;            
            Contador_Servos++;
            Contador_Servos--;
        }
        RD0=0;
        // SERVO 2
        Contador_Servos = 0;
        RD1 = 1;
        while (Contador_Servos <= Valor_Servo_2){ // max 199, min 98 
            Contador_Servos++; // inicia en 16.5us, tiempo 5.5 us (char)
            Contador_Servos++; // inicia en 22.0us, tiempo 8 us (int)        
            Contador_Servos--;            
            Contador_Servos++;
            Contador_Servos--;
        }
        RD1=0;
        // SERVO 3
        Contador_Servos = 0;
        RD2 = 1;
        while (Contador_Servos <= Valor_Servo_3){ // max 199, min 98 
            Contador_Servos++; // inicia en 16.5us, tiempo 5.5 us (char)
            Contador_Servos++; // inicia en 22.0us, tiempo 8 us (int)        
            Contador_Servos--;            
            Contador_Servos++;
            Contador_Servos--;
        }
        RD2=0;   
    } // Fin de interrupci?n timer0
    
    // Interrupcion del ADC module
    if (ADIF == 1){
        ADIF = 0;
        if (ADCON0bits.CHS == 0){
            ADRESH_Servo_1 = ADRESH;
            ADCON0bits.CHS = 1;
        } else if(ADCON0bits.CHS == 1){
            ADRESH_Servo_2 = ADRESH;
            ADCON0bits.CHS = 2;
        } else if(ADCON0bits.CHS == 2){
            ADRESH_Servo_3 = ADRESH;
            ADCON0bits.CHS = 0;
        }   
        __delay_us(50);
        ADCON0bits.GO = 1; 
    }
    
}    

void main(void) {
    // Oscilador
    IRCF0 = 1;       // Configuraci?n del reloj interno 
    IRCF1 = 1;
    IRCF2 = 1;       // 8 Mhz   
    
    // Configurar Timer0
    PS0 = 1;
    PS1 = 1;
    PS2 = 1;         //Prescaler de 256
    T0CS = 0;
    PSA = 0;
    INTCON = 0b10101000;
    TMR0 = Valor_TMR0;
    
    // Configuraci?n del modulo ADC
    PIE1bits.ADIE = 1;
    ADIF = 0; // Bandera de interrupci?n
    ADCON1bits.ADFM = 0; // Justificado a la izquierda    
    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG0 = 0; // Voltajes de referencia en VSS y VDD
    ADCON0bits.ADCS0 = 0;
    ADCON0bits.ADCS1 = 1; // FOSC/8
    ADCON0bits.ADON = 1;
    __delay_us(50);
    ADCON0bits.GO = 1;
    
    // Configurar puertos
    ANSEL  = 0b00000111;
    ANSELH = 0;
    TRISA  = 0xff;  // Definir el puerto A como entradas
    TRISC  = 0;     // Definir el puerto C como salida
    TRISD  = 0;     // Definir el puerto D como salida
    TRISE  = 0;     // Definir el puerto E como salida
    
    //Limpieza de puertos
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    PORTE = 0;
    
    //loop principal
    while(1){  
        Valor_Servo_1 = (ADRESH_Servo_1-0)*(217-42)/(255-0)+42;
        Valor_Servo_2 = (ADRESH_Servo_2-0)*(217-42)/(255-0)+42;
        Valor_Servo_3 = (ADRESH_Servo_3-0)*(217-42)/(255-0)+42;
    } // fin loop principal while 
} // fin main

