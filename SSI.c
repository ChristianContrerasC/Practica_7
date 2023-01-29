
#include "lib/include.h"
extern void Configurar_SSI2(void)
{
    
    SYSCTL->RCGCSSI |= (1<<2); //ACTIVAR MODULO 2 SSI2
    SYSCTL->RCGCGPIO |= (1<<3);//Puerto B
    GPIOD->DIR |= (0<<4) | (1<<5) | (0<<6) | (1<<7) ; //selector es salida = 1
    GPIOD->AFSEL = (1<<4)|(1<<5)|(1<<6)|(1<<7); //Pines del 4 al 7
    GPIOD->PCTL = (GPIOD->PCTL&0xFF0000FF) | 0x00222200; // tabla p.688
    
    GPIOD->DEN |= (1<<4)|(1<<5)|(1<<6)|(1<<7); // DIGITAL ENABLE DE DICHOS PINES
    //                MISO    MOSI    CS    SCLK

   // GPIOD_AHB->DATA |= (1<<5); //registrar CS
    //GPIOB->PUR |= (0<<7)|(0<<6)|(0<<5)|(0<<4);
    //GPIOB->PDR |= (0<<7)|(0<<6)|(0<<5)|(0<<4);
    //GPIOB->AMSEL |= (0<<7)|(0<<6)|(0<<5)|(0<<4);
    
    SSI2->CR1 = (0<<1); //SSE=0 deshabilitar modulo
    SSI2->CR1 = (0<<2); //MS = 0 modo maestro
    SSI2->CC = (0x9<<0); //system clock = 50MHz
    //SSInClk = SysClk / (CPSDVSR * (1 + SCR))
    // 4MHz = 20MHz / 1*(1 + 4) // NUESTRA PROPUESTA PARA 4 MHz
    //2 500 000 = 50 000 000/(2*(1+SCR))
    // SCR = (50 000 000/2 500 000*2) - 1 = 9
    SSI2->CPSR =0x1; //NUMERO QUE ASIGNO EL PROFE CPSDVSR
    SSI2->CR0 = (0x4<<8) | (0x1<<6) | (0x1<<4) | (0xB<<0); // datos de 8 bits  // 0x4 Viene del SCR que vale 4 luego en el segundo argumento 0x1 es nuetsro modulo de polaridad y El 0xB es el numero de bits a leer que seria 12
    SSI2->CR1 |= (1<<1); //SSE=1 habilitar modoulo p.961 (0x02)
}

extern void SPI_write(uint8_t data)
{
    while (SSI2->SR & 0x2)
    {
        SSI2->DR = (uint16_t)data;
    }
    
}

extern void SPI_write_data(uint8_t reg, uint8_t data)
{
    GPIOD->DATA &= ~(1<<3); // CS = 0 se niega
    SPI_write(reg & ~0x80); //escribir registro + MSB igualado a cero
    SPI_write(data);
    GPIOD->DATA |= (1<<3); //CS = 1
}

extern uint8_t SPI_read(void)
{
    uint8_t data = 0;
    while ((SSI2->SR & 0x10) == 0x10); // espera por busy bit
    data = SSI2->DR;
    return data;
}
// LECTURA DE DATOS
extern uint8_t SPI_read_data(uint8_t reg)
{
    uint8_t data = 0;
    GPIOD->DATA &= ~(1<<3); // CS = 0
    SPI_write(reg | 0x80); // escribe registro + MSB
    SPI_write(0x00); //escribir dato para generar señal de reloj
    data = SPI_read(); //leer dato
    GPIOD->DATA |= (1<<3); //CS = 1
    return data;
}
