.org 0x0
.section .iv,"a"
		
.set IRQ_MODE,          	 0xD2
.set SUPERVISOR_MODE,    	 0x13
.set SYS_MODE,            	 0x1F

.align 4
.org 0x0                    
.section .iv, "a"

_start:		

interrupt_vector:
    b RESET_HANDLER
.org 0x8
	b SVC_HANDLER
.org 0x18
    b IRQ_HANDLER

.align 4
.text

@ Inicializa Stacks

	@ inicializa pilha do user
	msr cpsr_c, #SYS_MODE
	ldr sp, =0x77805000

	@ inicializa pilha do irq
	msr cpsr_c, #IRQ_MODE
	ldr sp, =0x77806000

	@ inicializa pilha do supervisor
	msr cpsr_c, #SUPERVISOR_MODE
	ldr sp, =0x77808000

@Iniciando o sistema 4.4
RESET_HANDLER:
	@ RESET machine time
	ldr r1, =TIME_COUNTER
	mov r0, #0
	str r0, [r1]

    @configurando o gpt
   	SET_GPT:
	   	@variaveis do gpt
		.set GPT_CR,                0x53FA0000 @configure
	    .set GPT_PR,                0x53FA0004 @prescaler, determine the divide value of the clock
	    .set GPT_SR, 				0x53FA0008 @counter roled over
	    .set GPT_OCR1,              0x53FA0010 @output compare register
	    .set GPT_IR,                0x53FA000C @interrupt register
	    .set TIME_SZ, 				100 

	    mov r0, #0

	    @Set interrupt table base address on coprocessor 15.
	    ldr r0, =interrupt_vector
	    mcr p15, 0, r0, c12, c0, 0

	    ldr r1, =GPT_CR
	    mov r2, #0X00000041
	    str r2, [r1]
	    
	    ldr r1, =GPT_PR
	    mov r2, #0
	    str r2, [r1]
	    
	    @time_sz
	    ldr r1, =GPT_OCR1
	    mov r2, #0x00010000 
	    str r2, [r1]
	    
	    ldr r1, =GPT_IR
	    mov r2, #1
	    str r2, [r1] 

	@configurando tzic
	SET_TZIC:
	 	@ Constantes para os endereços do TZIC
	   .set TZIC_BASE, 		0x0FFFC000
	   .set TZIC_INTCTRL, 	0x0
	   .set TZIC_INTSEC1, 	0x84 
	   .set TZIC_ENSET1,	0x104
	   .set TZIC_PRIOMASK, 	0xC
	   .set TZIC_PRIORITY9, 0x424

		@ Liga o controlador de interrupções
		@ R1 <= TZIC_BASE
		ldr	r1, =TZIC_BASE

		@ Configura interrupção 39 do GPT como não segura
		mov	r0, #(1 << 7)
		str	r0, [r1, #TZIC_INTSEC1]

		@ Habilita interrupção 39 (GPT)
		@ reg1 bit 7 (gpt)
		mov	r0, #(1 << 7)
		str	r0, [r1, #TZIC_ENSET1]

		@ Configure interrupt39 priority as 1
		@ reg9, byte 3
		ldr r0, [r1, #TZIC_PRIORITY9]
		bic r0, r0, #0xFF000000
		mov r2, #1
		orr r0, r0, r2, lsl #24
		str r0, [r1, #TZIC_PRIORITY9]

		@ Configure PRIOMASK as 0
		eor r0, r0, r0
		str r0, [r1, #TZIC_PRIOMASK]

		@ Habilita o controlador de interrupções
		mov	r0, #1
		str	r0, [r1, #TZIC_INTCTRL]
				   
		msr CPSR_c, #0x13
    
    @configurando o gpio
	SET_GPIO:
		.set GPIO_BASE,       0x53F84000
		.set GPIO_DR,         0x0
		.set GPIO_GDIR,       0x4
		.set GPIO_PSR,        0x8

    	ldr r1, =GPIO_BASE

    	@ configuracao de entrada e saída
    	@ entrada 0 saída 1
   		ldr r0, =0b11111111111111000000000000111110
   		str r0, [r1, #GPIO_GDIR]

IRQ_HANDLER:
    push {r0-r12}
    @ 1 em GPT_SR, tratando a interrupção
    mov r0, #1
    ldr r1, =GPT_SR
    str r0, [r1]

    @ Incrementa o contador de interrupções
    ldr r1, =TIME_COUNTER
    ldr r0, [r1]
    add r0, r0, #1
    str r0, [r1]

    @ Subtrai 4 do registrador de lr_irq (que contém PC+8 antes da interrupção)
    mov r0, lr
    sub r0, r0, #4
    mov lr, r0

    @ Faz o retorno da interrupção alterando também o modo
    pop {r0-r12}
    movs pc, lr

@ Chamada das funcoes relacionadas as syscalls 4.3
SVC_HANDLER:
    push {lr}

    cmp r7, #21
    bleq READ_SONAR

 	cmp r7, #20
    bleq SET_MOTOR_SPEED

    cmp r7, #17
    bleq GET_TIME

    cmp r7, #18
    bleq SET_TIME

    pop {lr}
    movs pc, lr

READ_SONAR:
	push {r4-r11, lr}

	ldr r1, =GPIO_BASE
	ldr r2, [r1, #GPIO_DR] @r2 <- DR

	@limpa esses bits do reg
	bic r2, r2, #0b111110
	@DR <- MUX <- ID
	orr r2, r2, r0, lsl #2
	
	@Delay 15ms
	mov r0, #15
	bl DELAY

	@trigger <- 1
	@GPIO_DR <- r2
	ldr r2, [r1, #GPIO_DR]
	@TRIGGER <- 1
	orr r2, r2, #0b10
	@r2 para GPIO_DR
    str r2, [r1, #GPIO_DR] 

    @Delay 15ms
    mov r0, #15
    bl DELAY

    @Verificar flag == 1
    loop_check:
   		@r2 <- GPIO_DR
    	ldr r2, [r1, #GPIO_DR]
    	@Flag em r2
    	and r2, r2, #0b1

    	@Verifica flag em r2
    	cmp r2, #1
    	beq loop_check_ok

    	@Flag diferente de 1, entra no loop novamente apos 10ms

    	mov r0, #10
    	bl DELAY

    	b loop_check

    loop_check_ok:
    	@r0 <- SONAR_DATA
        @r2 <- GPIO_DR (Data register)
        ldr r2, [r1, #GPIO_DR]
        mov r0, r2, lsr #6

        ldr r2, =0b111111111111

        @ r0 <- distancia
        and r0, r0, r2

    @retorna r0, distancia
    pop {r4-r11, pc}

DELAY:
	mov r0, r0, lsl #4
	delay_loop:
		cmp r0, #0
		beq delay_loop_end
		sub r0, r0, #1
		b delay_loop

	delay_loop_end:
		pop {r4-r11}
		mov pc, lr

SET_MOTOR_SPEED:
	push {r4-r11, lr}
	ldr r2, =GPIO_BASE			@ carrega valor de DR
	ldr r3, [r2, #GPIO_DR]

	@se id == 0, vai para o motor0
	cmp r0, #0				
	beq set_motor0

	@se id == 1, vai para o motor1
	cmp r0, #1				
	beq set_motor1

	set_motor0:
		@speed -> [19-24]
		lsl r1, #19
		@bits [18-24] == #0		
		bic r3, r3, #0x1FC0000
		@r3 <- speed
		orr r3, r3, r1	
		@GPIO_DR <- r3			
		str r3, [r2, #GPIO_DR]
		mov r0, #0
		b motor_speed_end

	set_motor1:
		@speed -> [26-31]
		lsl r1, #26				
		@bits [25-31] == #0		
		bic r3, r3, #0xFE000000
		@r3 <- speed
		orr r3, r3, r1
		@GPIO_DR <- r3		
		str r3, [r2, #GPIO_DR]
		mov r0, #0
		b motor_speed_end

	motor_speed_end:
		pop {r4-r11, lr}
		movs pc, lr

GET_TIME:
	@r0 <- TIME_COUNTER
	ldr r0, =TIME_COUNTER
	ldr r0, [r0]
	@return TIME_COUNTER
	mov pc, lr

SET_TIME:
	ldr r2, =TIME_COUNTER
	str r0, [r2]
	mov pc, lr

.data
	TIME_COUNTER:
		.word 0