// PIC24FJ128GA010 Configuration Bit Settings

// CONFIG2
#pragma config POSCMOD = XT
#pragma config OSCIOFNC = ON
#pragma config FCKSM = CSDCMD
#pragma config FNOSC = PRI
#pragma config IESO = ON

// CONFIG1
#pragma config WDTPS = PS32768
#pragma config FWPSA = PR128
#pragma config WINDIS = ON
#pragma config FWDTEN = ON      // WDT włączony
#pragma config ICS = PGx2
#pragma config GWRP = OFF
#pragma config GCP = OFF
#pragma config JTAGEN = OFF

#include <xc.h>
#include <libpic30.h>

// -----------------
// Zmienne globalne:
// -----------------
static unsigned char snake  = 0x01;  // do case 7
static char          snakeDir = 1;

static unsigned char queueVal = 0x00; // do case 8

static unsigned char lfsr   = 0x3B;   // do case 9 (6-bitowy rejestr, stan początkowy)

// -----------------
// Funkcja główna
// -----------------
int main(void)
{
    // port = zmienna do liczenia w case 1..6; 
    // używam 'int', by warunki typu (port < 0) działały poprawnie:
    int port = 1;

    char current6 = 0;
    char prev6    = 0;

    // Ustawienia portów
    TRISA = 0x0000;  // Port A na wyjście
    TRISD = 0xFFFF;  // Port D na wejście (RD6 itp.)

    // tryb/stan (value = 1..9)
    int value = 1;

    while(1)
    {
        switch(value)
        {
            case 1:  // Licznik binarny w górę (8 bitów)
                LATA = (unsigned char)port;
                port++;
                if(port > 0xFF){
                    port = 0;
                }
                break;

            case 2:  // Licznik binarny w dół
                LATA = (unsigned char)port;
                port--;
                if(port < 0){
                    port = 0xFF;
                }
                break;

            case 3:  // Licznik w kodzie Graya (w górę)
            {
                unsigned char gray = ((unsigned char)port >> 1) ^ (unsigned char)port;
                LATA = gray;
                port++;
                if(port > 0xFF){
                    port = 0;
                }
                break;
            }

            case 4:  // Licznik w kodzie Graya (w dół)
            {
                unsigned char gray = ((unsigned char)port >> 1) ^ (unsigned char)port;
                LATA = gray;
                port--;
                if(port < 0){
                    port = 0xFF;
                }
                break;
            }

            case 5:  // Licznik BCD w górę (0..99)
            {
                unsigned char bcd = (((unsigned char)(port / 10)) << 4) | (unsigned char)(port % 10);
                LATA = bcd;
                port++;
                if(port > 99){
                    port = 0;
                }
                break;
            }

            case 6:  // Licznik BCD w dół (99..0)
            {
                unsigned char bcd = (((unsigned char)(port / 10)) << 4) | (unsigned char)(port % 10);
                LATA = bcd;
                port--;
                if(port < 0){
                    port = 99;
                }
                break;
            }

            case 7:  // "Wężyk" 3-bitowy
            {
                // Przesuw w lewo/prawo
                if(snakeDir) {
                    snake <<= 1;
                } else {
                    snake >>= 1;
                }
                // Zmiana kierunku na krawędziach
                if(snake == 0x04){
                    snakeDir = 0;  // w prawo
                } else if(snake == 0x01){
                    snakeDir = 1;  // w lewo
                }
                LATA = snake;
                break;
            }

            case 8:  // "Kolejka"
            {
                LATA = queueVal;
                // Przesuw w lewo i ustaw bit0 na 1
                queueVal = (queueVal << 1) | 0x01;
                // Sprawdź wypełnienie
                if(queueVal == 0xFF){
                    queueVal = 0x00;
                }
                break;
            }

            case 9:  // 6-bitowy LFSR z taps: bity 0,1,3,4
            {
                unsigned char feedback = 
                  ((lfsr >> 0) ^ (lfsr >> 1) ^ (lfsr >> 3) ^ (lfsr >> 4)) & 0x01;
                // Przesuwamy w prawo i wstawiamy feedback w bit5
                lfsr = (lfsr >> 1) | (feedback << 5);
                // Wyświetlamy
                LATA = lfsr;
                break;
            }
        }

        // Przycisk RD6, zbocze opadające
        prev6 = current6;
        current6 = PORTDbits.RD6;

        if(prev6 == 1 && current6 == 0){
            value++;
            if(value > 9){
                value = 1;
            }
            // Resetowanie port i innych zmiennych, jeśli chcesz 
            // za każdym razem startować od początku
            port     = 1;   
            snake    = 0x01;
            snakeDir = 1;
            queueVal = 0x00;
            lfsr     = 0x3B;
        }

        // Proste opóźnienie
        __delay32(150000);

        // WDT (opcjonalnie) – jeśli WDT jest włączony, czyść go
        ClrWdt();
    }
    return 0;
}
