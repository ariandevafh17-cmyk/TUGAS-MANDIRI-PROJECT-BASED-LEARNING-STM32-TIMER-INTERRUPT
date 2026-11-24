#include "stm32f4xx.h"

int main(void) {
    // Aktifkan Clock GPIOA, GPIOB, dan TIM2
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;


    // Konfigurasi Pin PA5 (Input Pull-Up) -> posisi bit: 5 * 2 = 10
    GPIOA->MODER &= ~(3U << 10);  // PA5 = Input (00)
    GPIOA->PUPDR &= ~(3U << 10);
    GPIOA->PUPDR |=  (1U << 10);  // PA5 = Pull-up (01)

    // Konfigurasi Pin PB1 (Output) -> posisi bit: 1 * 2 = 2
    GPIOB->MODER &= ~(3U << 2);   // Reset bit PB1
    GPIOB->MODER |=  (1U << 2);   // PB1 = Output (01)

    // Konfigurasi TIM2 (Interval kedip 200 ms)
    TIM2->PSC = 16000;
    TIM2->ARR = 50	;
    TIM2->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM2_IRQn);

    while (1) {
        // Cek register IDR bit 5 (PA5): bernilai 0 jika tombol ditekan
        if ((GPIOA->IDR & (1U << 5)) == 0) {
            TIM2->CR1 |= TIM_CR1_CEN;   // Jalankan TIM2
        } else {
            TIM2->CR1 &= ~TIM_CR1_CEN;  // Hentikan TIM2
            TIM2->CNT = 0;              // Reset hitungan timer
            GPIOB->ODR &= ~(1U << 1);   // Matikan LED (PB1 = 0)
        }
    }
}

// ISR: Mengedipkan LED setiap TIM2 mencapai 200 ms
void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF;        // Reset flag UIF
        GPIOB->ODR ^= (1U << 1);        // Toggle LED di PB1
    }
}
