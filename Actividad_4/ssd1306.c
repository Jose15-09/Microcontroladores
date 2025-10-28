/*
 * Librería SSD1306 para STM32F10x sin HAL
 * Basado en el código de Controllerstech:
 * https://controllerstech.com/oled-display-using-i2c-stm32/
 * Adaptado para usar librerías i2c.c y delay.c proporcionadas.
 * Fuente: Controllerstech (adaptado)
 */
#include "ssd1306.h"
#include "i2c.h"  
#include "delay.h"
#include "fonts.h"
#include <string.h> 

// --- Definiciones Internas ---
// Buffer para la pantalla (128 columnas * (64 filas / 8 bits por byte) = 1024 bytes)
static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

// Estructura para el manejo interno del display
typedef struct {
    uint8_t CurrentX;
    uint8_t CurrentY;
    uint8_t Initialized;
} SSD1306_t;

static SSD1306_t SSD1306;

// --- Comandos SSD1306 ---
#define SSD1306_CMD_MEM_MODE         0x20 // Set Memory Addressing Mode
#define SSD1306_CMD_COL_ADDR         0x21 // Set Column Address
#define SSD1306_CMD_PAGE_ADDR        0x22 // Set Page Address
#define SSD1306_CMD_SET_START_LINE   0x40 // Set Display Start Line (0x40 | line=0..63)
#define SSD1306_CMD_CONTRAST         0x81 // Set Contrast Control
#define SSD1306_CMD_CHARGE_PUMP      0x8D // Set Charge Pump Setting
#define SSD1306_CMD_SEG_REMAP_OFF    0xA0 // Segment Re-map OFF (column 0 to SEG0)
#define SSD1306_CMD_SEG_REMAP_ON     0xA1 // Segment Re-map ON (column 127 to SEG0)
#define SSD1306_CMD_ENTIRE_DISP_OFF  0xA4 // Resume to RAM content display (RESET)
#define SSD1306_CMD_ENTIRE_DISP_ON   0xA5 // Entire display ON, Output ignores RAM content
#define SSD1306_CMD_NORMAL_DISP      0xA6 // Normal display (0 = OFF, 1 = ON)
#define SSD1306_CMD_INVERSE_DISP     0xA7 // Inverse display (0 = ON, 1 = OFF)
#define SSD1306_CMD_MULTIPLEX_RATIO  0xA8 // Set Multiplex Ratio
#define SSD1306_CMD_DISPLAY_OFF      0xAE // Set display OFF
#define SSD1306_CMD_DISPLAY_ON       0xAF // Set display ON
#define SSD1306_CMD_COM_SCAN_INC     0xC0 // COM Output Scan Direction normal (RESET)
#define SSD1306_CMD_COM_SCAN_DEC     0xC8 // COM Output Scan Direction remapped
#define SSD1306_CMD_DISPLAY_OFFSET   0xD3 // Set Display Offset
#define SSD1306_CMD_SET_CLOCK_DIV    0xD5 // Set Display Clock Divide Ratio/Oscillator Frequency
#define SSD1306_CMD_PRECHARGE_PERIOD 0xD9 // Set Pre-charge Period
#define SSD1306_CMD_COM_PINS_CONFIG  0xDA // Set COM Pins Hardware Configuration
#define SSD1306_CMD_VCOMH_DESELECT   0xDB // Set VCOMH Deselect Level
//----------------------------------------------------------------------

// --- Funciones Privadas ---

// Envía un comando al SSD1306
static void ssd1306_send_command(uint8_t command) {
    uint8_t data_t[2];
    data_t[0] = 0x00;
    data_t[1] = command;
    write_i2c(SSD1306_I2C_ADDR, data_t, 2);
}

// Envía un buffer de datos al SSD1306
static void ssd1306_send_data_buffer(uint8_t* buffer, uint16_t size) {
    uint8_t address_shifted = SSD1306_I2C_ADDR << 1; // Dirección + bit W(0)
    uint16_t i;

    start_i2c();
    adress_i2c(address_shifted, 0); // Enviar dirección + W

    // Enviar byte de control para indicar modo Datos (0x40)
    data_i2c(0x40);

    // Enviar los datos del buffer
    for (i = 0; i < size; i++) {
        data_i2c(buffer[i]);
    }

    stop_i2c();
}

// --- Funciones Públicas ---

