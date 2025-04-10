
// PIC24FJ128GA010 Configuration Bit Settings

// 'C' source line config statements

// CONFIG2
#pragma config POSCMOD = XT             // Primary Oscillator Select (XT Oscillator mode selected)
#pragma config OSCIOFNC = ON            // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as port I/O (RC15))
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = PRI              // Oscillator Select (Primary Oscillator (XT, HS, EC))
#pragma config IESO = ON                // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) enabled)

// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config WINDIS = ON              // Watchdog Timer Window (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
#pragma config FWDTEN = ON              // Watchdog Timer Enable (Watchdog Timer is enabled)
#pragma config ICS = PGx2               // Comm Channel Select (Emulator/debugger uses EMUC2/EMUD2)
#pragma config GWRP = OFF               // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG port is disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include "xc.h"
#include "libpic30.h"
static unsigned char snake = 0x01;  
static char snakeDir = 1;
static unsigned char queueVal = 0x00;
static unsigned char lfsr = 0x3B;
int main(void) {
    unsigned port=0x0001;
    char current6=0;
    char prev6=0;
    TRISA=0x0000;
    TRISD=0xFFFF;
    int value=1;
   
    while(1){
        switch(value){
            case 1:
                LATA=port;
                port++;
                if(port ==0xFF){
                    port=0;
                }
                
                break;
            
            case 2:
                LATA=port;
                port--;
                if(port ==0xFF){
                    port=0;
                }
                break;
            case 3:
                
                LATA = (port >> 1) ^ port;
                port++;
                if(port ==0xFF){
                    port=0;
                }
                break;
            case 4:
                
                LATA=(port >>1) ^port;
                port--;
                if(port ==0x00){
                    port=0xFF;
                }
                break;
            case 5:
                
                LATA=((port/10)<<4)|port %10;
                port++;
                if(port>99){
                    port=0;
                }
                break;
            case 6:
                LATA=((port/10)<<4)|port%10;
                port--;
                if(port<0){
                    port=99;
                }
                break;
            case 7:
                if(snakeDir){
                    snake<<=1;
                }else{
                    snake>>=1;
                }
                if(snake ==0x04){
                    snakeDir=0;
                }else if(snake==0x01){
                    snakeDir =1;
                }
                LATA=snake;
                break;
            case 8:
                LATA= queueVal;
                queueVal=(queueVal <<1)|0x01;
                if(queueVal==0xFF){
                    queueVal=0x00;
                }
                break;
            case 9:
                unisgned char feedback= ((lfsr >> 0) ^ (lfsr >> 1) ^ (lfsr >> 3) ^ (lfsr >> 4)) & 0x01;
                lfsr = (lfsr >> 1) | (feedback << 5);
                LATA = lfsr;
                break;
        }
        prev6 = PORTDbits.RD6;
        __delay32(150000);
        current6=PORTDbits.RD6;
        
        if(prev6==1 && current6==0){
            value++;
            if(value>9){
                value=1;
            }
            port=1;
        }
    }
    
    return 0;
}
