#include "pico/stdlib.h"
#include "hardware/gpio.h"

const uint LED_PIN = 25;

uint32_t read_timer(void)
{
    uint32_t value = time_us_32();
    return value;
}

uint32_t measure_sum(uint8_t count)
{
    uint32_t s;
    uint32_t interval = read_timer();
    for (int i = 0; i < count; i++)
    {
        s = read_timer() + read_timer();
    }
    interval = read_timer() - interval;
    return interval;
}

void profile_experiment(uint8_t measures, uint8_t count_per_measure)
{
    float* results = malloc(measures * sizeof(uint32_t));
    for (int i = 0; i < measures; i++)
    {
        uint32_t result = measure_sum(count_per_measure);
        float result = results / count_per_measure;
        results[i] = result;
    }
    
    free(results);
}

int main()
{
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (1)
    {
        gpio_put(LED_PIN, 1);
        sleep_ms(250);
        gpio_put(LED_PIN, 0);
        sleep_ms(1000);
    }
}