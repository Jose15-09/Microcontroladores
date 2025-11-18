#ifndef DRIVERS_H
#define DRIVERS_H

#include <stdint.h>

/************ USART1 ************/
void usart1_init(void);         // Configura USART1 a 9600 bps
void usart1_sendbyte(uint8_t c); // Envía un byte

/************ FOQUITO (LED) ************/
void foquito_init(void);   // Configura PC13 para el LED
void foquito_on(void);     // Enciende LED
void foquito_off(void);    // Apaga LED
void foquito_toggle(void); // Cambia estado del LED

#endif