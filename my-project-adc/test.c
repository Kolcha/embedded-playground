#include <stdio.h>
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"

uint16_t capture_buf[3*8];
int adc_chan;
int fft_chan;

const uint16_t adc_range = 1 << 12;
const float adc_convert = 3.3f / (adc_range - 1);

void dma_handler() {
    printf("interrupt handler!\n");
    adc_run(false);
    adc_fifo_drain();

    for (int i = 0; i < count_of(capture_buf) / 3; i++) {
        float v1 = capture_buf[3*i+0] * adc_convert;
        float v2 = capture_buf[3*i+1] * adc_convert;
        float v3 = capture_buf[3*i+2] * adc_convert;
        printf("   %02d: 1: %.3f, 2: %.3f 3: %.3f\n", i, v1, v2, v3);
    }

    // Clear the interrupt request.
    dma_hw->ints0 = 1u << adc_chan;
    // re-trigger it
    // dma_channel_start(adc_chan);
    // Give the channel a new wave table entry to read from, and re-trigger it
    adc_select_input(0);
    dma_channel_set_write_addr(adc_chan, capture_buf, true);
    adc_run(true);
}

// Channel 0 is GPIO26
#define CAPTURE_CHANNEL 0

void core1_main();

int main() {
    stdio_init_all();

    // Init GPIO for analogue use: hi-Z, no pulls, disable digital input buffer.
    adc_gpio_init(26 + CAPTURE_CHANNEL);
    adc_gpio_init(26 + CAPTURE_CHANNEL + 1);

    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(CAPTURE_CHANNEL);
    adc_set_round_robin(0x13);
    adc_fifo_setup(
        true,    // Write each completed conversion to the sample FIFO
        true,    // Enable DMA data request (DREQ)
        1,       // DREQ (and IRQ) asserted when at least 1 sample present
        false,   // We won't see the ERR bit because of 8 bit reads; disable.
        false     // Shift each sample to 8 bits when pushing to FIFO
    );

    // Divisor of 0 -> full speed. Free-running capture with the divider is
    // equivalent to pressing the ADC_CS_START_ONCE button once per `div + 1`
    // cycles (div not necessarily an integer). Each conversion takes 96
    // cycles, so in general you want a divider of 0 (hold down the button
    // continuously) or > 95 (take samples less frequently than 96 cycle
    // intervals). This is all timed by the 48 MHz ADC clock.
    adc_set_clkdiv(499);

    printf("Arming DMA\n");
    sleep_ms(1000);
    // Set up the DMA to start transferring data as soon as it appears in FIFO
    adc_chan = dma_claim_unused_channel(true);
    dma_channel_config cfg = dma_channel_get_default_config(adc_chan);

    // Reading from constant address, writing to incrementing byte addresses
    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
    channel_config_set_read_increment(&cfg, false);
    channel_config_set_write_increment(&cfg, true);

    // channel_config_set_ring(&cfg, true, 5); // 1 << 5 byte boundary on write ptr

    // Pace transfers based on availability of ADC samples
    channel_config_set_dreq(&cfg, DREQ_ADC);

    dma_channel_configure(adc_chan, &cfg,
        capture_buf,    // dst
        &adc_hw->fifo,  // src
        count_of(capture_buf),  // transfer count
        false            // start immediately
    );

    // Tell the DMA to raise IRQ line 0 when the channel finishes a block
    dma_channel_set_irq0_enabled(adc_chan, true);

    // Configure the processor to run dma_handler() when DMA IRQ 0 is asserted
    irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
    irq_set_enabled(DMA_IRQ_0, true);

    // Send core 1 off to start driving the "DAC" whilst we configure the ADC.
    multicore_launch_core1(core1_main);

    dma_channel_start(adc_chan);

    printf("Starting capture\n");
    adc_run(true);

    while (true)
        tight_loop_contents();
}

void core1_main() {
    while (true) {
        sleep_ms(30);
    }
}
