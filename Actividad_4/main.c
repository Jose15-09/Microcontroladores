#include "gpio.h"
#include "ext_in.h"
#include "delay.h"   
#include "i2c.h"     
#include "ssd1306.h" 

int main(void)
{
    // === Configuración inicial ===
    delay_init();          // Activar temporizador SysTick
    init_ports();          // Configurar GPIO, I2C y EXTI
    init_i2c();            // Preparar interfaz I2C2
    ssd1306_init();        // Encender e inicializar pantalla OLED

    // === Preparar pantalla ===
    ssd1306_fill(0);       // Limpiar buffer de video

    // Mostrar texto principal
    ssd1306_set_cursor(12, 10);
    ssd1306_draw_string("Jose Adrian", 7, 1);

    ssd1306_set_cursor(12, 28);
    ssd1306_draw_string("Actividad 4", 7, 1);

    // Aplicar los cambios a la pantalla
    ssd1306_update_screen();

    // === Bucle principal ===
    while (1)
    {
        // Encender LED (PC13 activo en bajo)
        GPIOC->ODR &= ~(1U << 13);
        delay_ms(400);

        // Apagar LED
        GPIOC->ODR |= (1U << 13);
        delay_ms(400);
    }
}
