#include <SPI.h>

#define CS_PIN     PA4
SPIClass SPI_1(PA7, PA6, PA5);

static void write_reg(uint8_t reg, uint8_t value) {
  // uint8_t tx_data[2] = { reg, value };
  digitalWrite(CS_PIN, LOW);
  SPI_1.transfer(reg);
  SPI_1.transfer(value);
  // SPI_1.transfer(tx_data, 2);
  digitalWrite(CS_PIN, HIGH);
}

static void set_row(uint8_t row_index) {
  write_reg(row_index + 1, 0xFF);
}

static void set_col(uint8_t col_index) {
  for (int i = 0; i < 8; i++) {
    write_reg(i + 1, 0x01 << col_index);
  }
}

static void clear(void) {
  for (int i = 0; i < 8; i++) {
    write_reg(i + 1, 0x00);
  }
}


#define DECODE_MODE_REG     0x09
#define INTENSITY_REG       0x0A
#define SCAN_LIMIT_REG      0x0B
#define SHUTDOWN_REG        0x0C
#define DISPLAY_TEST_REG    0x0F

static void max7219_init() {
  write_reg(DISPLAY_TEST_REG, 0);
  write_reg(SCAN_LIMIT_REG, 7);
  write_reg(DECODE_MODE_REG, 0);
  write_reg(SHUTDOWN_REG, 1);
  clear();
}


void setup() {
  pinMode(CS_PIN, OUTPUT);
  SPI_1.begin();
  Serial1.begin(115200);
  while (!Serial1);
  Serial1.printf("setup()\n");
  max7219_init();
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // Serial1.printf("loop()\n");
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  write_reg(0x03, 0x0F);
  delay(1000);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  write_reg(0x03, 0xF0);
  delay(1000);                      // wait for a second
}
