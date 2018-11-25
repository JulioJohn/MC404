.global SET_MOTOR_SPEED
.global READ_SONAR
.global GET_TIME
.global SET_TIME

SET_MOTOR_SPEED:
    push {r7, lr}

    @carrega em r7 o id do motor
    ldrb r7, [r0]

    @Lembrar de verificar se a ordem dos motores esta certa

    @ empilha os valores de id e velocidade para syscall
    push {r1,r2}            

    ldrb r1, [r1, #1]
    ldrb r0, [r0, #1]

    @chamada da syscall
    mov r7, #20  
    svc 0x0                       

    pop {r7, pc}

READ_SONAR:
    push {r7, lr}        

    @verifica se o id do sonar eh valido
    cmp r7, #15					
    bgt read_sonar_end

    cmp r7, #0						
    blt read_sonar_end

    @chamada da syscall
    push {r0}                 
    mov r7, #21			
	svc 0x0

    pop {r7, pc} 

    @caso em que erro sonar > 15 ou sonar < 0
    read_sonar_end:
        mov r0, #-1
        pop {r7, pc}   

GET_TIME:
    push {r4-r11, lr}        
    mov r1, r0  
    @chamada da syscall                    
    mov r7, #17                    
    svc 0x0
    str r0, [r1]                   
    pop {r4-r11, pc}        


SET_TIME:
    push {r4-r11, lr}      
    @chama da syscall
    push {r0}                 
    mov r7, #18                    
    svc 0x0
    pop {r4-r11, pc}         
