#include "lib/include.h"
extern void Configurar_GPIO(void)
{
    //Paso 1 Enceder el periferico Run Clock Gate Control GPIO
    SYSCTL->RCGCGPIO |= (1<<0) | (1<<1) | (1<<2) | (1<<3);//HABILITAMOS PUERTSO A,B,C y D
    while((SYSCTL->PRGPIO&0x20)==0){};
    //Paso 2
 //   GPIO_PORTB_AHB_LOCK_R = 0x4C4F434B;
 //   GPIO_PORTB_AHB_CR_R = (1<<3);

    //Salidas
    GPIOD->DIR |= (1<<0) | (1<<1); //N - D y J - C
    GPIOB->DIR |= (1<<0);
    //Entradas
    GPIOD->DIR |= (0<<1) | (0<<0);
    GPIOC->PUR =  (1<<1) | (1<<0);
    //Paso 3
    GPIOC->AFSEL |= 0x00;
    GPIOD->AFSEL |= 0x00;
    GPIOB->AFSEL |= 0x00;

    GPIOC->PCTL|= 0x00;
    GPIOD->PCTL |= 0x00;
    GPIOB->PCTL |= 0x00;

    GPIOD->DEN |= (1<<0) | (1<<1);
    GPIOC->DEN |= (1<<1) | (1<<0);
    GPIOB->DEN |=  (1<<0);

    //configurar evento de interrupcion PORTJ
    GPIOC->IM |= (0<<1) | (0<<0); //Limpiar los bits
    GPIOC->IS |= (0<<1) | (0<<0);
    GPIOC->IBE |= (0<<1) | (0<<0);
    GPIOC->IEV |= (1<<1) | (1<<0);
    GPIOC->RIS |= (0<<1) | (0<<0);
    GPIOC->IM |= (1<<1) | (1<<0);
    //                             
    NVIC->IP[12] = (NVIC->IP[12]&0x00FFFFFF) | 0x80000000;;
    NVIC->ISER[1] = (1UL << 19);
    // numero de interrupcion PORTJ = 51
    // n=12 ----> [4n+3] [4n+2] [4n+1] [4n] ---> [4n+3]

}

extern void Prender_LED(void)
{
    static int i=0;
    GPIOF->DATA = i;
    i = i + 1;
}

extern void GPIOC_INT_ISR(void)
{
    if (GPIOC->RIS == (1<<0))//J0  el cero representara que se presiono el boton 0 y en pycharm se contara los ceros enviados
    {
        
       // printString(global);
      
    }
    if (GPIOC->RIS == (1<<1))//J1 el uno representara que se presiono el boton 1
    {


   
    }
    GPIOC->ICR |=(1<<0)|(1<<1);
}

