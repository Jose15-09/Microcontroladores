#include <stdint.h>
#include "logic.h"
#include "drivers.h"
#include "core.h"

#define CANTIDAD 200

// Buffers que almacenan los 200 números generados
char bufA[CANTIDAD];
char bufB[CANTIDAD];

// Semillas para los generadores pseudo-aleatorios
static unsigned int semA = 12345;
static unsigned int semB = 54321;

/*******************************************************
 *                GENERADORES PSEUDOALEATORIOS
 *******************************************************/

/*
 * generarA
 * -------------------------------------------------------
 * Llena bufA[] con 200 dígitos (carácter '0'–'9').
 *
 * Usa un generador tipo LCG:
 *      x = x * 3 + 7
 *
 * El resultado x % 10 se convierte en un carácter.
 */
void generarA(void)
{
    unsigned int x = semA;

    for (int i = 0; i < CANTIDAD; i++) {
        x = x * 3 + 7;
        bufA[i] = '0' + (x % 10);
    }

    semA = x;   // Guardar nuevo estado
}

/*
 * generarB
 * -------------------------------------------------------
 * Igual que generarA pero con otro LCG y otra semilla.
 *
 *      x = x * 5 + 11
 */
void generarB(void)
{
    unsigned int x = semB;

    for (int i = 0; i < CANTIDAD; i++) {
        x = x * 5 + 11;
        bufB[i] = '0' + (x % 10);
    }

    semB = x;
}

/*******************************************************
 *                      MODO A
 *******************************************************/

/*
 * modoA (antes metodo1)
 * -------------------------------------------------------
 * - Enciende el foquito de forma fija.
 * - Envía por UART un mensaje de inicio.
 * - Envía los 200 caracteres de bufA.
 * - Envía mensaje final.
 *
 * No hace parpadeos ni retardos.
 */
void modoA(void)
{
    foquito_on();   // LED encendido fijo

    const char *txt = "\r\nModo A\r\n";
    while (*txt) usart1_sendbyte(*txt++);

    // Enviar los 200 dígitos
    for (int i = 0; i < CANTIDAD; i++)
        usart1_sendbyte(bufA[i]);

    const char *fin = "\r\nFin A\r\n";
    while (*fin) usart1_sendbyte(*fin++);
}

/*******************************************************
 *                      MODO B
 *******************************************************/

/*
 * modoB (antes metodo2)
 * -------------------------------------------------------
 * - Apaga foquito al inicio.
 * - Envía mensaje de inicio.
 * - Envía bufB completo, pero:
 *        Cada 5 caracteres ? salto de línea + parpadeo.
 * - Parpadeo con retardo de 60 ms.
 * - Envía mensaje final y apaga foquito.
 */
void modoB(void)
{
    const char *txt = "\r\nModo B\r\n";
    while (*txt) usart1_sendbyte(*txt++);

    foquito_off();

    for (int i = 0; i < CANTIDAD; i++) {

        usart1_sendbyte(bufB[i]);

        // Cada 5 dígitos ? salto de línea + toggle
        if ((i % 5) == 4) {
            usart1_sendbyte('\r');
            usart1_sendbyte('\n');

            foquito_toggle();
            delay_ms(60);
        }
    }

    const char *fin = "\r\nFin B\r\n";
    while (*fin) usart1_sendbyte(*fin++);

    foquito_off(); // Estado final
}