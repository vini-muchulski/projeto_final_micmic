
#pragma config FOSC = XT     // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF         // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3/PGM pin has PGM function; low-voltage programming enabled)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <pic16f877a.h>
#define _XTAL_FREQ 20000000
#define INC_BTN !PORTDbits.RD0  // Increment button
#define DEC_BTN !PORTDbits.RD1 // Decrement button 
#define OK_BTN !PORTDbits.RD2 // botao que o usuario aperta qnd for confirmar a temperatura requerida
#define LED PORTCbits.RC3  // Somente para facilitar a leitura do programa
#define OUT_UNI PORTC // Unity display
#define OUT_DEZ PORTB // Tens display  
#define FLAG_TEMPORIZER flagsbits.f0 // Temporizer flag


unsigned int digital_value=0; 
unsigned int new_value=0;


void aumentar();

void diminuir();

void temporizer();

void ligar_aquecedor();

void desligar_aquecedor();

void ADC_Init() 
{
    ADRESL = 0x00;
    ADRESH = 0x00;

    ADCON0bits.ADCS1 = 1;
    ADCON0bits.ADCS0 = 1;
    ADCON0bits.CHS2 = 1;
    ADCON0bits.CHS1 = 0;
    ADCON0bits.CHS0 = 0;
    ADCON0bits.GO_nDONE = 0; //ONLY TURN ON WHEN READING
    ADCON0bits.ADON = 1;

    ADCON1bits.ADFM = 1;
    ADCON1bits.ADCS2 = 0;
    ADCON1bits.ADFM = 1;
    ADCON1bits.PCFG3 = 0;
    ADCON1bits.PCFG2 = 0;
    ADCON1bits.PCFG1 = 0;
    ADCON1bits.PCFG0 = 0;

    PIR1bits.ADIF = 0;
}

void Int_Init() 
{
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    PIE1bits.ADIE = 1;
}

void ADC_Read() 
{
    ADCON0bits.GO_nDONE = 1; //INITIALIZE A/D CONVERTION
}

void __interrupt() TrataInt(void)
{    
  if (ADIF)  //foi a interrup??o de final de convers?o AD?
     {  
        PIR1bits.ADIF = 0; //reseta o flag da interrup??o 
        digital_value=((ADRESH<<8)+ADRESL);
        new_value = digital_value/2;
        
        
  }
  return;
}

void main(void) {
    // configura  es e setup

    OUT_UNI = 0;
    OUT_DEZ = 0;
  
    OPTION_REGbits.nRBPU = 0; // Ativa resistores de pull-ups
    PORTA = 0xFF;
    PORTD = 0x00;
    TRISC = 0x00;
    PORTC = 0x00;
    TRISB = 0b00000000;       // Configura pinos de entrada(1)e saida (0)
    TRISC = 0b00000000;       // Configura pinos de entrada(1)e saida (0)
    TRISA = 0xFF;       // Configura pinos de entrada(1)e saida (0)
    TRISBbits.TRISB7 = 0; // RB7 como sa da 
    float temperatura_do_usuario = 0;
    float temperatura_requerida;
    float valor_do_sensor;
    float temperatura_do_sensor;
   __delay_us(20);
    ADC_Init();
    Int_Init();
    while (1) 
    {
        ADC_Read();
        __delay_ms(1);
        if(INC_BTN){
           aumentar(); // aumenta o valor no display apenas
           temperatura_do_usuario++;
       }
       if(DEC_BTN){
           diminuir(); // diminui o valor no display apenas
           temperatura_do_usuario--;
       }
       if(OK_BTN){
           temperatura_requerida = temperatura_do_usuario;
           
           if(temperatura_do_sensor < temperatura_requerida){
               while(temperatura_do_sensor < temperatura_requerida){
                   ADC_Read();
               ligar_aquecedor();
               temporizer();
               
               
               valor_do_sensor = new_value; // l  o valor convertido
               temperatura_do_sensor = new_value;
                
               temporizer();
       
               
               }
               desligar_aquecedor(); // sai do while qnd a temperatura requrida for igual 
                                       // a temperatura do ambiente e desliga o aquecedor
           }
           
           else{
               desligar_aquecedor();
               
           }
       }
    
    }



    return;
}

void aumentar(){
    if(OUT_UNI == 9){
        OUT_UNI = 0;
        
         if(OUT_DEZ == 9){
             OUT_DEZ = 0;
         }
        else {
      OUT_DEZ++;  
        }
        
        
    }
    
    else {
    OUT_UNI++; 
  }
    temporizer();
    
}


void diminuir(){
    if(OUT_UNI == 0) {
    OUT_UNI = 9;
    if(OUT_DEZ == 0) {
      OUT_DEZ = 9; 
    }
    else {
      OUT_DEZ--;
    }
  }
  else {
    OUT_UNI--;
  }

    temporizer();


}

void temporizer() {

  //while(!FLAG_TEMPORIZER); 
 // FLAG_TEMPORIZER = 0;
  __delay_ms(100);
}

void ligar_aquecedor(){
    // liga aquecedor 
    PORTBbits.RB7 = 1; 
}

void desligar_aquecedor(){
    // desliga aquecedor
  PORTBbits.RB7 = 0; 
}
