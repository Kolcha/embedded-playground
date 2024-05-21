#include "pico/stdlib.h"
#include "hardware/pwm.h"

int main() {
    gpio_set_function(2, GPIO_FUNC_PWM);
    gpio_set_function(3, GPIO_FUNC_PWM);
    gpio_set_function(4, GPIO_FUNC_PWM);

    pwm_config config = pwm_get_default_config();
    pwm_config_set_wrap(&config, 255);
    pwm_config_set_clkdiv(&config, 4.f);

    pwm_init(1, &config, true);     // GPIO 2,3
    pwm_init(2, &config, true);     // GPIO 4

    pwm_set_gpio_level(2, 128);
    pwm_set_gpio_level(3, 128);
    pwm_set_gpio_level(4, 16);

    while (1)
        tight_loop_contents();
}
