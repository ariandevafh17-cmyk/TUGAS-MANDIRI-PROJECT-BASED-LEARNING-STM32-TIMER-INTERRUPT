#include "stm32f4xx.h"

volatile uint8_t semaphore_flag = 0;

int main(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    GPIOA->MODER &= ~(3U << 10);
    GPIOA->PUPDR &= ~(3U << 10);
    GPIOA->PUPDR |=  (1U << 10);

    GPIOA->MODER &= ~(3U << 14);
    GPIOA->MODER |=  (1U << 14);
    GPIOA->ODR   &= ~(1U << 7);

    SYSCFG->EXTICR[1] &= ~(0xFU << 4);
    EXTI->IMR  |= (1U << 5);
    EXTI->FTSR |= (1U << 5);
    NVIC_EnableIRQ(EXTI9_5_IRQn);

    TIM2->PSC = 15999;
    TIM2->ARR = 49;
    TIM2->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM2_IRQn);

    while (1) {
        if (semaphore_flag == 1) {
            semaphore_flag = 0;
            GPIOA->ODR ^= (1U << 7);
        }
    }
}

void EXTI9_5_IRQHandler(void) {
    if (EXTI->PR & (1U << 5)) {
        EXTI->PR  |=  (1U << 5);
        EXTI->IMR &= ~(1U << 5);

        TIM2->CNT = 0;
        TIM2->CR1 |= TIM_CR1_CEN;
    }
}

void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR  &= ~TIM_SR_UIF;
        TIM2->CR1 &= ~TIM_CR1_CEN;

        if ((GPIOA->IDR & (1U << 5)) == 0) {
            semaphore_flag = 1;
        }

        EXTI->PR  |= (1U << 5);
        EXTI->IMR |= (1U << 5);
    }
}
