#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF       // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)



#include <xc.h>            // Inclusao da biblioteca do compilador



#define _XTAL_FREQ 4000000 // fosc 4Mhz neste caso

#define LED PORTCbits.RC3  // Somente para facilitar a leitura do programa

int valor; // var para guardar valor da conversao

#define INC_BTN !PORTDbits.RD0  // Increment button
#define DEC_BTN !PORTDbits.RD1 // Decrement button 
#define OK_BTN !PORTDbits.RD2 // botao que o usuario aperta qnd for confirmar a temperatura requerida

#define OUT_UNI PORTC // Unity display
#define OUT_DEZ PORTB // Tens display  

#define FLAG_TEMPORIZER flagsbits.f0 // Temporizer flag


float calcular_temperatura(float valor);

void aumentar();

void diminuir();

void temporizer();

void ligar_aquecedor();

void desligar_aquecedor();

unsigned int ADC_Read(unsigned int channel);












//*** subrotina para tratar a interrrup??o *************************************
void __interrupt() TrataInt(void)
{
  if (PIR1bits.ADIF)  //foi a interrupcao de final de conversao AD?
     {
        PIR1bits.ADIF = 0; //reseta o flag da interrupcao
        valor = (ADRESH << 8) | ADRESL;;    // Passa valores convertido do reg para a variavel
        PORTB = (unsigned char)valor;    // Coloca os valores convertido no port B em binario

        //testa valor da conversao em formato decimal
        //if (valor == 55)
        //    LED = 1;
        //else
        //    LED = 0;
  }
  return;
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
   TRISA = 0xFF;       // Configura pinos de entrada(1)e saida (0)
   
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
   ADCON0bits.CHS = 0;
   
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
   
   
   
   
   
   
   
   
   
   
   
   //****************************************************************************************
   
   // configura��es e setup

  OUT_UNI = 0;
  OUT_DEZ = 0;
  
   TRISBbits.TRISB7 = 0; // RB7 como sa�da 
  ADCON1bits.PCFG0 = 1; // RA0 configurado como I/O digital
   ADCON1bits.PCFG1 = 0; // RA1 configurado como entrada anal�gica
   
   ADCON0 = 0x81;
   ADCON1 = 010000000;
   
   ADCON0bits.CHS0 = 0; 
    ADCON0bits.CHS1 = 1; // Canal AN1 selecionado
    
    
    float temperatura_do_usuario = 0;
    float temperatura_requerida;
    float valor_do_sensor;
     float temperatura_do_sensor;
   __delay_us(20);
  // ADCON0bits.GO = 1; // Inicia a convers�o
   
   
   while (1)   //loop infinito
   {
        ADCON0bits.GO = 1; // Inicia a convers�o
       //ADRESH = 0;
       //ADRESL = 0;
 
       // while(ADCON0bits.GO == 1) {}; // Espera at� a convers�o acabar
      // ADCON0bits.GO = 1;  // INICIA CONVERS?O
     //  __delay_us(10);     // Tempo de conversao
     //  __delay_ms(1);      // Tempo de conversao
       
       
       valor_do_sensor = ADRESH; // l� o valor convertido
       temperatura_do_sensor = calcular_temperatura(valor_do_sensor);
       
       
       // mostrar a temperatura que o usuario setou no display
       // comparar a temperatura setado pelo user com a do sensor
       // === se for menor que a dor sensor -> ligar aquecedor
       // se a temp do sensor for maior que a user -> desligar sensor
       
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
                   
               ligar_aquecedor();
               temporizer();
               
               
               valor_do_sensor = ADRESH; // l� o valor convertido
               temperatura_do_sensor = calcular_temperatura(valor_do_sensor);
                
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
}





float calcular_temperatura(float valor){
    float temperatura;
    temperatura = (valor * 5 * 100) / 1023; // LM35 - 10mV/oC, tens�o alimenta��o 5V
    return temperatura;
    
    
    /*
     Onde:

5 = tens�o alimenta��o em Volts
100 = fator escala, 10mV -> 0,01V (100x)
1023 = resolu��o conversor AD
Exibir ou usar o valor de temperatura para o controle desejado.
Assim l�-se a informa��o do sensor de temperatura na entrada anal�gica AN1 e converte para temperatura em oC.
     */
}

 /*
  * 
  * float calcTemperatura(float Tensao){
    float Temperatura = (Tensao - 1.6111)/(-0.0218);    // Equa��o normalizada da temperatura
    valor = Temperatura;	
    return valor;
  * 
  * 
}
  * 
  * */

/* 
Fun��o que calcula temperatura  baseada na linearidade da resistencia do sensor que foi usado como referencia em aula.
*/


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