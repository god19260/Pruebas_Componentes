
/*
 * File:   USART
 * Author: Fredy
 *
 * Created on May 3, 2021, 1:12 PM
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
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3/PGM 
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
int V_Receptor;

//------------------------------------------------------------------------------
//***************************** Prototipos *************************************

//------------------------------------------------------------------------------
//*************************** Interrupciones ***********************************
void __interrupt() isr (void){
    
    if (RCIF == 1){
        // RCREG (Receptor)
        // TXREG (Transmisor)
        RCIF = 0; 
        PORTB = RCREG;
        TXREG = PORTB;  
    }
}    

void main(void) {
//------------------------------------------------------------------------------
//*************************** Configuraciones **********************************
    // Oscilador
    IRCF0 = 1;       // Configuración del reloj interno 
    IRCF1 = 1;
    IRCF2 = 1;       // 8 Mhz   
       
    INTCON = 0b11101000;
    // Configuración de Asynchronous TRANSMITTER
    TXEN = 1;
    SYNC = 0;
    SPEN = 1;
   
    // Configuración de Asynchronous RECEIVER
    CREN = 1;
    PIE1bits.RCIE = 1;
    PIR1bits.RCIF = 0;
    SPBRG=12;  // baudrate 9600 para 8MHz
  
    // Configurar puertos
    ANSEL  = 0;
    ANSELH = 0;
    TRISB  = 0;  // Definir el puerto A como salida
    TRISD  = 0;  // Definir el puerto D como salida
    
    
    //Limpieza de puertos
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    PORTE = 0;
       
    //loop principal
    while(1){  
        
        
    } // fin loop principal while 
} // fin main