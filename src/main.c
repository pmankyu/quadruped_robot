#include <stdint.h>
#include <stdio.h>

#include "stm32f10x.h"
#include "stm32_eval.h"

USART_InitTypeDef USART_InitStructure;

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

int main(void)
{
  volatile int i;
	//uint8_t ch_data = 'D';

  /* Initialize Leds mounted on STM32 board */
  GPIO_InitTypeDef  GPIO_InitStructure;
  /* Initialize LED which connected to PC13, Enable the Clock*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  STM_EVAL_COMInit(COM1, &USART_InitStructure);
	setvbuf(stdout, NULL, _IONBF, 0);

  printf("Hello printf!\r\n");
	printf("Characters: %c %c \r\n", 'a', 65);
  printf("Decimals: %d %ld\r\n", 1977, 650000L);
  printf("Preceding with blanks: %10d \r\n", 1977);
  printf("Preceding with zeros: %010d \r\n", 1977);
  printf("Some different radices: %d %x %o %#x %#o \r\n"
																				, 100, 100, 100, 100, 100);
  printf("floats: %4.2f %+.0e %E \r\n", 3.1416, 3.1416, 3.1416);
  printf("Width trick: %*d \r\n", 5, 10);
  printf("%s \r\n", "A string");
	
	while(1)
  {
    /* Toggle LED which connected to PC13*/
    GPIOA->ODR ^= GPIO_Pin_5;

    /* delay */
    for(i=0;i<500000;i++);
	}

	return 0;
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
int __io_putchar(int ch)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(EVAL_COM1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
