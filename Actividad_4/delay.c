/**
  ******************************************************************************
  * @file		delay.c
  * @author	Yohanes Erwin Setiawan
  * @date		10 January 2016
  * @source https://github.com/controllerstech/STM32/tree/master/DELAY_SYSTICK
  ******************************************************************************
  */

#include "delay.h"

// Variable para almacenar la cuenta de ticks en microsegundos
static __IO uint32_t usTicks;

// Rutina de interrupción del SysTick - se llama cada 1 us
void SysTick_Handler()
{
    // Decrementa el contador si no es cero
    if (usTicks != 0)
    {
        usTicks--;
    }
}

// Inicializa y configura el temporizador SysTick
void delay_init()
{

    // Configura el SysTick para que interrumpa cada 1 microsegundo
    if (SysTick_Config(SystemCoreClock / 1000000))
    {
        while (1);
    }
}

// Función de retardo bloqueante en microsegundos
void delay_us(uint32_t us)
{
    // Carga el número de microsegundos a esperar
    usTicks = us;
    // Espera activa hasta que el contador llegue a cero
    while (usTicks);
}

// Función de retardo bloqueante en milisegundos
void delay_ms(uint32_t ms)
{
    // Espera hasta que ms llegue a cero
    while (ms--)
    {
        // Llama a delay_us para esperar 1000 microsegundos (1 ms)
        delay_us(1000);
    }
}

/********************************* END OF FILE ********************************/
/******************************************************************************/