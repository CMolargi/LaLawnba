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

void setup_GPIO(void){
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIODEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOAEN;
    GPIOA->MODER &= ~0xF0003;
    GPIOA->MODER |= 0x50000;
    GPIOA->PUPDR &= ~0x1;
    GPIOA->PUPDR |= 0x2;
    GPIOB->MODER &= ~0x3F;
    GPIOB->MODER |= 0xA;
    GPIOB->PUPDR &= ~0x30;
    GPIOB->PUPDR |= 0x20;
    GPIOB->AFR[0] &= ~0x22;
    GPIOB->AFR[0] |= 0x11;
    GPIOC->MODER &= ~GPIO_MODER_MODER7;
    GPIOC->MODER &= ~GPIO_MODER_MODER6;
    GPIOC->MODER |= GPIO_MODER_MODER7_1;
    GPIOC->MODER |= GPIO_MODER_MODER6_1;
    GPIOC->AFR[0] &= ~GPIO_AFRL_AFR7;
    GPIOC->AFR[0] &= ~GPIO_AFRL_AFR6;
    GPIOC->AFR[0] |= 0x11000000;

}
void setup_tim3(void){
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    TIM3->PSC = 0;
    TIM3->ARR = 1500-1;
    TIM3->CCMR1 |= 0x6060;
    TIM3->CCMR2 |= 0x6060;
    TIM3->CCMR1 |= 0x0800;
    TIM3->CCER |= TIM_CCER_CC1E;
    TIM3->CCER |= TIM_CCER_CC2E;
    TIM3->CCER |= TIM_CCER_CC3E;
    TIM3->CCER |= TIM_CCER_CC4E;
    TIM3->CR1 |= 0x1;
}
void setup_UART(void){
    RCC->APB2ENR |= RCC_APB2ENR_USART7EN;
    USART7->CR1 &= ~USART_CR1_UE;
    USART7->CR1 &= ~USART_CR1_OVER8 & ~USART_CR1_PCE & ~USART_CR1_M & ~0x10000000;
    USART7->CR2 &= ~USART_CR2_STOP;
    USART7->BRR |= 0x1388;
    USART7->CR1 |= USART_CR1_TE | USART_CR1_RE;
    USART7->CR1 |= USART_CR1_UE;
    while((USART7->ISR & USART_ISR_TEACK) != 0x200000){

    }
    while((USART7->ISR & USART_ISR_REACK) != 0x400000){
    }
}
char UART7_GetChar(void){
    char c;
    while(!(USART7->ISR & USART_ISR_RXNE)){
    }
    c = USART7->RDR;
    nano_wait(250000);
   // USART5->TDR = c;
   // USART5->TDR = '\n';
    return c;
}
void blade_control(void){
    if(((GPIOB->IDR &=0x4) == 4)){
        GPIOA->ODR &= ~0x100;
        GPIOA->ODR |= 0x200;
    }
    else{
        GPIOA->ODR &= ~0x200;
        GPIOA->ODR |= 0x100;
    }
}
void drive_var(char buffer[]){
    int x1 = buffer[2];
    int x2 = buffer[5];
    x1 = x1 - 48;
    x2 = x2 - 48;
    if(x1 == 49){
        x1 = 10;
    }
    else if(x2 == 49){
        x2 = 10;
    }
    if(buffer[2] == '0' && buffer[5] == '0'){
        TIM3->CCR3 = TIM3->CCR4 = 0;
    }
    else if(buffer[1] == '1'){
        GPIOC->ODR |= GPIO_ODR_8;
        if(buffer[4] == '1' && buffer[5] != '0'){
            TIM3->CCR3 = (x1*100) + 400;
            TIM3->CCR4 = (x2*100) + 400;
        }
        else if(buffer[4] == '0' && buffer[5] != '0'){
            TIM3->CCR3 = (x1*100) + 400;
            TIM3->CCR4 = (x2*100) + 400;

        }
        else{
            TIM3->CCR3 = TIM3->CCR4 = (x1*100) + 400;
        }
    }
    else if(buffer[1] == '0'){
        GPIOC->ODR &= ~GPIO_ODR_8;
        if(buffer[4] == '1' && buffer[5] != '0'){
            TIM3->CCR3 = (x1*100) + 400;
            TIM3->CCR4 = (x2*100) + 400;
        }
        else if(buffer[4] == '0' && buffer[5] != '0'){
            TIM3->CCR3 = (x1*100) + 400;
            TIM3->CCR4 = (x2*100) + 400;
        }
        else{
            TIM3->CCR3 = TIM3->CCR4 = (x1*100) + 400;
        }
    }
}
void drive(char *buffer){
    int x1 = buffer[2];
    int x2 = buffer[5];
    x1 = x1 - 48;
    x2 = x2 - 48;
    if(x1 == 49){
        x1 = 10;
    }
    if(buffer[1] == '1' && buffer[2] != '0'){
        GPIOC->ODR |= GPIO_ODR_8;
        GPIOC->ODR|= GPIO_ODR_9;
        TIM3->CCR3 = TIM3->CCR4 = (x1*100) + 400;
    }
    else if(buffer[4] == '1' && buffer[5] != '0'){
        GPIOC->ODR &= ~GPIO_ODR_8;
        GPIOC->ODR|= GPIO_ODR_9;
        TIM3->CCR3 = TIM3->CCR4 = (x2*100) + 400;
    }
    else if(buffer[4] == '0' && buffer[5] != '0'){
        GPIOC->ODR &= ~GPIO_ODR_9;
        GPIOC->ODR|= GPIO_ODR_8;
        TIM3->CCR3 = TIM3->CCR4 = (x2*100) + 400;
    }
    else if(buffer[1] == '0' && buffer[2] != '0'){
        GPIOC->ODR &= ~GPIO_ODR_8;
        GPIOC->ODR &= ~GPIO_ODR_9;
        TIM3->CCR3 = TIM3->CCR4 = (x1*100) + 400;
    }
    else{
        TIM3->CCR3 = TIM3->CCR4 = 0;
    }
}
void read_UART(void){
    GPIOC->MODER &= ~GPIO_MODER_MODER8;
    GPIOC->MODER |= GPIO_MODER_MODER8_0;
    GPIOC->MODER &= ~GPIO_MODER_MODER9;
    GPIOC->MODER |= GPIO_MODER_MODER9_0;
    GPIOA->ODR|= GPIO_ODR_9;
    char buffer[6];
    char c;
    int i = 0;
    int fillStarted  = 0;
    while(i < 6){
        c = UART7_GetChar();
        if((c == '$') & (fillStarted == 0)){
            i = 0;
            fillStarted = 1;
            buffer[i] = c;
        }
        else if (fillStarted == 1){
            buffer[i] = c;
        }
        nano_wait(100000);
        USART7->TDR = buffer[i]; // serial print of value
        i++;
        nano_wait(100); //Gotta wait
    }
    USART7->TDR = '\r\n';
    //drive_var(buffer);
    drive(buffer);


}
void test_UART(void){
    while(!(USART7->ISR & USART_ISR_TXE)){}
    USART7->TDR = '9';
}
void test_waveform(void){
    TIM3->CCR3 = TIM3->CCR4 = 800;
}

int main(void)
{
    setup_GPIO();
    setup_tim3();
    setup_UART();
	for(;;){

	    read_UART();
	    //test_UART();
	    //blade_control();
	    //test_waveform();
    }

}
