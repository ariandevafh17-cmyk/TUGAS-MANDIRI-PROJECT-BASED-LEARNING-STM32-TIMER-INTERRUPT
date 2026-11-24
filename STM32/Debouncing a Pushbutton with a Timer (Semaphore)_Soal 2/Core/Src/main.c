#include "stm32f4xx.h"

// Variabel Semaphore (Flag)
volatile uint8_t semaphore_flag = 0;

int main(void) {
    // 1. Aktifkan Clock GPIOA, GPIOB, SYSCFG (untuk EXTI), dan TIM2
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // 2. Konfigurasi Pin PA5 (Input Pull-Up) -> posisi bit: 5 * 2 = 10
    GPIOA->MODER &= ~(3U << 10);  // PA5 = Input (00)
    GPIOA->PUPDR &= ~(3U << 10);
    GPIOA->PUPDR |=  (1U << 10);  // PA5 = Pull-up (01)

    // 3. Konfigurasi Pin PA7 (Output) -> posisi bit: 1 * 2 = 2
    GPIOA->MODER &= ~(3U << 14);   // Reset bit PB1
    GPIOA->MODER |=  (1U << 14);   // PB1 = Output (01)
    GPIOA->ODR   &= ~(1U << 7);   // Matikan LED awal (PA7 = 0)

    // 4. Konfigurasi Jalur Interupsi Eksternal EXTI5 ke Port A (PA5)
    // EXTI5 berada di EXTICR[1] posisi bit: (5 % 4) * 4 = 4
    SYSCFG->EXTICR[1] &= ~(0xFU << 4);  // 0000 = Port A untuk EXTI5
    EXTI->IMR  |= (1U << 5);            // Aktifkan mask interrupt jalur 5
    EXTI->FTSR |= (1U << 5);            // Deteksi Falling Edge (tombol ditekan ke GND)
    NVIC_EnableIRQ(EXTI9_5_IRQn);       // Ak	tifkan EXTI9_5 di NVIC

    // 5. Konfigurasi TIM2 (Waktu Debounce 50 ms)
    TIM2->PSC = 15999;                  // 16 MHz / 16000 = 1000 Hz (1 tick = 1 ms)
    TIM2->ARR = 49;                     // 50 ms
    TIM2->DIER |= TIM_DIER_UIE;         // Aktifkan Update Interrupt
    NVIC_EnableIRQ(TIM2_IRQn);          // Aktifkan TIM2 di NVIC

    while (1) {
        // Cek Semaphore tanpa membuat proses utama terblokir (non-blocking)
        if (semaphore_flag == 1) {
            semaphore_flag = 0;         // Reset semaphore
            GPIOA->ODR ^= (1U << 7);    // Toggle LED di PB1
        }
    }
}

// ISR EXTI5: Dipicu saat tombol PA5 pertama kali ditekan
void EXTI9_5_IRQHandler(void) {
    if (EXTI->PR & (1U << 5)) {
        EXTI->PR  |=  (1U << 5);        // Reset pending flag EXTI5
        EXTI->IMR &= ~(1U << 5);        // Matikan sementara EXTI5 (abaikan bouncing)

        TIM2->CNT = 0;                  // Reset hitungan timer
        TIM2->CR1 |= TIM_CR1_CEN;       // Mulai timer debounce 50 ms
    }
}

// ISR TIM2: Dipicu setelah jeda debounce 50 ms selesai
void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR  &= ~TIM_SR_UIF;       // Reset flag UIF
        TIM2->CR1 &= ~TIM_CR1_CEN;      // Hentikan TIM2

        // Pastikan tombol PA5 masih dalam kondisi ditekan (logika 0)
        if ((GPIOA->IDR & (1U << 5)) == 0) {
            semaphore_flag = 1;         // Kirim sinyal semaphore ke main loop
        }

        EXTI->PR  |= (1U << 5);         // Bersihkan sisa pending flag
        EXTI->IMR |= (1U << 5);         // Aktifkan kembali interupsi EXTI5
    }
}
