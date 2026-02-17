extern "C" {
#include "hardware/dma.h"
#include "hardware/adc.h"
}

#include "dma.h"

uint8_t frame_a[FRAME_SIZE];
uint8_t frame_b[FRAME_SIZE];

int dma_chan;

void dma_handler() {
	dma_hw->ints0 = 1u << dma_chan;
	frame_ready = true;

	dma_buf = (dma_buf == frame_a) ? frame_b : frame_a;
	dma_channel_set_write_addr(dma_chan, dma_buf, true);
}

void ini_dma() {
    dma_chan = dma_claim_unused_channel(true);
    dma_channel_config cfg = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_8);
    channel_config_set_read_increment(&cfg, false);
    channel_config_set_write_increment(&cfg, true);
    // channel_config_set_dreq(&cfg, spi_get_dreq(SPI_PORT, false)); // RX
	
    dma_channel_configure(dma_chan, &cfg, dma_buf, adc_read, FRAME_SIZE, false);
	dma_channel_set_irq0_enabled(dma_chan, true);
	irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
	irq_set_enabled(DMA_IRQ_0, true);

	dma_channel_start(dma_chan);
}