// Inicialización del display SSD1306
void ssd1306_init(void) {
    delay_ms(150);

    // Secuencia de inicialización (basada en datasheet)
    ssd1306_send_command(SSD1306_CMD_DISPLAY_OFF);

    ssd1306_send_command(SSD1306_CMD_SET_CLOCK_DIV);
    ssd1306_send_command(0x80);

    ssd1306_send_command(SSD1306_CMD_MULTIPLEX_RATIO);
    ssd1306_send_command(SSD1306_HEIGHT - 1);

    ssd1306_send_command(SSD1306_CMD_DISPLAY_OFFSET);
    ssd1306_send_command(0x00);

    ssd1306_send_command(SSD1306_CMD_SET_START_LINE | 0x0);

    ssd1306_send_command(SSD1306_CMD_CHARGE_PUMP); 
    ssd1306_send_command(0x14); 

    ssd1306_send_command(SSD1306_CMD_MEM_MODE);   
    ssd1306_send_command(0x00);
    ssd1306_send_command(SSD1306_CMD_SEG_REMAP_ON);
    ssd1306_send_command(SSD1306_CMD_COM_SCAN_DEC); 

    ssd1306_send_command(SSD1306_CMD_COM_PINS_CONFIG);
    ssd1306_send_command(0x12);

    ssd1306_send_command(SSD1306_CMD_CONTRAST); 
    ssd1306_send_command(0xCF);  

    ssd1306_send_command(SSD1306_CMD_PRECHARGE_PERIOD);
    ssd1306_send_command(0xF1);  

    ssd1306_send_command(SSD1306_CMD_VCOMH_DESELECT); 
    ssd1306_send_command(0x40); 

    ssd1306_send_command(SSD1306_CMD_ENTIRE_DISP_OFF);

    ssd1306_send_command(SSD1306_CMD_NORMAL_DISP); 

    // Limpiamos el buffer local antes de encender
    ssd1306_fill(0);
    // Enviamos el buffer limpio a la pantalla
    ssd1306_update_screen();

    // Encender pantalla
    ssd1306_send_command(SSD1306_CMD_DISPLAY_ON);

    // Marcamos como inicializado y reseteamos cursor
    SSD1306.Initialized = 1;
    SSD1306.CurrentX = 0;
    SSD1306.CurrentY = 0;

    delay_ms(10); 
}

// Llena el buffer con un color (0 = Negro, 1 = Blanco)
void ssd1306_fill(uint8_t color) {

    memset(SSD1306_Buffer, (color == 0) ? 0x00 : 0xFF, sizeof(SSD1306_Buffer));
    SSD1306.CurrentX = 0;
    SSD1306.CurrentY = 0;
}

// Envía el buffer local completo a la memoria RAM de la pantalla
void ssd1306_update_screen(void) {
    if (!SSD1306.Initialized) return;

    // Establecer rango de páginas y columnas para cubrir toda la pantalla
    ssd1306_send_command(SSD1306_CMD_PAGE_ADDR);
    ssd1306_send_command(0); 
    ssd1306_send_command(7);
    ssd1306_send_command(SSD1306_CMD_COL_ADDR);
    ssd1306_send_command(0);
    ssd1306_send_command(SSD1306_WIDTH - 1); 

    // Enviar todo el buffer
    ssd1306_send_data_buffer(SSD1306_Buffer, sizeof(SSD1306_Buffer));
}

// Dibuja un pixel en el buffer local
void ssd1306_draw_pixel(uint8_t x, uint8_t y, uint8_t color) {
    // Verificar límites
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
        return;
    }

    // Calcular índice y bit dentro del buffer
    // El buffer está organizado por páginas horizontales. Cada página tiene 8 filas.
    // Buffer[columna + (pagina * ancho)]
    // Pagina = y / 8
    // Bit dentro del byte = y % 8
    uint16_t index = x + (y / 8) * SSD1306_WIDTH;
    uint8_t bit_pos = y % 8;

    // Modificar el bit correspondiente
    if (color == 1) { 
        SSD1306_Buffer[index] |= (1 << bit_pos);
    } else { 
        SSD1306_Buffer[index] &= ~(1 << bit_pos);
    }
}

// Establece la posición del cursor para texto
void ssd1306_set_cursor(uint8_t x, uint8_t y) {
    SSD1306.CurrentX = x;
    SSD1306.CurrentY = y;
}

