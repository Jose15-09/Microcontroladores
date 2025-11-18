#include <stm32f10x.h>
#include <stdint.h>

#include "drivers.h"
#include "logic.h"
#include "core.h"

/*
 * delay_ms
 * -------------------------------------------------------
 * Genera un retardo aproximado en milisegundos.
 * Utiliza ciclos vacíos; NO depende de SysTick.
 *
 * Parámetro:
 *   t ? tiempo en milisegundos
 *
 * Nota:
 *   El valor 6000 fue calibrado para un sistema a 72 MHz.
 */
void delay_ms(uint16_t t)
{
    volatile unsigned long i;
    while (t--) {
        for (i = 0; i < 6000; i++) {
            // bucle vacío
        }
    }
}

/*
 * main
 * -------------------------------------------------------
 * Punto de entrada principal del programa.
 * - Inicializa el foquito (led), USART1 y los generadores
 * - Ejecuta dos modos de operación (modoA y modoB)
 * - Queda en un bucle infinito tras terminar.
 */
int main(void)
{
    foquito_init();   // Inicializar GPIO para el LED
    usart1_init();    // Inicializar periférico USART1

    generarA();       // Llenar bufA con 200 dígitos
    generarB();       // Llenar bufB con 200 dígitos

    modoA();          // Enviar bufA por UART + encender foquito

    delay_ms(1500);   // Pausa entre modos

    modoB();          // Enviar bufB + parpadear foquito

    while (1) {
        // Estado final del programa
    }
}