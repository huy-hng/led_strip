#include <Arduino.h>

extern "C" {
#include "hardware/spi.h"
#include "hardware/dma.h"
}

#define FRAME_SIZE 12

uint8_t rx_buf[FRAME_SIZE];
int dma_chan;

uint32_t frames = 0;
uint32_t frames_last = 0;
uint32_t sync_err = 0;
uint32_t checksum_err = 0;
uint32_t drop_err = 0;
uint32_t payload_err = 0;
uint8_t expected_id = 0;

void reset_data() {
	frames = 0;
	frames_last = 0;
	sync_err = 0;
	checksum_err = 0;
	drop_err = 0;
	payload_err = 0;
	expected_id = 0;
}

void dma_init_spi() {
    dma_chan = dma_claim_unused_channel(true);
    dma_channel_config cfg = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_8);
    channel_config_set_read_increment(&cfg, false);
    channel_config_set_write_increment(&cfg, true);
    channel_config_set_dreq(&cfg, spi_get_dreq(spi0, false));
	
    dma_channel_configure(
        dma_chan,
        &cfg,
        rx_buf,
        &spi_get_hw(spi0)->dr,
        FRAME_SIZE,
        false
    );
}

void start_dma_receive() {
    dma_channel_set_write_addr(dma_chan, rx_buf, false);
    dma_channel_set_trans_count(dma_chan, FRAME_SIZE, true);
}

void print_buffer() {
	for (int i = 0; i < FRAME_SIZE; i++) {
		Serial.print(rx_buf[i]);
		Serial.print(", ");
	}
	Serial.println();
}

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

void communication_setup() {
    delay(2000);

    // SPI slave setup
    spi_init(spi0, 50 * 1000 * 1000);
    spi_set_slave(spi0, true);
	spi_set_format(
		spi0,
		8,              // bits
		SPI_CPOL_0,
		// phase needs to be 1 as seen here https://github.com/raspberrypi/pico-examples/issues/115#issuecomment-1051222803
		SPI_CPHA_1,
		SPI_MSB_FIRST
	);

    gpio_set_function(16, GPIO_FUNC_SPI); // mosi
    gpio_set_function(17, GPIO_FUNC_SPI); // chip select
    gpio_set_function(18, GPIO_FUNC_SPI); // sclk
    gpio_set_function(19, GPIO_FUNC_SPI); // miso

    dma_init_spi();

    Serial.println("Pico SPI receiver ready");
}

int idx = 0;
void print_spi() {
	if (!spi_is_readable(spi0))
		return;

	uint8_t b = spi_get_hw(spi0)->dr;
	rx_buf[idx++] = b;

	if (idx == FRAME_SIZE) {               // full frame received
		idx = 0;
		if (rx_buf[0] == 0xA5 && rx_buf[1] == 0x5A) {
			// valid frame, print or process
			Serial.print("Frame "); Serial.print(rx_buf[2]);
			Serial.print(" | Payload: ");
			for (int i=3;i<11;i++) Serial.print(rx_buf[i], HEX), Serial.print(" ");
			Serial.println();
		} else {
		}
	}
}

void write_dma() {
	start_dma_receive();
    dma_channel_wait_for_finish_blocking(dma_chan);

    if (validate_frame(rx_buf)) {
		Serial.print(sync_err);
        Serial.print(" Frame ");
        Serial.print(rx_buf[2]);
        Serial.print(" | Payload: ");

        for (int i = 3; i < FRAME_SIZE; i++) {
			Serial.print(rx_buf[i], HEX);
			Serial.print(" ");
		}

		Serial.println();

	} else {


		Serial.print(sync_err);
		Serial.print(" Bad sync: ");
		for (int i = 0; i < FRAME_SIZE; i++) {
			Serial.print(rx_buf[i]);
			Serial.print(", ");
		}
		Serial.println();
	}
}

void check_stability() {
	static bool error = false;
	while (spi_is_readable(spi0)) {
		rx_buf[idx++] = spi_get_hw(spi0)->dr;

		if (!error && idx == FRAME_SIZE) {
			if (!validate_frame(rx_buf)) {
				error = true;
			}
			idx = 0;
		} else if (error && rx_buf[idx-1] == 0x5A && rx_buf[idx-2] == 0xA5) {
			idx = 2;
			expected_id = rx_buf[2] + 1;
			error = false;
		}
	}

	// start_dma_receive();
	// dma_channel_wait_for_finish_blocking(dma_chan);

	// validate_frame(rx_buf);

    // static uint32_t last = 0;

    // if (millis() - last > 1000) {
    //     last = millis();
		
  //       Serial.println("-----");
  //       Serial.print("Total Frames: "); Serial.println(frames);
		// Serial.print("Frames since last print: "); Serial.println(frames - frames_last);
  //       Serial.print("Sync: "); Serial.println(sync_err);
  //       Serial.print("Checksum: "); Serial.println(checksum_err);
  //       Serial.print("Drops: "); Serial.println(drop_err);
  //       Serial.print("Payload: "); Serial.println(payload_err);
		// float error_rate = (sync_err + checksum_err + drop_err + payload_err) / (float)frames;
		// Serial.print("Error Rate: "); Serial.print(error_rate * 100, 4); Serial.println("%");

		// if (frames - frames_last == 0)
		// 	reset_data();

		// frames_last = frames;
    // }
}

uint32_t step = 0;
double cum_time = 0;

void communication_loop() {
	static uint32_t last = 0;

	int start = micros();

	// write_dma();
	check_stability();

	cum_time += micros() - start;

	if (millis() - last > 1000) {
		last = millis();

		Serial.print("loop takes on average: ");
		Serial.print(cum_time / (double) step, 4);
		Serial.println("us");
	}
	step++;
}
