#include <stm32f10x.h>

void init_portc(void)
{
    RCC->APB2ENR |= 1 << 4; //activa reloj
    GPIOC->CRH |= 1 << 21;
    GPIOC->CRH &= ~(1 << 20);
    GPIOC->CRH &= ~((1 << 22)|(1 << 23));
}

void init_ports(void)
{
    //Enable clock for GPIOC, GPIOB and Alternate Function
    RCC->APB2ENR |= ((1 << 4) | (1 << 3) | (1 << 0));

    //use I2C ****************************
    //enable i2c-2 clock
    RCC->APB1ENR |= (1 << 22);
    //reset PB11 and PB10 configuration bits
    GPIOB->CRH &= ~((0xF << ( (11-8)*4 )) | (0xF << ( (10-8)*4 ))); // Reset bits para PB10 y PB11
    //set PB11 and PB10 as AF Output 50MHz Open-Drain
    // CNF = b11 (AF Open-drain), MODE = b11 (Output 50MHz)
    GPIOB->CRH |= ((0xB << ( (11-8)*4 )) | (0xB << ( (10-8)*4 ))); // Set AF OD 50MHz para PB10 y PB11

    //use of LED ****************************
    //configure PC13 pin as output push-pull maximum speed 10MHz
    // CNF = b00 (GP Output PP), MODE = b01 (Output 10MHz)
    GPIOC->CRH &= ~(0xF << ( (13-8)*4 )); // Reset bits para PC13
    GPIOC->CRH |=  (0x1 << ( (13-8)*4 )); // Set GP PP 10MHz para PC13
    // Inicialmente apagado (Set pin high - active low)
    GPIOC->ODR |= (1 << 13);

    //use of EXTI ***************************
    //configure PB0 pin as input pull up or pull down
    // CNF = b10 (Input PU/PD), MODE = b00 (Input mode)
    GPIOB->CRL &= ~(0xF << (0*4)); // Reset bits para PB0
    GPIOB->CRL |=  (0x8 << (0*4)); // Set Input PU/PD para PB0
    //activate PB0 pull up (set ODR bit high)
    GPIOB->ODR |= (1 << 0);
}