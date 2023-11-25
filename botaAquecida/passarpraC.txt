
    #include "p16f877a.inc" ; INCLUDE DO PIC
    
    ; PALAVRA DE CONFIGURAÇÃO - HABILITA E DESABILITA ALGUMAS OPÇÕES
    __CONFIG _XT_OSC & _WDTE_OFF & _PWRTE_OFF & _BOREN_OFF & _LVP_OFF & _CPD_OFF & _WRT_OFF & _CP_OFF 
    
    ;---------- VARIÁVEIS P/ O PROG. -> SERÃO ESCRITAS NA RAM ----------
    cblock 0x20
	flags
	tmp_work
	tmp_status
	tmp_fsr
	tmp_pclath
	aux_timer0
    endc
    ;--------------------------------------------------------------------
    
    ;---------- 'NICK NAMES' PARA ALGUMAS INSTRUÇÕES (FALICITAR ENTENDIMENTO) ----------
    #define INC_BTN PORTD, 0
    #define DEC_BTN PORTD, 1
    #define RESET_BTN PORTD, 2
    #define OUT_UNI PORTB
    #define OUT_DEZ PORTC
    #define FLAG_TEMPORIZER flags, 0
    ;-----------------------------------------------------------------------------------
    
    org 0x00			; VETOR DE RESET
    goto start
    
    org 0x04			; VETOR DE INTERRUPÇÃO 
	
    context_saver:		; SALVA CONTEÚDO
	movwf	tmp_work	        
	swapf	STATUS,W	        
	movwf	tmp_status
	movf	FSR,W
	movwf	tmp_fsr	        
	movf	PCLATH,W
	movwf	tmp_pclath	        
	clrf	PCLATH		        
	clrf	STATUS	 
	
				; TRATA INTERRUPÇÃO
	btfss INTCON, TMR0IF    ; TESTA SE A INTERRUPÇÃO OCORREU PELO OVERFLOW DO TIMER
	goto exit_INT		; SE NÃO, SAI DA INTERRUPÇÃO. SE SIM, CONTINUA.
	
	bcf INTCON, TMR0IF 	; LIMPA A FLAG
	movlw .0		; RECARREGA TIMER
	movwf TMR0	
	
	decfsz aux_timer0, F	; DECREMENTA AUX - SE NÃO FOR ZERO, DECREMENTA E
	goto exit_INT		; SAI
	
	movlw .128		; SE ZERO, RECARREGA AUX E
	movwf aux_timer0
	
	BSF FLAG_TEMPORIZER	; SETA FLAG TEMPORIZADORA
			     
    exit_INT:			; RECUPERA REGISTRADORES ANTES DE SAIR DA FUNC
	movf	tmp_pclath,W
	movwf	PCLATH		
	movf	tmp_fsr,W
	movwf	FSR		
	swapf	tmp_status,W
	movwf	STATUS		
	swapf	tmp_work,F
	swapf	tmp_work,W	
	
     retfie
    
    ;---------- CONFIG. INICIAIS - PORTAS ----------
    start: 
	bsf STATUS, RP0 ; SELECIONA BANCO 01
	
	movlw 0x00		; SELECIONA PORTAS B E C COMO SAÍDA
	movwf TRISB 
	movlw 0x00
	movwf TRISC
	movlw 0xFF		; SELECIONA PORTAS D COMO ENTRADA
	movwf TRISD
	
	movlw b'10010100'	; SETA OPÇÕES, PRINCIPALMENTE: CICLO DE CLOCK INTERNO 
			  	; IMPLEMENTA TIMER0, PREESCALER DESIGNADO PRA TIMER0,
	movwf OPTION_REG	; PREESCALER RATE 1:32.
	
	bcf STATUS, RP0		; SELECIONA BANCO 0
	
	bsf INTCON, 7		; HABILITA INTERRUPÇÃO GLOBAL
	bsf INTCON, 5		; HABILITA INTERRUPÇÃO PELO TIMER0
	
	movlw .0		; INICIALIZA TIMER0 EM '0'
	movwf TMR0 
	
	bcf FLAG_TEMPORIZER	; LIMPA FLAG TEMPORIZADORA
	
	movlw .128		; MOVE '128' PARA A VARIÁVEL AUXILIAR
	movwf aux_timer0

	clrf OUT_UNI		; ZERA OS DISPLAYS
	clrf OUT_DEZ
    ;------------------------------------------------
    
    
    ;---------- FUNÇÃO PRINCIPAL - AGUARDA ACIONAMENTO DE ALGM BTN ----------
    main:
	btfss INC_BTN
	call increase
	
	btfss DEC_BTN
	call decrease
	
	btfss RESET_BTN
	call zeroOnDisplay
	
     goto main
    ;------------------------------------------------------------------------
    
    increase:
	call temporizer	    ; CHAMA TEMPORIZADOR
	movf OUT_UNI, W	    ; RECUPERA CONTEUDO DA SAIDA DE UNIDADE PARA O WREG
	
	addlw .247	    ; CONFERE SE A UNIDADE JÁ EH '9' PARA CHAMAR DEZENA
	btfsc STATUS, Z	    ; SE CHEGOU, CHAMA FUNÇÃO Q COMPLEMENTA DEZENA
	call increaseDez
	
	movf OUT_UNI, W	    ; SE NÃO, APENAS COMPLEMENTA UNIDADE
	addlw 0x01
	movwf OUT_UNI
	
     return		    ; VOLTA PRA MAIN
	
    decrease:
	call temporizer	    ; CHAMA TEMPORIZADOR
	movf OUT_UNI, W	    ; RECUPERA CONTEUDO DA SAIDA DE UNIDADE PARA O WREG
	
	addlw 0x00 	    ; CONFERE SE A UNIDADE JÁ EH '0' PARA CHAMAR DEZENA
	btfsc STATUS, Z	    ; SE JÁ CHEGOU, CHAMA FUNÇÃO Q DECREMENTA DEZENA
	call decreaseDez
	
	movf OUT_UNI, W	    ; SE NÃO, APENAS DECREMENTA UNIDADE
	addlw .255	    ; ADD 255 = DEC em 1, já que, ex: 08h + 255h = 07h
	movwf OUT_UNI
	

     return		    ; VOLTA PRA MAIN
	
    
    increaseDez:
	
	movlw 0x00	    ; ZERA DISPLAY UNIDADE
	movwf OUT_UNI
	
	movf OUT_DEZ, W	    ; RECUPERA CONTEUDO DA SAIDA DA DEZENA PARA O WREG
	
	addlw .247	    ; CONFERE SE DEZENA JÁ EH '9' PARA ZERAR
	btfsc STATUS, Z	    ; SE JÁ CHEGOU ZERA DISPLAYS, JÁ Q EH '99'
	call zeraQndNineNine
	
	movf OUT_DEZ, W	    ; SE NÃO, APENAS INCREMENTA DEZENA
	addlw 0x01
	movwf OUT_DEZ
	
	
     goto main		    ; VOLTA PRA MAIN
	
    
    decreaseDez:
	
	movlw 0x09	    ; BOTA '9' NO DISPLAY NA UNIDADE
	movwf OUT_UNI
	
	movf OUT_DEZ, W	    ; RECUPERA CONTEUDO DA SAIDA DA DEZENA PARA O WREG
	
	addlw 0x00	    ; CONFERE SE DEZENA JÁ EH '0' PARA POR '99'
	btfsc STATUS, Z	    ; SE JÁ CHEGOU BOTA '99' NOS DISPLAYS, JÁ Q EH '00'
	call NineNineQndZeroZero
	
	movf OUT_DEZ, W	    ; SE NÃO, APENAS DECREMENTA DEZENA
	addlw .255	    ; ADD 255 = DEC em 1, já que, ex: 08h + 255h = 07h
	movwf OUT_DEZ
	
     
     goto main		    ; VOLTA PRA MAIN
     
    
    zeraQndNineNine:	    ; QUANDO CHEGA EM 99 E O BOTÃO DE INCREMENTO FOR 
			    ; ACIONADO ESSA FUNÇÃO EH ATIVADA.

	movlw 0x00	    ; MOVE '0' PARA UNIDADE PARA SER MOSTRADO NO DISPLAY
	movwf OUT_UNI
	movlw .255	    ; MOVE '255' PARA DEZENA, POIS QUANDO ELA RETORNAR 
			    ; PARA A FUNÇÃO DE INCREMENTO, ELA SERÁ INCREMENTADA 
			    ; EM '1' + UMA VEZ, OQ RESULTARÁ EM '0' E ENTÃO
			    ; SERÁ MOSTRADA NO DISPLAY
	movwf OUT_DEZ
	
	
     return
     
    NineNineQndZeroZero:   ; QUANDO CHEGA EM 00 E O BOTÃO DE DECREMENTO FOR 
			   ; ACIONADO ESSA FUNÇÃO EH ATIVADA.
	
	
	movlw 0x09	    ; MOVE '9' PARA UNIDADE PARA SER MOSTRADO NO DISPLAY
	movwf OUT_UNI
	movlw 0x0A	    ; MOVE '10' PARA DEZENA, POIS QUANDO ELA RETORNAR 
			    ; PARA A FUNÇÃO DE DECREMENTO, ELA SERÁ DECREMENTADA 
			    ; EM '1' + UMA VEZ, OQ RESULTARÁ EM '9' E ENTÃO
			    ; SERÁ MOSTRADA NO DISPLAY
	movwf OUT_DEZ
	
     return
	
	
    zeroOnDisplay:
	call temporizer
	movlw 0x00
	movwf OUT_UNI
	movlw 0x00
	movwf OUT_DEZ
	
     
     return
    
    
    ;-------------------- TEMPORIZADOR --------------------
    temporizer:
	btfss FLAG_TEMPORIZER
	goto temporizer
	bcf FLAG_TEMPORIZER
     return	
    ; TEMPORIZADOR = CONTAGEM TIMER0 X PRESCALER X CICLO DE MAQUINA X AUX_TIMER0
    ; TEMPORIZADOR =	  256	     X	  32	 X     1.10^-6	    X     128
    ; TEMPORIZADOR = 1,05s aprox.
    ;-----------------------------------------------------
     
     
    end				; FIM DO PROGRAMA    