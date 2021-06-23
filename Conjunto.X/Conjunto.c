/*
 * File:   Conjunto.c
 * Author: Fredy 
 *
 */

// SERVOS, USART

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
unsigned char USART_Servo_1 = 127;
unsigned char USART_Servo_2 = 127;
unsigned char USART_Servo_3 = 127;
unsigned char USART_Num_Servo;
unsigned char USART_Receptor;
unsigned char Control = 1;
//------------------------------------------------------------------------------
//***************************** Prototipos *************************************
void USART_SERVOS(void);
void Regla_3(void);
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
    } // Fin de interrupción timer0
    
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
    }// Fin de Interrupcion ADC
    
    // Interrupcion USART
    if (RCIF == 1){
        // RCREG (Receptor)
        // TXREG (Transmisor)
        RCIF = 0; 
        USART_Receptor = RCREG;
        if (RCREG == '1'){
            USART_Num_Servo = 1;
        } else if(RCREG == '2'){
            USART_Num_Servo = 2;
        } else if(RCREG == '3'){
            USART_Num_Servo = 3;
        } else if (RCREG == '4'){
            Control = 0; // Control via ADC
        } else if (RCREG == '5'){
            Control = 1; // Control via USART
            USART_Servo_1 = ADRESH_Servo_1;
            USART_Servo_2 = ADRESH_Servo_2;
            USART_Servo_3 = ADRESH_Servo_3;
        }
    }// FIN Interrupcion USART
}    

void main(void) {
//------------------------------------------------------------------------------
//*************************** Configuraciones **********************************
    // Oscilador
    IRCF0 = 1;       // Configuración del reloj interno 
    IRCF1 = 1;
    IRCF2 = 1;       // 8 Mhz   
    
    // Configuración de Asynchronous TRANSMITTER
    TXEN = 1;
    SYNC = 0;
    SPEN = 1;
   
    // Configuración de Asynchronous RECEIVER
    CREN = 1;
    PIE1bits.RCIE = 1;
    PIR1bits.RCIF = 0;
    SPBRG=12;  // baudrate 9600 para 8MHz
    
    // Configurar Timer0
    PS0 = 1;
    PS1 = 1;
    PS2 = 1;         //Prescaler de 256
    T0CS = 0;
    PSA = 0;
    INTCON = 0b10101000;
    TMR0 = Valor_TMR0;
    
    // Configuración del modulo ADC
    PIE1bits.ADIE = 1;
    ADIF = 0; // Bandera de interrupción
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
    TRISB  = 0;     // Definir el puerto B como salida
    TRISD  = 0;     // Definir el puerto D como salida
    TRISE  = 0;     // Definir el puerto E como salida
    
    //Limpieza de puertos
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    PORTE = 0;
    
//------------------------------------------------------------------------------
//*************************** loop principal ***********************************
    while(1){        
        USART_SERVOS();
        Regla_3();
    } // fin loop principal while 
} // fin main

//------------------------------------------------------------------------------
//***************************** Funciones  *************************************
void USART_SERVOS(void){
    if(USART_Num_Servo == 1){ 
        if (USART_Receptor == 'w'){
            if(USART_Servo_1 <= 250){
                USART_Servo_1 = USART_Servo_1 + 5;    
            }
        } else if(USART_Receptor == 'q'){
            if(USART_Servo_1 >= 5){
                USART_Servo_1 = USART_Servo_1 - 5;
            }
        }         
    } else if(USART_Num_Servo == 2){
        if (USART_Receptor == 'w'){ 
            if(USART_Servo_2 <= 250){
                USART_Servo_2 = USART_Servo_2 + 5;
            }
        } else if(USART_Receptor == 'q'){
            if(USART_Servo_2 >= 5){
                USART_Servo_2 = USART_Servo_2 - 5;
            }
        }  
    } else if(USART_Num_Servo == 3){
        if (USART_Receptor == 'w'){
            if(USART_Servo_3 <= 250){
                USART_Servo_3 = USART_Servo_3 + 5;
            }
        } else if(USART_Receptor == 'q'){
            if(USART_Servo_3 >= 5){
                USART_Servo_3 = USART_Servo_3 - 5;
            }
        }  
    }
    USART_Receptor = 0;
}
void Regla_3(void){
    if (Control == 0){ // Via ADC
        Valor_Servo_1 = (ADRESH_Servo_1-0)*(217-42)/(255-0)+42;
        Valor_Servo_2 = (ADRESH_Servo_2-0)*(217-42)/(255-0)+42;
        Valor_Servo_3 = (ADRESH_Servo_3-0)*(217-42)/(255-0)+42; 
    } else if (Control == 1){ // Via USART
        Valor_Servo_1 = (USART_Servo_1-0)*(217-42)/(255-0)+42;
        Valor_Servo_2 = (USART_Servo_2-0)*(217-42)/(255-0)+42;
        Valor_Servo_3 = (USART_Servo_3-0)*(217-42)/(255-0)+42;
    }
    
}