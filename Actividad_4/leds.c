#include <stm32f10x.h>
#include "delay.h"

// Función invocada por la interrupción definida en ext_in.c
void led_on_off(void)
{
    int x;

    // Realiza tres parpadeos del LED
    for (x = 0; x < 3; x++)
    {
        // Encender LED (activo en bajo)
        GPIOC->ODR &= ~(1 << 13);
        delay_ms(100);

        // Apagar LED
        GPIOC->ODR |= (1 << 13);
        delay_ms(100);
    }

    // Dejar el LED apagado (nivel alto)
    GPIOC->ODR |= (1 << 13);
}