// Dibuja un caracter ASCII en la posición actual del cursor
// Asume que las fuentes en fonts.c tienen formato vertical, LSB arriba.
// Para fuentes > 8px de alto, asume que los bytes por columna están consecutivos.
char ssd1306_draw_char(char ch, uint8_t Font_Size, uint8_t color) {
    const uint8_t* font_data;
    uint8_t font_width;
    uint8_t font_height;
    uint8_t bytes_per_col; 

    // Seleccionar la fuente y sus dimensiones
    if (Font_Size == 7) { // 7x10
        font_data = Font7x10;
        font_width = 7;
        font_height = 10;
    }

    else { 
        font_data = Font7x10;
        font_width = 7;
        font_height = 10;
    }

    // Calcular cuántos bytes ocupa cada columna verticalmente
    bytes_per_col = (font_height + 7) / 8;

    // Verificar si el caracter está en el rango soportado (ASCII 32 a 126)
    if (ch < ' ' || ch > '~') {
        ch = '?'; 
    }

    // Calcular el offset al inicio de los datos del caracter
    // Offset = (ASCII - 32) * (Ancho * Bytes_por_columna)
    uint16_t char_offset = (ch - 32) * font_width * bytes_per_col;
    const uint8_t* char_data = &font_data[char_offset];

    // Verificar si cabe horizontalmente
    if (SSD1306.CurrentX + font_width > SSD1306_WIDTH) {
        SSD1306.CurrentX = 0; 
        SSD1306.CurrentY += font_height; 
        // Verificar si cabe verticalmente después del salto
        if (SSD1306.CurrentY + font_height > SSD1306_HEIGHT) {
            return 0; 
        }
    }
     // Verificar si cabe verticalmente en la posición actual
    if (SSD1306.CurrentY + font_height > SSD1306_HEIGHT) {
        return 0; 
    }


    // Dibujar el caracter columna por columna
    uint8_t current_byte;
    uint8_t current_bit;
    for (uint8_t col = 0; col < font_width; col++) { 
        for (uint8_t byte_idx = 0; byte_idx < bytes_per_col; byte_idx++) { 
            current_byte = char_data[col * bytes_per_col + byte_idx];
            for (uint8_t bit_idx = 0; bit_idx < 8; bit_idx++) { 
                current_bit = (current_byte >> bit_idx) & 0x01;
                uint8_t pixel_y = SSD1306.CurrentY + (byte_idx * 8) + bit_idx;
                // Dibujar solo si el bit corresponde a una fila válida del caracter
                if (pixel_y < SSD1306.CurrentY + font_height) {
                    if (current_bit) {
                        ssd1306_draw_pixel(SSD1306.CurrentX + col, pixel_y, color);
                    } else {
                        // Si el color es blanco, el fondo es negro. Si color=negro, fondo blanco.
                        ssd1306_draw_pixel(SSD1306.CurrentX + col, pixel_y, !color);
                    }
                }
            }
        }
    }

    // Mover cursor para el siguiente caracter (añadir un espacio pequeño)
    SSD1306.CurrentX += font_width + 1; 
    return ch; // Éxito
}

// Dibuja una cadena de texto
void ssd1306_draw_string(char* str, uint8_t Font_Size, uint8_t color) {
    if (!SSD1306.Initialized) return;
    while (*str) {
        if (ssd1306_draw_char(*str, Font_Size, color) == 0) {
            // Error al dibujar o se quedó sin espacio
            return;
        }
        str++;
    }
}

// Dibuja un bitmap monocromático (formato horizontal, 1 bit por pixel)
void ssd1306_draw_bitmap(uint8_t x, uint8_t y, const unsigned char* bitmap, uint16_t w, uint16_t h, uint8_t color) {
    if (!SSD1306.Initialized) return;

    int16_t byteWidth = (w + 7) / 8; 
    uint8_t byte = 0;

    for (uint16_t j = 0; j < h; j++, y++) {
        for (uint16_t i = 0; i < w; i++) {
            // Leer el byte del bitmap solo cuando empezamos una nueva columna de 8 bits
            if (i % 8 == 0) {
                byte = bitmap[j * byteWidth + i / 8];
            }
            // Verificar el bit más significativo y dibujar el píxel
            if (byte & 0x80) {
                ssd1306_draw_pixel(x + i, y, color);
            } else {
                ssd1306_draw_pixel(x + i, y, !color); // Dibujar fondo
            }
            // Desplazar el byte para el siguiente píxel
            byte <<= 1;
        }
    }
}