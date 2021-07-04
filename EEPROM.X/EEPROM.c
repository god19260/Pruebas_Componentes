
/*
 * File:   EEPROM
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
char Escritura = 0;
char Lectura = 0;
char Lectura_Enable = 0;
//------------------------------------------------------------------------------
//***************************** Prototipos *************************************
void Escritura_EEPROM(char direccion, char dato);
char Lectura_EEPROM (char direccion);
//------------------------------------------------------------------------------
//*************************** Interrupciones ***********************************
void __interrupt() isr (void){
    if (RCIF == 1){
        // RCREG (Receptor)
        // TXREG (Transmisor)
        RCIF = 0; 
        if (RCREG == 'p'){
            Escritura = 1;
        }else if(RCREG == 'o'){
            Escritura = 2;
        }else if(RCREG == 'r'){
            Lectura_Enable = 1;
        }
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
        if (Escritura == 1){
            Escritura_EEPROM(0, 11); // direccion, dato
            Escritura = 0;
        }else if (Escritura == 2){
            Escritura_EEPROM(0, 6); // direccion, dato
            Escritura = 0;
        }
        if (Lectura_Enable == 1){
            Lectura = Lectura_EEPROM (0);
            Lectura_Enable = 0;
        }
        
        if (Lectura == 11){
            RD7 = 1;
            RD6 = 0;
        }else if(Lectura == 6){
            RD7 = 0;
            RD6 = 1;
        }
    } // fin loop principal while 
} // fin main

//------------------------------------------------------------------------------
//***************************** Funciones  *************************************

void Escritura_EEPROM(char direccion_Escritura, char dato_Escritura){
    RD2 = 1;
    EEADR = direccion_Escritura; // direccion que se quiere modificar
    EEDAT = dato_Escritura;      // dato que queremos colocar en la 
                                 // direccion seleccionada
    EECON1bits.EEPGD = 0; // Point to data memory
    EECON1bits.WREN  = 1; // Habilita la escritura
    
    // Secuancia requerida
    INTCONbits.GIE = 0; // Se apagan las interrupciones
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;
    INTCONbits.GIE = 1; // Se encienden las interrupciones
    
    __delay_ms(100);
    EECON1bits.WREN  = 0; // deshabilita la escritura
}

char Lectura_EEPROM (char direccion_Lectura){
    EEADR = direccion_Lectura; // direccion que se quiere leer
    EECON1bits.EEPGD = 0; // Point to data memory
    EECON1bits.RD  = 1; // Habilita la lectura
 
    return EEDAT;
}