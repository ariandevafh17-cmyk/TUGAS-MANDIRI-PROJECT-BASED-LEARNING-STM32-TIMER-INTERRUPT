#include "stm32f4xx.h"

int main(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    GPIOA->MODER &= ~(3U << 10);
    GPIOA->PUPDR &= ~(3U << 10);
    GPIOA->PUPDR |=  (1U << 10);

    GPIOB->MODER &= ~(3U << 2);
    GPIOB->MODER |=  (1U << 2);

    TIM2->PSC = 16000;
    TIM2->ARR = 50;
    TIM2->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM2_IRQn);

    while (1) {
        if ((GPIOA->IDR & (1U << 5)) == 0) {
            TIM2->CR1 |= TIM_CR1_CEN;
        } else {
            TIM2->CR1 &= ~TIM_CR1_CEN;
            TIM2->CNT = 0;
            GPIOB->ODR &= ~(1U << 1);
        }
    }
}

void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF;
        GPIOB->ODR ^= (1U << 1);
    }
}
