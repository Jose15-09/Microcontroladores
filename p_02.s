; Definición de registros
number  rn r0     ; número límite (n)
count   rn r1     ; índice de ciclo (i)
temp    rn r2     ; puntero a memoria


; Área de datos
; ----------------
        area    datos, data, readwrite
n       dcd     9              ; Variable con el valor límite de la serie


; Área de programa
; ----------------
        area    l_fibo, code, readonly
        entry
        export  __main

__main
        ; Cargar dirección de inicio de la SRAM
        ldr     temp, =0x20000000

        ; Cargar el valor de n desde la variable
        ldr     number, =n
        ldr     number, [number]

        ; Comprobar si n < 0
        cmp     number, #0
        blt     fin             ; Si n es negativo, terminar.

        ; Manejar F(0)
        mov     r3, #0
        str     r3, [temp], #4  ; Guardar F(0) = 0
        
        cmp     number, #0
        beq     fin             ; Si n = 0, fin

        ; Manejar F(1)
        mov     r3, #1
        str     r3, [temp], #4  ; Guardar F(1) = 1

        cmp     number, #1
        beq     fin             ; Si n = 1, fin
        
        ; n >= 2
        mov     count, #2

ciclo
        ldr     r3, [temp, #-4] ; F(i-1)
        ldr     r4, [temp, #-8] ; F(i-2)
        add     r3, r3, r4      ; F(i) = F(i-1) + F(i-2)
        str     r3, [temp], #4  ; Guardar F(i)
        add     count, count, #1
        cmp     count, number
        ble     ciclo

fin
        b       fin
        end
;Jose Adrian Flores Mondragon
