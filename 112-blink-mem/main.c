#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define SIO_BASE      0xd0000000
#define GPIO_OE_SET   (*(volatile uint32_t *)(SIO_BASE + 0x024))
#define GPIO_OUT_SET  (*(volatile uint32_t *)(SIO_BASE + 0x014))

const uint LED_PIN = 25;

int main()
{
    // gpio_init(LED_PIN);
    // gpio_set_dir(LED_PIN, GPIO_OUT);

    GPIO_OE_SET  = 1u << 25;   // вывод 25 — выход
    GPIO_OUT_SET = 1u << 25;   // на выводе 25 — высокий уровень

    while (1)
    {
        GPIO_OUT_SET = 1u << 25;   // на выводе 25 — высокий уровень
        sleep_ms(250);
        GPIO_OUT_SET = 0;   // на выводе 25 — высокий уровень
        sleep_ms(1000);
    }
}