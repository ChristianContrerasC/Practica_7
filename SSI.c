
#include "lib/include.h"
extern void Configurar_SSI2(void)
{
    // NOSOTROS TRABAJAMOS CON UN SENSOR MAX6675 QUE ES UN SENSOR DE TEMPERATURA QUE TRABAJA A 4.3 MHz y nosotros lo redondeamos a 4MHz
    SYSCTL->RCGCSSI |= (1<<2); //ACTIVAR MODULO 2 SSI2
    SYSCTL->RCGCGPIO |= (1<<1); //Puerto B
    GPIOB->DIR |= (0<<4) | (1<<5) | (0<<6) | (1<<7) ; //selector es salida = 1
    GPIOB->AFSEL = (1<<4)|(1<<5)|(1<<6)|(1<<7); //Pines del 4 al 7
    GPIOB->PCTL = (GPIOB->PCTL&0xFF0000FF) | 0x00222200; // tabla p.688
    
    GPIOB->DEN |= (1<<4)|(1<<5)|(1<<6)|(1<<7); // DIGITAL ENABLE DE DICHOS PINES
    //            MISO    MOSI    CS    SCLK

   // GPIOD_AHB->DATA |= (1<<5); //registrar CS
    //GPIOB->PUR |= (0<<7)|(0<<6)|(0<<5)|(0<<4);
    //GPIOB->PDR |= (0<<7)|(0<<6)|(0<<5)|(0<<4);
    //GPIOB->AMSEL |= (0<<7)|(0<<6)|(0<<5)|(0<<4);
    
    SSI2->CR1 = (0<<1); //SSE=0 deshabilitar modulo
    SSI2->CR1 = (0<<2); //MS = 0 modo maestro
    SSI2->CC = (0x9<<0); //system clock = 20MHz
    //SSInClk = SysClk / (CPSDVSR * (1 + SCR))
    // NUESTRAS CUENTAS:
    // 4MHz = 20MHz / 1*(1 + 4) // NUESTRA PROPUESTA PARA 4 MHz
    //2 500 000 = 50 000 000/(2*(1+SCR))
    // SCR = (50 000 000/2 500 000*2) - 1 = 9
    SSI2->CPSR =0x1; //NUMERO QUE ASIGNO EL PROFE CPSDVSR y NOSOTROS ELEGIMOS VALOR = 1
    SSI2->CR0 = (0x4<<8) | (0x1<<6) | (0x1<<4) | (0xF<<0); // datos de 8 bits  // 0x4 Viene del SCR que vale 4 luego en el segundo argumento 0x1 es nuestro modulo de polaridad y El 0xB es el numero de bits a leer que seria 16
    SSI2->CR1 |= (1<<1); //SSE=1 habilitar modoulo p.961 (0x02)
}
// ESCRITURA DE DATOS

extern void SPI_write(uint8_t data)
{
    while (SSI2->SR & 0x2)
    {
        SSI2->DR = (uint16_t)data;
    }
    
}

extern void SPI_write_data(uint8_t reg, uint8_t data)
{
    GPIOB->DATA &= ~(1<<5); // CS = 0 se niega
    SPI_write(reg & ~0x80); //escribir registro + MSB igualado a cero
    SPI_write(data);
    GPIOB->DATA |= (1<<5); //CS = 1
}

// LECTURA DE DATOS 

extern uint8_t SPI_read(void)
{
    uint8_t data = 0;
    while ((SSI2->SR & 0x10) == 0x10); // espera por busy bit
    data = SSI2->DR;
    return data;
}

extern uint8_t SPI_read_data(uint8_t reg)
{
    uint8_t data = 0;
    GPIOB->DATA &= ~(1<<5); // CS = 0
    SPI_write(reg | 0x80); // escribe registro + MSB
    SPI_write(0x00); //escribir dato para generar señal de reloj
    data = SPI_read(); //leer dato
    GPIOB->DATA |= (1<<5); //CS = 1
    return data;
}
