#include <stdint.h>

//#include "stm32f10x.h"
//#include "stm32f10x_gpio.h"
//#include "stm32f10x_rcc.h"

// register address
#define RCC_BASE      0x40021000
#define GPIOA_BASE    0x40010800

#define RCC_APB2ENR   *(volatile uint32_t *)(RCC_BASE   + 0x18)
#define GPIOA_CRH     *(volatile uint32_t *)(GPIOA_BASE + 0x04)
#define GPIOA_CRL     *(volatile uint32_t *)(GPIOA_BASE + 0x00)
#define GPIOA_ODR     *(volatile uint32_t *)(GPIOA_BASE + 0x0C)

// bit fields
#define RCC_IOPAEN   (1<<2)
#define GPIOA5       (1UL<<5)

int main(void)
{
	RCC_APB2ENR |= RCC_IOPAEN;
	GPIOA_CRL   &= 0xFF0FFFFF;
	GPIOA_CRL   |= 0x00200000;
  //volatile int i;
  /* Initialize Leds mounted on STM32 board */
  //GPIO_InitTypeDef  GPIO_InitStructure;
  /* Initialize LED which connected to PC13, Enable the Clock*/
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  /* Configure the GPIO_LED pin */
  //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //GPIO_Init(GPIOA, &GPIO_InitStructure);

	while(1)
  {
    
		GPIOA_ODR |=  GPIOA5;
		for (volatile int i = 0; i < 500000; i++); // arbitrary delay
		GPIOA_ODR &= ~GPIOA5;
		for (volatile int i = 0; i < 500000; i++); // arbitrary delay
    
    /* Toggle LED which connected to PC13*/
    //GPIOC->ODR ^= GPIO_Pin_13;

    /* delay */
    //for(i=0;i<500000;i++);
	}

	return 0;
}
