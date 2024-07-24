// cricial audio bits taken from https://github.com/espressif/esp-idf/blob/master/examples/bluetooth/bluedroid/classic_bt/a2dp_sink/main/main.c

// bluetooth, config, discover and audio
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"
#include "esp_a2dp_api.h"

// the audio DAC and amp configuration.
#include "driver/i2s.h"

#include <string.h>

static int16_t raw_input[2*1024];

static void a2d_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param) {
  switch (event) {
    case ESP_A2D_CONNECTION_STATE_EVT:
      Serial.printf("ESP_A2D_CONNECTION_STATE_EVT: %d\n", param->conn_stat.state);
      break;
    case ESP_A2D_AUDIO_STATE_EVT:
      Serial.printf("ESP_A2D_AUDIO_STATE_EVT: %d\n", param->audio_stat.state);
      break;
    case ESP_A2D_AUDIO_CFG_EVT:
      Serial.println("ESP_A2D_AUDIO_CFG_EVT");
      break;
    case ESP_A2D_MEDIA_CTRL_ACK_EVT:
      Serial.println("ESP_A2D_MEDIA_CTRL_ACK_EVT");
      break;
    case ESP_A2D_PROF_STATE_EVT:
      Serial.println("ESP_A2D_PROF_STATE_EVT");
      break;
  }
}

// the callback(processes bluetooth data).
// this is the most important function.
void bt_data_cb(const uint8_t *data, uint32_t len) {
  uint32_t now = micros();
  static uint32_t total_bytes = 0;
  static uint32_t ts = now;
  total_bytes += len;
  if (total_bytes >= 4096) {
    auto dwn = now - ts;
    Serial.printf("bytes: %lu %lu, t: %ld us\n", total_bytes, len, dwn);
    total_bytes = 0;
    ts = now;
  }
  /*
   // number of 16 bit samples
   int n = len/2;

   // point to a 16bit sample
   int16_t* data16=(int16_t*)data;

   // create a variable (potentially processed) that we'll pass via I2S
   int16_t fy;

   // Records number of bytes written via I2S
   size_t i2s_bytes_write = 0;

   for(int i=0;i<n;i++){
    // put the current sample in fy
    fy=*data16;

    //making this value larger will decrease the volume(Very simple DSP!).
    fy/=1;

    // write data to I2S buffer
    i2s_write(I2S_NUM_0, &fy, 2, &i2s_bytes_write,  10 );

    //move to next memory address housing 16 bit data
    data16++;
   }
*/
  size_t i2s_bytes_write = 0;
  i2s_write(I2S_NUM_0, data, len, &i2s_bytes_write, portMAX_DELAY);
}


void setup() {
  Serial.begin(115200);
  while (!Serial) delay(1);

  // i2s configuration
  static const i2s_config_t i2s_config = {
    .mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = 48000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = static_cast<i2s_comm_format_t>(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,  // default interrupt priority
    .dma_buf_count = 8,
    .dma_buf_len = 1024,
    .use_apll = true,
    .tx_desc_auto_clear = true
  };

  // i2s pinout
  static const i2s_pin_config_t pin_config = {
    .bck_io_num = 26,
    .ws_io_num = 27,
    .data_out_num = 25,
    .data_in_num = I2S_PIN_NO_CHANGE
  };

  // now configure i2s with constructed pinout and config
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
  // i2s_set_clk(I2S_NUM_0, 44100, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_STEREO);
  // i2s_set_sample_rates(I2S_NUM_0, 44100);

  // set up bluetooth classic via bluedroid
  btStart();
  esp_bluedroid_init();
  esp_bluedroid_enable();

  esp_a2d_register_callback(a2d_cb);

  // set up device name
  const char *dev_name = "ESP_SPEAKER_TEST";
  esp_bt_dev_set_device_name(dev_name);

  // initialize A2DP sink and set the data callback(A2DP is bluetooth audio)
  esp_a2d_sink_register_data_callback(bt_data_cb);
  esp_a2d_sink_init();

  // set discoverable and connectable mode, wait to be connected
  esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
}