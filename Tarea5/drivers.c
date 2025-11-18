#include <stm32f10x.h>
#include "drivers.h"

/*******************************************************
 *                 USART1 DRIVERS
 *******************************************************/

/*
 * usart1_init
 * -------------------------------------------------------
 * Configura USART1 en:
 *   - 9600 baudios
 *   - 8 bits
 *   - Sin paridad
 *   - 1 stop
 *
 * Configura pines:
 *   PA9  ? TX (AF push-pull 50 MHz)
 *   PA10 ? RX (entrada pull-up)
 *
 * Requisitos:
 *   El sistema corre a 72 MHz (BRR = 7500)
 */
void usart1_init(void)
{
    // Habilitar reloj de GPIOA, USART1 y AFIO
    RCC->APB2ENR |= (1 << 14) | (1 << 2) | (1 << 0);

    // --- Configurar RX (PA10) ---
    GPIOA->CRH &= ~((1 << 9) | (1 << 8)); // Modo entrada
    GPIOA->CRH |=  (1 << 11);             // Entrada pull-up/down
    GPIOA->ODR  |=  (1 << 10);            // Activar pull-up

    // --- Configurar TX (PA9) ---
    GPIOA->CRH &= ~(0xF << 4);
    GPIOA->CRH |=  (0xB << 4);            // AF push-pull, 50 MHz

    // Activar USART1, TX y RX
    USART1->CR1 |= (1 << 13) | (1 << 3) | (1 << 2);

    // 9600 bps para 72 MHz
    USART1->BRR = 7500;
}

/*
 * usart1_sendbyte
 * -------------------------------------------------------
 * Envía un único byte por USART1.
 *
 * Espera a que la transmisión termine (TC = Transmission Complete)
 * antes de continuar.
 */
void usart1_sendbyte(uint8_t c)
{
    USART1->DR = c;
    while ((USART1->SR & (1 << 6)) == 0);  // Esperar TC
    USART1->SR &= ~(1 << 6);               // Limpiar TC
}

/*******************************************************
 *                 FOQUITO (LED) DRIVERS
 *******************************************************/

/*
 * foquito_init
 * -------------------------------------------------------
 * Inicializa PC13 como salida a 10 MHz.
 * Este pin controla el LED de la placa Blue Pill.
 * Deja el foquito APAGADO al iniciar.
 */
void foquito_init(void)
{
    // Activar reloj de GPIOC
    RCC->APB2ENR |= (1 << 4);

    // PC13 = salida 10 MHz push-pull
    GPIOC->CRH &= ~(0xF << 20);
    GPIOC->CRH |=  (0x1 << 20);

    foquito_off();
}

/*
 * foquito_on
 * -------------------------------------------------------
 * Enciende el LED.
 * En la Blue Pill, el LED se activa escribiendo 0 (reset).
 */
void foquito_on(void)
{
    GPIOC->BSRR = (1 << 29);  // Reset bit 13
}

/*
 * foquito_off
 * -------------------------------------------------------
 * Apaga el LED.
 * En la Blue Pill el LED está en PC13 y se apaga escribiendo 1.
 */
void foquito_off(void)
{
    GPIOC->BSRR = (1 << 13);  // Set bit 13
}

/*
 * foquito_toggle
 * -------------------------------------------------------
 * Cambia el estado del foquito:
 *   Si está encendido ? lo apaga
 *   Si está apagado  ? lo enciende
 *
 * Se basa en el valor actual de PC13 (ODR).
 */
void foquito_toggle(void)
{
    if (GPIOC->ODR & (1 << 13))
        foquito_on();
    else
        foquito_off();
}