#include "state_machine.h"

#include "main.h"
#include "gpio_button.h"

// 5x7, 5 bytes, bit 7 unused
static const uint8_t numbers[] = {
  0x3E, 0x45, 0x49, 0x51, 0x3E,     // 0
  0x00, 0x21, 0x7F, 0x01, 0x00,     // 1
  0x21, 0x43, 0x45, 0x49, 0x31,     // 2
  0x22, 0x41, 0x49, 0x49, 0x36,     // 3
  0x0C, 0x14, 0x24, 0x7F, 0x04,     // 4
  0x72, 0x51, 0x51, 0x51, 0x4E,     // 5
  0x1E, 0x29, 0x49, 0x49, 0x06,     // 6
  0x40, 0x47, 0x48, 0x50, 0x60,     // 7
  0x36, 0x49, 0x49, 0x49, 0x36,     // 8
  0x30, 0x49, 0x49, 0x4A, 0x3C,     // 9
};

#define FONT_CHAR_BYTES     5

#define charidx(n)          (n*FONT_CHAR_BYTES)

static uint64_t max7219_display[4];

extern GPIO_Button usr_btn;

static void draw_char(uint64_t* target, const uint8_t* ch, uint8_t sz,
                      uint8_t x, uint8_t y)
{
  uint8_t* t = (uint8_t*)target;
  for (uint8_t i = 0; i < sz; i++) {
    uint8_t t_bit = i + x;
    for (uint8_t b = 0; b < 8; b++) {
      uint8_t s_bit = 6 - b;
      uint8_t t_byte = 7 - y - b;
      t[t_byte] &= ~(1 << t_bit);
      t[t_byte] |= ((ch[i] & (1 << s_bit)) >> s_bit) << t_bit;
    }
  }
}

static void toggle_bit(uint64_t* v, uint8_t b)
{
  uint64_t m = 1ULL << (b & 0x3F);
  uint64_t d = ~(*v& m) & m;
  *v &= ~m;
  *v |= d;
}

static void render_time()
{
  uint8_t h1 = s_time.Hours / 10;
  uint8_t h2 = s_time.Hours % 10;
  uint8_t m1 = s_time.Minutes / 10;
  uint8_t m2 = s_time.Minutes % 10;
  draw_char(&max7219_display[0], numbers + charidx(h1), FONT_CHAR_BYTES, 2, 1);
  draw_char(&max7219_display[1], numbers + charidx(h2), FONT_CHAR_BYTES, 1, 1);
  draw_char(&max7219_display[2], numbers + charidx(m1), FONT_CHAR_BYTES, 2, 1);
  draw_char(&max7219_display[3], numbers + charidx(m2), FONT_CHAR_BYTES, 1, 1);
  toggle_bit(&max7219_display[1], 15);
  toggle_bit(&max7219_display[1], 47);
}

static void display_time()
{
  render_time();
  max7219_write_frame(max7219_display, countof(max7219_display));
}

#define CALL_VOID_FN(fn)    if (fn) fn()

static void normal_state_enter();
static void normal_state_timer_event(const ClockState** cstate);
static void normal_state_long_press(const ClockState** cstate);

static const ClockState normal_state = {
  .enter_state = &normal_state_enter,
  .timer_event = &normal_state_timer_event,
  .long_press = &normal_state_long_press,
};

static void date_time_config_enter();

static void set_h_state_timer_event(const ClockState** cstate);
static void set_h_state_short_press(const ClockState** cstate);
static void set_h_state_double_click(const ClockState** cstate);

static const ClockState set_h_state = {
  .timer_event = &set_h_state_timer_event,
  .short_press = &set_h_state_short_press,
  .double_click = &set_h_state_double_click,
};

static void set_m_state_timer_event(const ClockState** cstate);
static void set_m_state_short_press(const ClockState** cstate);
static void set_m_state_double_click(const ClockState** cstate);

static const ClockState set_m_state = {
  .timer_event = &set_m_state_timer_event,
  .short_press = &set_m_state_short_press,
  .double_click = &set_m_state_double_click,
};

static void next_state(const ClockState** cs, const ClockState* ns)
{
  CALL_VOID_FN((*cs)->leave_state);
  *cs = ns;
  CALL_VOID_FN((*cs)->enter_state);
}

static void normal_state_enter()
{
  usr_btn.mode = GPIO_BUTTON_MODE_LONG_PRESS;
  display_time();
}

static void normal_state_timer_event(const ClockState** cstate)
{
  UNUSED(cstate);
  display_time();
}

static void normal_state_long_press(const ClockState** cstate)
{
  next_state(cstate, &set_h_state);
}

static void date_time_config_enter()
{
  uint8_t mode = GPIO_BUTTON_MODE_REPEAT | GPIO_BUTTON_FLAG_DBL_CLICK;
  if (usr_btn.mode != mode) usr_btn.mode = mode;
}

static void rtc_set_date_time()
{
  HAL_RTC_SetTime(&hrtc, &s_time, RTC_FORMAT_BIN);
  HAL_RTC_SetDate(&hrtc, &s_date, RTC_FORMAT_BIN);
}

static void set_h_state_timer_event(const ClockState** cstate)
{
  UNUSED(cstate);
  static bool visible = true;
  max7219_display[1] = 0;   // always draw dots
  render_time();
  if (!visible) {
    max7219_display[0] = 0;
    max7219_display[1] = 0;
    toggle_bit(&max7219_display[1], 15);
    toggle_bit(&max7219_display[1], 47);
  }
  max7219_write_frame(max7219_display, countof(max7219_display));
  visible = !visible;
}

static void set_h_state_short_press(const ClockState** cstate)
{
  UNUSED(cstate);
  // button driver bug workaround:
  // don't change button mode until button released
  date_time_config_enter();

  s_time.Hours += 1;
  switch (hrtc.Init.HourFormat) {
    case RTC_HOURFORMAT_24: {
      if (s_time.Hours == 24) {
        s_time.Hours = 0;
      }
      break;
    }
    case RTC_HOURFORMAT_12: {
      if (s_time.Hours == 12) {
        if (s_time.TimeFormat == RTC_HOURFORMAT12_AM) {
          s_time.TimeFormat = RTC_HOURFORMAT12_PM;
        }
        if (s_time.TimeFormat == RTC_HOURFORMAT12_PM) {
          s_time.TimeFormat = RTC_HOURFORMAT12_AM;
        }
      }
      if (s_time.Hours > 12) {
        s_time.Hours -= 12;
      }
      break;
    }
  }
  rtc_set_date_time();
  max7219_display[1] = 0;   // always draw dots
  display_time();
}

static void set_h_state_double_click(const ClockState** cstate)
{
  next_state(cstate, &set_m_state);
}

static void set_m_state_timer_event(const ClockState** cstate)
{
  UNUSED(cstate);
  static bool visible = true;
  max7219_display[1] = 0;   // always draw dots
  render_time();
  if (!visible) {
    max7219_display[2] = 0;
    max7219_display[3] = 0;
  }
  max7219_write_frame(max7219_display, countof(max7219_display));
  visible = !visible;
}

static void set_m_state_short_press(const ClockState** cstate)
{
  UNUSED(cstate);
  s_time.Minutes += 1;
  if (s_time.Minutes == 60) s_time.Minutes = 0;
  rtc_set_date_time();
  max7219_display[1] = 0;   // always draw dots
  display_time();
}

static void set_m_state_double_click(const ClockState** cstate)
{
  next_state(cstate, &normal_state);
}

const ClockState state_machine_initial_state = normal_state;
