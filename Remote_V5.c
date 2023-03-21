/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f0xx.h"
#include <unistd.h>
#include <string.h>

static inline void nano_wait(unsigned int n){
    asm("   mov r0,%0\n"
            "repeat: sub r0,#83\n"
            " bgt repeat\n" : : "r"(n) : "r0", "cc");
}

void setup_UART(void){
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIODEN;
    GPIOC->MODER &= ~GPIO_MODER_MODER12;
    GPIOC->MODER |= GPIO_MODER_MODER12_1;
    GPIOC->AFR[1] &= ~GPIO_AFRH_AFR12;
    GPIOC->AFR[1] |= 0x20000;
    GPIOD->MODER &= ~GPIO_MODER_MODER2;
    GPIOD->MODER |= GPIO_MODER_MODER2_1;
    GPIOD->AFR[0] &= ~GPIO_AFRL_AFR2;
    GPIOD->AFR[0] |= 0x200;
    RCC->APB1ENR |= RCC_APB1ENR_USART5EN;
    USART5->CR1 &= ~USART_CR1_UE;
    USART5->CR1 &= ~USART_CR1_OVER8 & ~USART_CR1_PCE & ~USART_CR1_M & ~0x10000000;
    USART5->CR2 &= ~USART_CR2_STOP;
    USART5->BRR |= 0x1388;
    USART5->CR1 |= USART_CR1_TE | USART_CR1_RE;
    USART5->CR1 |= USART_CR1_UE;
    while((USART5->ISR & USART_ISR_TEACK) != 0x200000){

    }
    while((USART5->ISR & USART_ISR_REACK) != 0x400000){
    }
}
char UART5_GetChar(void){
    char c;
    while(!(USART5->ISR & USART_ISR_RXNE)){

    }
    c = USART5->RDR;
    nano_wait(25000);
 //   USART5->TDR = c;
   // USART5->TDR = '\n';
    return c;
}

void read_UART(void){
    GPIOC->MODER &= ~GPIO_MODER_MODER6;
    GPIOC->MODER |= GPIO_MODER_MODER6_0;
    GPIOC->MODER &= ~GPIO_MODER_MODER7;
    GPIOC->MODER |= GPIO_MODER_MODER7_0;
    char buffer[6];
    char c;
    int i = 0;
    int fillStarted  = 0;

    while(i < 6){
        c = UART5_GetChar();
        while((c != '$') & (fillStarted == 0)){
        }
        fillStarted = 1;
        buffer[i] = c;
        USART5->TDR = buffer[i];
        nano_wait(10000); //Gotta wait
        i++;
    }
    USART5->TDR = '\n';





        /*if(USART5->RDR == '0'){
            GPIOC->ODR |= GPIO_ODR_6;
            GPIOC->ODR &= ~GPIO_ODR_7;
        }
        else{
            GPIOC->ODR |= GPIO_ODR_7;
            GPIOC->ODR &= ~GPIO_ODR_6;
        }*/

}
void test_UART(void){
    while(!(USART5->ISR & USART_ISR_TXE)){}
    USART5->TDR = '!';
}
int main(void)
{
    setup_UART();
	for(;;){
	    read_UART();
	}

}
