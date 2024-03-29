#include <stdint.h>

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

void main(void)
{
	RCC_APB2ENR |= RCC_IOPAEN;
	GPIOA_CRL   &= 0xFF0FFFFF;
	GPIOA_CRL   |= 0x00200000;

	while(1)
	{
		GPIOA_ODR |=  GPIOA5;
		for (volatile int i = 0; i < 500000; i++); // arbitrary delay
		GPIOA_ODR &= ~GPIOA5;
		for (volatile int i = 0; i < 500000; i++); // arbitrary delay
	}
}
