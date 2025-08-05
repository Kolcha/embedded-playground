#define ADC_GPIO        36
#define LED_GPIO        2

#define PWM_FREQ        50000
#define PWM_BITS        10

constexpr uint32_t max_duty = (1 << PWM_BITS) - 1;

uint32_t last_duty = 0;

void setup() {
  ledcAttach(LED_GPIO, PWM_FREQ, PWM_BITS);
  analogReadResolution(PWM_BITS);
}

void loop() {
  uint32_t curr_duty = analogRead(ADC_GPIO);
  if (curr_duty != last_duty) {
    last_duty = curr_duty;
    ledcWrite(LED_GPIO, curr_duty);
  }
}
