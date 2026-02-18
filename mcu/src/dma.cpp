extern "C" {
#include "hardware/dma.h"
#include "hardware/adc.h"
}

#include "../include/dma.h"
#include "../include/util.h"
#include "../include/pins.h"

int dma_chan;
volatile uint16_t adc_buffer[ADC_BUF_LEN];
volatile uint16_t *adc_buffer_view;
volatile bool adc_buffer_ready = false;

void dma_handler() {
	dma_hw->ints0 = 1u << dma_chan;

	// static uint8_t pointer = 0;
	// pointer = pointer++ % (ADC_BUF_LEN / FFT_HOP_SIZE);

	// if (half) {
	// 	adc_buffer_ready = true;
	// 	adc_buffer_view = &adc_buffer[0];
	// } else {
	// 	adc_buffer_ready = true;
	// 	adc_buffer_view = &adc_buffer[ADC_BUF_LEN / 2];
	// }

	// restart DMA manually if transfer count is zero
	if (!dma_channel_is_busy(dma_chan)) {
		dma_channel_set_read_addr(dma_chan, &adc_hw->fifo, false);
		dma_channel_set_write_addr(dma_chan, adc_buffer, false);
		dma_channel_set_trans_count(dma_chan, ADC_BUF_LEN, true);
	}

}

void init_dma() {
	dma_chan = dma_claim_unused_channel(true);
	dma_channel_config cfg = dma_channel_get_default_config(dma_chan);

	channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
	channel_config_set_read_increment(&cfg, false); // false → always read from ADC FIFO
	channel_config_set_write_increment(&cfg, true); // true → fill buffer sequentially
	channel_config_set_dreq(&cfg, DREQ_ADC);		// DREQ_ADC → trigger when ADC FIFO has data

	channel_config_set_ring(&cfg,
							true, // Write address wraps
							ADC_BUF_EXPONENT);
	
	// channel_config_set_chain_to(&cfg, dma_chan); // chain to self to prevent stopping

	dma_channel_configure(dma_chan, &cfg,
						  adc_buffer,	 // Destination
						  &adc_hw->fifo, // Source
						  FFT_HOP_SIZE,	 // Number of transfers
						  true);		 // Start immediately

	dma_channel_set_irq0_enabled(dma_chan, true);
	irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
	irq_set_enabled(DMA_IRQ_0, true);
}
