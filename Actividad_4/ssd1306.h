#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>

// Direccion I2C del display SSD1306 (puede ser 0x3C o 0x3D)
#define SSD1306_I2C_ADDR         0x3C

// Dimensiones del display
#define SSD1306_WIDTH            128
#define SSD1306_HEIGHT           64

// --- Funciones principales ---
// Inicializa la comunicación I2C y la pantalla SSD1306.
void ssd1306_init(void);

// Envía el contenido del buffer interno a la memoria RAM de la pantalla.
void ssd1306_update_screen(void);

// Rellena todo el buffer interno con un color (0 = Negro, 1 = Blanco).
void ssd1306_fill(uint8_t color);

// Dibuja un píxel en el buffer interno en las coordenadas (x, y).
void ssd1306_draw_pixel(uint8_t x, uint8_t y, uint8_t color);

// Establece la posición del cursor (esquina superior izquierda) para el próximo carácter.
void ssd1306_set_cursor(uint8_t x, uint8_t y);

// Dibuja un carácter ASCII en la posición actual del cursor usando la fuente especificada.
// Retorna el carácter dibujado si tuvo éxito, 0 si no.
char ssd1306_draw_char(char ch, uint8_t Font_Size, uint8_t color);

// Dibuja una cadena de texto a partir de la posición actual del cursor.
void ssd1306_draw_string(char* str, uint8_t Font_Size, uint8_t color);

// Dibuja un mapa de bits monocromático en las coordenadas (x, y).
void ssd1306_draw_bitmap(uint8_t x, uint8_t y, const unsigned char* bitmap, uint16_t w, uint16_t h, uint8_t color);

#endif // SSD1306_H