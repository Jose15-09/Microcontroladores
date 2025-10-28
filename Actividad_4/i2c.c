#include "stm32f10x.h"
#include "i2c.h"

// Configura el periférico I2C2 para operar a 100 kHz (modo estándar, PCLK1 = 8 MHz)
void init_i2c(void)
{
    /*
        Nota:
        - La habilitación de relojes y la configuración de los pines PB10/PB11
          se realiza previamente en gpio.c.
        - Este bloque se encarga únicamente de la configuración interna del I2C2.
    */

    // Reiniciar I2C2 para garantizar un estado limpio antes de la configuración
    I2C2->CR1 |= I2C_CR1_SWRST;    // Activar bit de software reset
    I2C2->CR1 &= ~I2C_CR1_SWRST;   // Liberar reset para continuar con la inicialización

    // Ajustar frecuencia del bus APB1 (en MHz)
    I2C2->CR2 = 8;

    // Configurar el modo estándar (100 kHz)
    I2C2->CCR &= ~I2C_CCR_FS;      // Seleccionar modo estándar
    I2C2->CCR = 40;                // CCR = 40 para fPCLK1 = 8 MHz ? SCL = 100 kHz

    // Establecer tiempo máximo de subida
    // TRISE = (t_rise_max / t_PCLK1) + 1 = (1000 ns / 125 ns) + 1 = 9
    I2C2->TRISE = 9;

    // Activar el periférico I2C2
    I2C2->CR1 |= I2C_CR1_PE;
}

// Genera la condición START en el bus I2C
void start_i2c(void)
{
    // Habilitar ACK y generar condición de inicio
    I2C2->CR1 |= I2C_CR1_ACK;
    I2C2->CR1 |= I2C_CR1_START;

    // Esperar hasta que se establezca el flag de inicio (SB = 1)
    while (!(I2C2->SR1 & I2C_SR1_SB));
}

// Genera la condición STOP en el bus I2C
void stop_i2c(void)
{
    // Emitir señal de parada (STOP)
    I2C2->CR1 |= I2C_CR1_STOP;
}

// Envía la dirección del esclavo con el bit R/W

void adress_i2c(uint8_t address_shifted, uint8_t r_w) // address_shifted ya es addr<<1
{
    // Enviar dirección (ya shifteada) + bit R/W
    I2C2->DR = address_shifted | r_w;
    // Esperar hasta que el flag ADDR (Address sent) se active (EV6)
    while(!(I2C2->SR1 & I2C_SR1_ADDR));

    // Limpiar el flag ADDR leyendo SR1 y luego SR2

    volatile uint32_t temp = I2C2->SR1 | I2C2->SR2;
    (void)temp;
}

// Envía un byte de datos al esclavo
void data_i2c(uint8_t data)
{
 
    while(!(I2C2->SR1 & I2C_SR1_TXE));
  
    I2C2->DR = data;

    while(!(I2C2->SR1 & I2C_SR1_BTF));
}

// Función completa para escribir datos vía I2C
// address_7bit: Dirección de 7 bits del esclavo (sin shiftear)
void write_i2c(uint8_t address_7bit, uint8_t data[], uint64_t length)
{
    uint64_t i = 0;
    uint8_t address_shifted = address_7bit << 1;

    start_i2c();
    adress_i2c(address_shifted, 0);

    for(i = 0; i < length; i++)
    {
        data_i2c(data[i]);
    }
    stop_i2c();
}