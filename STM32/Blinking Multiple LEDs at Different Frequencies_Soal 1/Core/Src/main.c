#include "stm32f4xx.h"

int main(void) {
    // 1. Aktifkan Clock GPIOA, GPIOB, serta TIM2, TIM3, dan TIM4
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

    // 2. Konfigurasi Pin PA0 (LED Hijau - Output) -> posisi bit: 0 * 2 = 0
    GPIOA->MODER &= ~(3U << 0);   // Reset bit PA0
    GPIOA->MODER |=  (1U << 0);   // PA0 = Output (01)

    // 3. Konfigurasi Pin PA6 (LED Kuning - Output) -> posisi bit: 6 * 2 = 12
    GPIOA->MODER &= ~(3U << 12);  // Reset bit PA6
    GPIOA->MODER |=  (1U << 12);  // PA6 = Output (01)

    // 4. Konfigurasi Pin PB6 (LED Merah - Output) -> posisi bit: 6 * 2 = 12
    GPIOB->MODER &= ~(3U << 12);  // Reset bit PB6
    GPIOB->MODER |=  (1U << 12);  // PB6 = Output (01)

    // 5. Konfigurasi TIM2 (Toggle setiap 500 ms -> Kedip 1 Hz)
    TIM2->PSC = 16000;            // 16 MHz / 16000 = 1000 Hz (1 tick = 1 ms)
    TIM2->ARR = 999;              // 1000 ms
    TIM2->DIER |= TIM_DIER_UIE;   // Aktifkan Update Interrupt
    TIM2->CR1  |= TIM_CR1_CEN;    // Jalankan TIM2
    NVIC_EnableIRQ(TIM2_IRQn);    // Aktifkan TIM2 di NVIC

    // 6. Konfigurasi TIM3 (Toggle setiap 250 ms -> Kedip 2 Hz)
    TIM3->PSC = 16000;            // 1 tick = 1 ms
    TIM3->ARR = 499;              // 250 ms
    TIM3->DIER |= TIM_DIER_UIE;   // Aktifkan Update Interrupt
    TIM3->CR1  |= TIM_CR1_CEN;    // Jalankan TIM3
    NVIC_EnableIRQ(TIM3_IRQn);    // Aktifkan TIM3 di NVIC

    // 7. Konfigurasi TIM4 (Toggle setiap 100 ms -> Kedip 5 Hz)
    TIM4->PSC = 16000;            // 1 tick = 1 ms
    TIM4->ARR = 299;               // 100 ms
    TIM4->DIER |= TIM_DIER_UIE;   // Aktifkan Update Interrupt
    TIM4->CR1  |= TIM_CR1_CEN;    // Jalankan TIM4
    NVIC_EnableIRQ(TIM4_IRQn);    // Aktifkan TIM4 di NVIC

    while (1) {
        // Loop utama kosong (semua berjalan paralel via hardware timer)
    }
}

// ISR TIM2: Toggle LED Hijau (PA0) setiap 500 ms
void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF;        // Reset flag UIF
        GPIOA->ODR ^= (1U << 0);        // Toggle PA0
    }
}

// ISR TIM3: Toggle LED Kuning (PA6) setiap 250 ms
void TIM3_IRQHandler(void) {
    if (TIM3->SR & TIM_SR_UIF) {
        TIM3->SR &= ~TIM_SR_UIF;        // Reset flag UIF
        GPIOA->ODR ^= (1U << 6);        // Toggle PA6
    }
}

// ISR TIM4: Toggle LED Merah (PB6) setiap 100 ms
void TIM4_IRQHandler(void) {
    if (TIM4->SR & TIM_SR_UIF) {
        TIM4->SR &= ~TIM_SR_UIF;        // Reset flag UIF
        GPIOB->ODR ^= (1U << 6);        // Toggle PB6
    }
}
