#include <stm32f10x.h>
#include "leds.h"

/*
   Configuración: PB0 como entrada para la interrupción externa (EXTI0).
   Cada flanco ascendente genera una interrupción que ejecuta una rutina
   encargada de hacer parpadear el LED en PC13.
*/

volatile int8_t int0_flag = 0; // Variable reservada (sin uso actual)

// --- Rutina de servicio de interrupción para la línea EXTI0 ---
void EXTI0_IRQHandler(void)
{
    // Comprobar si la línea 0 generó la interrupción
    if (EXTI->PR & (1 << 0))
    {
        // Borrar el bit pendiente (se limpia escribiendo un 1)
        EXTI->PR |= (1 << 0);

        // Ejecutar la acción principal: parpadear el LED
        led_on_off();
    }
}

// --- Configuración de la interrupción externa (EXTI0) en PB0 ---
void init_exti(void)
{
    /*
        Información de referencia:
        - PB0 corresponde a la línea EXTI0.
        - Configuración mediante el registro AFIO->EXTICR[0].
        - Bits EXTI0[3:0] definen el puerto fuente.
    */

    // Asignar PB0 como entrada para EXTI0
    AFIO->EXTICR[0] &= ~(0x000F);               // Limpia bits previos
    AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PB;   // Selecciona Puerto B (valor 0001)

    // Configurar activación por flanco de subida
    EXTI->RTSR |= (1 << 0);   // Rising trigger habilitado
    EXTI->FTSR &= ~(1 << 0);  // Falling trigger deshabilitado

    // Habilitar la línea EXTI0 (sin enmascaramiento)
    EXTI->IMR |= (1 << 0);

    // Asegurarse de limpiar cualquier bandera pendiente
    EXTI->PR |= (1 << 0);

    // Activar la interrupción EXTI0 dentro del NVIC
    NVIC_EnableIRQ(EXTI0_IRQn);

    // (Opcional) Configurar prioridad mediante NVIC_SetPriority()
}
