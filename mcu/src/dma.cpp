#include "../include/dma.h"

int dma_chan;
volatile uint16_t adc_buffer[ADC_BUF_LEN];
volatile uint8_t write_index = 0;
volatile uint8_t read_index = 0;

fft_frame_ptrs_t frame_ptrs[NUM_FRAMES];

void set_frame_ptrs(uint8_t frame_id, uint32_t start_idx) {
	fft_frame_ptrs_t *frame = &frame_ptrs[frame_id];

	if (start_idx + FFT_SIZE <= ADC_BUF_LEN) {
		// Contiguous
		frame->chunks[0] = &adc_buffer[start_idx];
		frame->lengths[0] = FFT_SIZE;
		frame->num_chunks = 1;
	} else {
		// Wrap occurs
		uint32_t first_len = ADC_BUF_LEN - start_idx;
		uint32_t second_len = FFT_SIZE - first_len;

		frame->chunks[0] = &adc_buffer[start_idx];
		frame->lengths[0] = first_len;

		frame->chunks[1] = &adc_buffer[0];
		frame->lengths[1] = second_len;

		frame->num_chunks = 2;
	}
}

void dma_irq_handler() {
	// Clear the interrupt request.
	dma_hw->ints0 = 1u << dma_chan;

	uint8_t next_write_index = (write_index + 1) % NUM_FRAMES;
	if (next_write_index == read_index) {
		println("--------dsp lagging, dropping frame--------");
		return; // DSP lagging → drop frame
	}

	uint32_t new_start = (frame_ptrs[(next_write_index + NUM_FRAMES - 1) % NUM_FRAMES].chunks[0] - adc_buffer) + FFT_HOP_SIZE;
	new_start %= ADC_BUF_LEN;

	set_frame_ptrs(next_write_index, new_start);

	write_index = next_write_index;
	multicore_fifo_push_blocking(next_write_index);

	// retrigger dma (so it doesn't stop)
	dma_channel_set_read_addr(dma_chan, &adc_hw->fifo, true);
}

void init_dma() {
	dma_chan = dma_claim_unused_channel(true);
	dma_channel_config cfg = dma_channel_get_default_config(dma_chan);

	channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
	channel_config_set_read_increment(&cfg, false); // false → always read from ADC FIFO
	channel_config_set_write_increment(&cfg, true); // true → fill buffer sequentially
	channel_config_set_dreq(&cfg, DREQ_ADC);		// DREQ_ADC → trigger when ADC FIFO has data

	channel_config_set_ring(&cfg, true, ADC_BUF_EXPONENT);

	dma_channel_configure(dma_chan, &cfg,
						  adc_buffer,	 // Destination
						  &adc_hw->fifo, // Source
						  FFT_HOP_SIZE,	 // Number of transfers
						  true);		 // Start immediately

	dma_channel_set_irq0_enabled(dma_chan, true);
	irq_set_exclusive_handler(DMA_IRQ_0, dma_irq_handler);
	irq_set_enabled(DMA_IRQ_0, true);
}
