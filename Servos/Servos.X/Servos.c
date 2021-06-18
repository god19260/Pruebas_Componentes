/*
 * File:   Proyecto_2.c
 * Author: Ana Barrientos
 *
 * Created on 17 de mayo de 2021, 09:42 PM
 */
//PIC16F887
//---------------------------CONFIGURACIONES--------------------------
//CONFIG 1
#pragma config FOSC=INTRC_NOCLKOUT // Oscilador interno sin salidas
#pragma config WDTE=OFF // WDT disabled (reinicio repetitivo del pic)
#pragma config PWRTE=OFF // PWRT enabled (espera de 72ms al iniciar)
#pragma config MCLRE=OFF // El pin de MCLR se utiliza como I/O
#pragma config CP=OFF // Sin protección de código 
#pragma config CPD=OFF // Sin protección de datos
    
#pragma config BOREN=OFF // Sin reinicio cuándo el voltaje de alimentación baja 
#pragma config IESO=OFF // Reinicio sin cambio de reloj de interno a externo
#pragma config FCMEN=OFF // Cambio de reloj externo a interno en caso de fallo 
#pragma config LVP=OFF // Programación en bajo voltaje permitida
//CONFIG 2
#pragma config WRT=OFF // Protección de autoescritura por el programa desactivad
#pragma config BOR4V=BOR40V // Reinicio abajo de 4V, (BOR21V=2.1V)
//--------------------------------------------------------------------
//Librerias
//-----------------------------------------------------------------------------
#include <xc.h>
#include <stdint.h>     //tipos de datos standard y otros
#include <math.h>
//-----------------------------------------------------------------------------
//Directivas del compilador
//-----------------------------------------------------------------------------
#define _XTAL_FREQ 1000000              //Cristal de 8MHz
//-----------------------------------------------------------------------------
//Variables
//-----------------------------------------------------------------------------
char pot_1;

//-----------------------------------------------------------------------------
//Prototipos de funciones (Como son mis funciones antes)
//-----------------------------------------------------------------------------
void setup(void);
void PWM_1(void);
//-----------------------------------------------------------------------------
//Interrupción
//-----------------------------------------------------------------------------
void __interrupt() isr (void)
{
    if (PIR1bits.ADIF == 1) {
        PIR1bits.ADIF = 0;      //Limpio la bandera
        //Revisa si la bandera del ADC está encendida
        if (ADCON0bits.CHS == 0) //Si está encendida, revisa si RA0 está encen.
        {   //Si está encendido carga el valor al CCPR1L del PWM, RC1
            pot_1 = ADRESH;
            //PWM_1();
            RD0 = RC2;
            ADCON0bits.CHS = 1;
        }
        else if (ADCON0bits.CHS == 1){      
            //Si no, carga el valor al otro CCPR1L, RC2
            ADCON0bits.CHS = 0;
           
        }
    
    }
    __delay_us(50);         // Le da tiempo para cargar el capacitor 
    ADCON0bits.GO = 1;      //Va a convertir
    //PIR1bits.ADIF = 0;      //Limpio la bandera
}

//-----------------------------------------------------------------------------
//Main
//-----------------------------------------------------------------------------
void main(void) {
    setup();         //Solo lo configura una vez  
    while (1)
    {
        
        PWM_1();
}
}



//-----------------------------------------------------------------------------
//Funciones
//-----------------------------------------------------------------------------
void setup (void)
{
    //Configuracion de entradas y salidas
    ANSEL = 0b00000011; //Entradas analogicas
    ANSELH = 0; //Entradas digitales
    TRISC = 0b00000000;      //Salidas
    TRISA = 0b00000011;     //Entradas
    TRISB = 0b00000000;
    TRISD = 0b00000000;
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    PORTE = 0; //Clear inicial en los puertos 
    
    //Configuracion del oscilador 
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 0;
    OSCCONbits.IRCF0 = 0; //Frecuencia del oscilador de 1MHz
    OSCCONbits.SCS = 1;
    
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
    
    
    //Configuracion de interrupciones
    INTCONbits.PEIE = 1; //Habilita interrupciones perifericas
    INTCONbits.GIE = 1; //Habilita interrupciones globales
    
    //Configuración PWM CCP1
    PORTC = 0b000000100;        //RC1 como entrada
    PR2 = 255;                  //Frecuencia de 16.03 ms
    CCP1CONbits.P1M = 0b00;     //Una sola salida
    CCP1CONbits.CCP1M3 = 1;
    CCP1CONbits.CCP1M2 = 1;
    CCP1CONbits.CCP1M1 = 0;     //Modo PWM (1 1 0 0)
    CCP1CONbits.CCP1M0 = 0;
    
    CCPR1L = 0;                 //Valor inicial para CCP1
    CCP1CONbits.DC1B = 0;       //Bits menos significativos del PWM 
    
    //Timer 2
    PIR1bits.TMR2IF = 0;        //Clear para la bandera inicial
    T2CONbits.T2CKPS1 = 1;      
    T2CONbits.T2CKPS0 = 1;      //Prescaler de 16
    T2CONbits.TMR2ON = 1;       //Activa el tmr2
    
    while (PIR1bits.TMR2IF == 0);
    PIR1bits.TMR2IF = 0;        //Clear para la bandera inicial
    PORTC = 0; 
    
    return;
}

void PWM_1(void){
    
    CCPR1L = (pot_1-0)*(32-15)/(255-0)+15;     //32 máx y 15 mín
    //RD0 = RC2;
    
    return;
}
