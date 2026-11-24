#include "stm32f4xx.h"

int main(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

    GPIOA->MODER &= ~(3U << 0);
    GPIOA->MODER |=  (1U << 0);

    GPIOA->MODER &= ~(3U << 12);
    GPIOA->MODER |=  (1U << 12);

    GPIOB->MODER &= ~(3U << 12);
    GPIOB->MODER |=  (1U << 12);

    TIM2->PSC = 16000;
    TIM2->ARR = 999;
    TIM2->DIER |= TIM_DIER_UIE;
    TIM2->CR1  |= TIM_CR1_CEN;
    NVIC_EnableIRQ(TIM2_IRQn);

    TIM3->PSC = 16000;
    TIM3->ARR = 499;
    TIM3->DIER |= TIM_DIER_UIE;
    TIM3->CR1  |= TIM_CR1_CEN;
    NVIC_EnableIRQ(TIM3_IRQn);

    TIM4->PSC = 16000;
    TIM4->ARR = 299;
    TIM4->DIER |= TIM_DIER_UIE;
    TIM4->CR1  |= TIM_CR1_CEN;
    NVIC_EnableIRQ(TIM4_IRQn);

    while (1) {
    }
}

void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF;
        GPIOA->ODR ^= (1U << 0);
    }
}

void TIM3_IRQHandler(void) {
    if (TIM3->SR & TIM_SR_UIF) {
        TIM3->SR &= ~TIM_SR_UIF;
        GPIOA->ODR ^= (1U << 6);
    }
}

void TIM4_IRQHandler(void) {
    if (TIM4->SR & TIM_SR_UIF) {
        TIM4->SR &= ~TIM_SR_UIF;
        GPIOB->ODR ^= (1U << 6);
    }
}
