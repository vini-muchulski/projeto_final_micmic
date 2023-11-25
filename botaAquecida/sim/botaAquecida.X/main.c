#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

#include <xc.h>            // Inclusao da biblioteca do compilador

#define _XTAL_FREQ 4000000 // fosc 4Mhz neste caso

#define LED PORTCbits.RC3  // Somente para facilitar a leitura do programa

int valor; // var para guardar valor da conversao
 
//*** subrotina para tratar a interrrup??o *************************************
void __interrupt() TrataInt(void)
{    
  if (ADIF)  //foi a interrupcao de final de conversao AD?
     {  
        PIR1bits.ADIF = 0; //reseta o flag da interrup??o
      
        valor = ADRESH;    // Passa valores convertido do reg para a variavel
        PORTB = valor;     // Coloca os valores convertido no port B em binario
       
        //testa valor da conversao em formato decimal
        if (valor == 55)
            LED = 1;
        else
            LED = 0;          
  }
  return;
}
float calcTemperatura(float Tensao){
    float Temperatura = (Tensao - 1.6111)/(-0.0218);    // Equa��o normalizada da temperatura
    return Temperatura;
}

void main(void) 
{  
       //** Configurando interrupcoes ***********************************
   INTCONbits.GIE  = 1;       // Habiliita a int global
   INTCONbits.PEIE = 1;       // Habilita a int dos perifericos
   PIE1bits.ADIE   = 1;       // Habilita int do AD
   //** Inicializacoes ************************************************
   TRISB = 0b00000000;       // Configura pinos de entrada(1)e saida (0)
   TRISC = 0b00000000;       // Configura pinos de entrada(1)e saida (0)
   TRISA = 0b11111111;       // Configura pinos de entrada(1)e saida (0)
   OPTION_REGbits.nRBPU = 0; // Ativa resistores de pull-ups
   
   //*** Configura o conversor analogico/digital*******************************
   // PORTA AN0
   // Configura Somente com AN0 como entrada analogica
   // Ver tabela no datasheet (pg.128)
   // Vdd/Vss como Vref+/Vref-
   //**************************************************************************
   ADCON1bits.PCFG0 = 0;     // Configura as entradas analogicas
   ADCON1bits.PCFG1 = 1;     // Configura as entradas analogicas
   ADCON1bits.PCFG2 = 1;     // Configura as entradas analogicas
   ADCON1bits.PCFG3 = 1;     // Configura as entradas analogicas
   
   // Define o clock de conversao
   ADCON0bits.ADCS0 = 0  ;   // Confirmando default Fosc/2
   ADCON0bits.ADCS1 = 0  ;   // Confirmando default Fosc/2
   
   ADCON1bits.ADFM  = 0   ;  // Default 0. Pra mostra que pegar os dados em 
                             // 8 bits no ADRESH, pois ests justifica a esquerda
                             // Passando pra 1 pode pegar os dados em 10 bits 
                             // usando os dois registradores ADRESHL justificado 
                             // a direita
   
   // Inicializa registradores do AD
   ADRESL = 0x00;            // Valor inicial da conversao em 0
   ADRESH = 0x00;          
   
   ADCON0bits.ADON = 1;      // Liga AD
   
   // **** inicializando saidas
   LED = 0;   // Apaga LED
   PORTB = 0; // Apaga LEDS da conversao 
   
   ADCON0bits.ADON = 1;  //
   
   //Seleciona canal de entrada 0 como entrada analogica
   ADCON0bits.CHS0 = 0; // Configura canal 0 como entrada analogica
   ADCON0bits.CHS1 = 0; // Configura canal 0 como entrada analogica
   ADCON0bits.CHS2 = 0; // Configura canal 0 como entrada analogica
     
   while (1)   //loop infinito
   {
       ADCON0bits.GO = 1;  // INICIA CONVERS?O
       __delay_us(10);     // Tempo de conversao
       __delay_ms(1);      // Tempo de conversao

    }
}