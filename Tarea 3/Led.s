; --- Definiciones ---
        AREA    Constantes, DATA, READONLY   ; Área de datos de solo lectura
RCC_APB2ENR     EQU     0x40021018           ; Registro de habilitación de reloj (APB2)
GPIOC_CRL       EQU     0x40011000           ; Configuración baja de puerto C
GPIOC_CRH       EQU     0x40011004           ; Configuración alta de puerto C
GPIOC_IDR       EQU     0x40011008           ; Datos de entrada del puerto C
GPIOC_ODR       EQU     0x4001100C           ; Datos de salida del puerto C
GPIOC_BSRR      EQU     0x40011010           ; Registro de set/reset
GPIOC_BRR       EQU     0x40011014           ; Registro de reset
SRAM_BASE       EQU     0x20000100           ; Memoria base para almacenamiento
FLAG_GENERATED  EQU     0x20000000           ; Bandera de estado de números generados

        AREA    act3, CODE, READONLY
        ENTRY
        EXPORT  __main

; --- Inicio del programa ---
__main
        ; Encender reloj del puerto C
        LDR     R1, =RCC_APB2ENR
        MOV     R0, #0x10
        STR     R0, [R1]
        ; Configurar PC0 y PC1 como entradas (pull-up/down)
        LDR     R1, =GPIOC_CRL
        LDR     R0, [R1]
        BIC     R0, R0, #(0x0F + 0xF0)   ; Limpia campos de PC0 y PC1
        STR     R0, [R1]
        ; Configurar PC13 como salida push-pull
        LDR     R1, =GPIOC_CRH
        LDR     R0, [R1]
        BIC     R0, R0, #0x00F00000
        ORR     R0, R0, #0x00200000
        STR     R0, [R1]
        ; LED apagado inicialmente
        LDR     R1, =GPIOC_BRR
        MOV     R0, #0x2000
        STR     R0, [R1]
        B       estado_inicio
; --- Estado: Inicio ---
estado_inicio
        ; LED apagado
        LDR     R1, =GPIOC_BRR
        MOV     R0, #0x2000
        STR     R0, [R1]
        ; Reset de bandera
        LDR     R1, =FLAG_GENERATED
        MOV     R0, #0
        STR     R0, [R1]
        B       leer_entrada
; --- Lectura de entradas ---
leer_entrada
        LDR     R1, =GPIOC_IDR
        LDR     R0, [R1]
        AND     R0, R0, #0x03
        CMP     R0, #0x00
        BEQ     estado_inicio
        CMP     R0, #0x01
        BEQ     estado_opcion1
        CMP     R0, #0x02
        BEQ     estado_opcion2
        B       leer_entrada
; --- Opción 1: Generar números ---
estado_opcion1
        ; LED encendido
        LDR     R1, =GPIOC_BSRR
        MOV     R0, #0x2000
        STR     R0, [R1]
        ; Bandera en 1
        LDR     R1, =FLAG_GENERATED
        MOV     R0, #1
        STR     R0, [R1]
        ; Generador simple de 100 números
        LDR     R2, =SRAM_BASE
        MOV     R3, #0
        MOV     R4, #12345
        MOV     R5, #12345
genera_loop
        LSL     R4, R4, #5
        ADD     R4, R4, R5
        AND     R4, R4, #0xFF
        STRB    R4, [R2], #1
        ADD     R3, R3, #1
        CMP     R3, #100
        BNE     genera_loop
espera_op1
        LDR     R1, =GPIOC_IDR
        LDR     R0, [R1]
        AND     R0, R0, #0x03
        CMP     R0, #0x00
        BEQ     estado_inicio
        B       espera_op1
; --- Opción 2: Ordenar ---
estado_opcion2
        ; Verifica si ya se generaron datos
        LDR     R1, =FLAG_GENERATED
        LDR     R0, [R1]
        CMP     R0, #0
        BEQ     estado_inicio

        ; Ordenamiento selección
        LDR     R2, =SRAM_BASE
        MOV     R3, #99
outer_sort
        LDR     R4, =SRAM_BASE
        ADD     R4, R4, R3
        MOV     R5, R2
        MOV     R6, R2
inner_sort
        ADD     R6, R6, #1
        LDRB    R7, [R6]
        LDRB    R8, [R5]
        CMP     R7, R8
        BGE     sin_cambio
        MOV     R5, R6
sin_cambio
        CMP     R6, R4
        BNE     inner_sort
        ; Intercambio
        LDRB    R7, [R2]
        LDRB    R8, [R5]
        STRB    R8, [R2]
        STRB    R7, [R5]
        ADD     R2, R2, #1
        SUB     R3, R3, #1
        CMP     R3, #0
        BPL     outer_sort
        ; LED encendido y bandera activa
        LDR     R1, =GPIOC_BSRR
        MOV     R0, #0x2000
        STR     R0, [R1]
        LDR     R1, =FLAG_GENERATED
        MOV     R0, #1
        STR     R0, [R1]
espera_op2
        LDR     R1, =GPIOC_IDR
        LDR     R0, [R1]
        AND     R0, R0, #0x03
        CMP     R0, #0x00
        BEQ     estado_inicio
        CMP     R0, #0x02
        BEQ     espera_op2
        B       estado_opcion1
; --- Fin del programa ---
        END
