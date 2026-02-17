#include <Arduino.h>

extern "C" {
#include "hardware/spi.h"
#include "hardware/dma.h"
}

#define FRAME_SIZE 12
#define SPI_PORT spi0

#define PIN_MOSI 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MISO 19

uint8_t frame_a[FRAME_SIZE];
uint8_t frame_b[FRAME_SIZE];

volatile bool frame_ready = false;

uint8_t *rx_buf = frame_a;
int dma_chan;

uint32_t frames = 0;
uint32_t frames_last = 0;
uint32_t sync_err = 0;
uint32_t checksum_err = 0;
uint32_t drop_err = 0;
uint32_t payload_err = 0;
uint8_t expected_id = 0;

uint32_t idx = 0;

enum {
    WAIT_A5,
    WAIT_5A,
    COLLECT
} state = WAIT_A5;

//----------------------------------------------Setup-----------------------------------------------

void dma_handler() {
	dma_hw->ints0 = 1u << dma_chan;
	frame_ready = true;

	rx_buf = (rx_buf == frame_a) ? frame_b : frame_a;
	dma_channel_set_write_addr(dma_chan, rx_buf, true);
}

void dma_init_spi() {
    dma_chan = dma_claim_unused_channel(true);
    dma_channel_config cfg = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_8);
    channel_config_set_read_increment(&cfg, false);
    channel_config_set_write_increment(&cfg, true);
    channel_config_set_dreq(&cfg, spi_get_dreq(SPI_PORT, false)); // RX
	
    dma_channel_configure(dma_chan, &cfg, rx_buf, &spi_get_hw(SPI_PORT)->dr, FRAME_SIZE, false);
	dma_channel_set_irq0_enabled(dma_chan, true);
	irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
	irq_set_enabled(DMA_IRQ_0, true);

	dma_channel_start(dma_chan);
}

void communication_setup() {
    delay(2000);

    // SPI slave setup
    spi_init(SPI_PORT, 50 * 1000 * 1000);
    spi_set_slave(SPI_PORT, true);
	spi_set_format(
		SPI_PORT,
		8,              // bits
		SPI_CPOL_0,
		// phase needs to be 1 as seen here https://github.com/raspberrypi/pico-examples/issues/115#issuecomment-1051222803
		SPI_CPHA_1,
		SPI_MSB_FIRST
	);

    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);

    dma_init_spi();

    Serial.println("Pico SPI receiver ready");
}

//----------------------------------------------Debug-----------------------------------------------

void reset_data() {
	frames = 0;
	frames_last = 0;
	sync_err = 0;
	checksum_err = 0;
	drop_err = 0;
	payload_err = 0;
	expected_id = 0;
}

void print_successful_buffer() {
	Serial.print(" Frame ");
	Serial.print(rx_buf[2]);
	Serial.print(" | Payload: ");

	for (int i = 3; i < FRAME_SIZE; i++) {
		Serial.print(rx_buf[i], HEX);
		Serial.print(" ");
	}

	Serial.println();

}

void print_buffer() {
	for (int i = 0; i < FRAME_SIZE; i++) {
		Serial.print(rx_buf[i]);
		Serial.print(", ");
	}
	Serial.println();
}

void print_error_rate(uint32_t error) {
	Serial.println((error / (float) frames)*100, 3);
}

void print_validation_stats() {
	Serial.println("--------");
	Serial.print("Total Frames: "); Serial.println(frames);
	Serial.print("Frames since last print: "); Serial.println(frames - frames_last);

	// Serial.print("Sync:     "); print_error_rate(sync_err);
	// Serial.print("Checksum: "); print_error_rate(checksum_err);
	// Serial.print("Drops:    "); print_error_rate(drop_err);
	// Serial.print("Payload:  "); print_error_rate(payload_err);

	float error_rate = (sync_err + checksum_err + drop_err + payload_err) / (float)frames;
	Serial.print("Total Error Rate: "); Serial.print(error_rate * 100, 3); Serial.println("%");
}


//---------------------------------------------helpers----------------------------------------------

bool validate_frame(uint8_t *buf) {
	frames++;

	// sync
    if (buf[0] != 0xA5 || buf[1] != 0x5A) {
        sync_err++;
        return false;
    }

    // checksum
    uint8_t chk = 0;
    for (int i=0;i<FRAME_SIZE;i++)
        if (i != 3) chk ^= buf[i];

    if (chk != buf[3]) {
        checksum_err++;
        return false;
    }

    // frame continuity
    if (buf[2] != expected_id) {
        drop_err++;
	}

    expected_id = buf[2] + 1;

    // payload pattern
    for (int i=0; i<8; i++) {
        if (buf[4 + i] != ((buf[2] + i) & 0xFF)) {
            payload_err++;
            break;
        }
    }

    return true;
}

void read_spi() {
	while (spi_is_readable(spi0)) {
		
		uint8_t b = spi_get_hw(spi0)->dr;

		switch(state) {
			case WAIT_A5:
				if (b == 0xA5) {
					rx_buf[0] = b;
					state = WAIT_5A;
				}
				break;
			case WAIT_5A:
				if (b == 0x5A) {
					rx_buf[1] = b;
					idx = 2;
					state = COLLECT;
				} else {
					state = WAIT_A5;
				}
				break;
			case COLLECT:
				rx_buf[idx++] = b;
				if (idx == FRAME_SIZE) {
					if (frames < 100000)
						expected_id = rx_buf[2];

					if (!validate_frame(rx_buf))
						expected_id = rx_buf[2] + 1;

					state = WAIT_A5;
					idx = 0;
				}
				break;
		}
	}
}

//--------------------------------------------Main loop---------------------------------------------

void check_stability() {
	// read_spi();

	if (frame_ready) {
        frame_ready = false;
		validate_frame(rx_buf);
    }


    static uint32_t last = 0;
    if (millis() - last > 1000) {
        last = millis();

		print_validation_stats();

		if (frames - frames_last == 0)
			reset_data();

		frames_last = frames;
    }

}

// uint32_t step = 0;
// double cum_time = 0;

void communication_loop() {
	// static uint32_t last = 0;
	// int start = micros();


	// write_dma();
	check_stability();

	// cum_time += micros() - start;

	// if (millis() - last > 1000) {
	// 	last = millis();

	// 	Serial.print("loop takes on average: ");
	// 	Serial.print(cum_time / (double) step, 3);
	// 	Serial.println("us");
	// 	cum_time = 0;
	// }
	// step++;
}
