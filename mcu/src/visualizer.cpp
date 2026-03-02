#include "../include/visualizer.h"
#include "../include/note_detection.h"
#include "../include/dsp.h"
#include <cmath>
#include <cstring>

// █▓▒░ transparency

const char horizontal_bars[9][4] = {" ", "▏", "▎", "▍", "▌", "▋", "▊", "▉", "█"};
const char hor_left[9][4] = {" ", "▏", "▎", "▍", "▌", "▋", "▊", "▉", "█"};
const char hor_right[9][5] = {" ", "▕", "🮇", "🮈", "▐", "🮉", "🮊", "🮋", "█"};
const char vertical_bars[9][4] = {" ", "▁", "▂", "▃", "▄", "▅", "▆", "▇", "█"};

const int16_t volume_offset = -30;
const uint16_t max_bars = 64;
const uint16_t bar_volume = 4096 / max_bars;
const uint8_t empty_lines = 35;
const uint8_t wstr_size = sizeof(horizontal_bars[0]);
const uint8_t nl_size = sizeof("\n");

static char new_lines[(empty_lines + 1) * nl_size];
static char clear_kitty_screen[8] = "\e[H\e[2J";
static char box_top[(max_bars + 3) * wstr_size + nl_size];
static char box_bot[(max_bars + 3) * wstr_size + nl_size];

const uint16_t bar_size = max_bars * wstr_size;
uint16_t total_size = sizeof(box_top) + sizeof(box_bot);

void init_visualizer() {
	total_size += sizeof(clear_kitty_screen);
	total_size += sizeof(new_lines);
	total_size += sizeof(box_top);
	total_size += sizeof(bar_size);
	total_size += sizeof(box_bot);

	// empty lines
	for (int i = 0; i < empty_lines; i++)
		strcat(new_lines, "\n");

	// box top
	strcat(box_top, "╭");
	for (int i = 0; i < max_bars; i++)
		strcat(box_top, "─");
	strcat(box_top, "╮\n│");

	// box bottom
	strcat(box_bot, "│\n╰");
	for (int i = 0; i < max_bars; i++)
		strcat(box_bot, "─");
	strcat(box_bot, "╯\n");
}

// val must be a float between 0 and 1
void create_vertical_bar(float val) { //
	printf("   %s   \r", vertical_bars[(int)(val * 8)]);
	// strcat(bar, vertical_bars[(int)(val * 8)]);
	// return bar;
}

void build_line(bool *line, const uint16_t length, char *output) {
	for (int i = 0; i < length; i++) {
		if (line[i])
			strcat(output, "█");
		else
			strcat(output, " ");
	}
}

void insert_wchar(const char *input, char *output, uint32_t index) {
	for (int i = 0; i < 3; i++)
		output[i + index] = input[i];
}

void visualize_fft_vertical() {
	const uint8_t height = 40;
	const float highest_freq = 4186.009;
	const float lowest_freq = 55;
	const float frequency_per_bin = (float)SAMPLE_RATE / FFT_SIZE;
	const uint16_t width = highest_freq / frequency_per_bin;

	char output[width * height * wstr_size * 2];
	output[0] = '\0';

	char output_line[width * wstr_size];

	for (int row = height; row > 0; row--) {
		int char_pointer = 0;
		for (int i = 0; i < width; i++) {
			float mag = magnitude_buffer[i] / 20000;
			if (mag > row) {
				insert_wchar("█", output_line, char_pointer);
				char_pointer += 3;
			} else {
				output_line[char_pointer] = '0';
				char_pointer++;
			}
		}

		output_line[char_pointer++] = '\n';
		output_line[char_pointer] = '\0';
		strcat(output, output_line);
	}

	printf("\e[H\e[2J");
	printf("\n%s\n", output);
	sleep_ms(10);
}

void visualize_fft_horizontal() {
	const float highest_freq = 4186.009;
	const float frequency_per_bin = (float)SAMPLE_RATE / FFT_SIZE;
	const uint16_t num_freq = FREQ_MAX / frequency_per_bin;

	normalize_magnitudes(num_freq);
	float magnitude_gain = clamp<float>(0, magnitude_buffer[0] * 6, 1);

	char output[max_bars * 2 * num_freq * wstr_size + 100];
	output[0] = '\0';
	for (int i = 1; i <= num_freq; i += 2) {
		float mag = magnitude_buffer[i] / magnitude_gain;
		float mag1 = magnitude_buffer[i + 1] / magnitude_gain;
		mag = (mag + mag1) / 2;

		mag = mag > 1 ? 1 : mag;
		mag = mag < 0 ? 0 : mag;
		strcat(output, create_horizontal_bar(mag));
		strcat(output, "\n");
	}
	printf("\e[H\e[2J");
	printf("%s\n", output);
}

void print_notes(float *mags) {
	float magnitude_gain = clamp<float>(0, magnitude_buffer[0] * 8, 1);
	float contrast_threshold = magnitude_buffer[0];

	char output[NUM_NOTES * 3 * wstr_size];
	output[0] = '\0';
	for (int i = 0; i < NUM_NOTES; i++) {
		// float mag = mags[i] / magnitude_gain;

		float mag = mags[i];
		if (mags[i] > contrast_threshold)
			mag /= magnitude_gain;
		else
			mag *= magnitude_gain;

		mag = mag > 1 ? 1 : mag;
		mag = mag < 0 ? 0 : mag;

		float half = (mag * 16.0f) / 2.0f;
		strcat(output, hor_right[(int)std::ceil(half)]);
		strcat(output, hor_left[(int)std::floor(half)]);
	}
	printf("%4.2f ", magnitude_buffer[0]);
	printf("%s\n", output);
}

void print_spectrogram(float *mags) {
	const float frequency_per_bin = (float)SAMPLE_RATE / FFT_SIZE;
	const uint16_t num_freq = FREQ_MAX / frequency_per_bin;

	normalize_magnitudes(num_freq);
	float magnitude_gain = clamp<float>(0, magnitude_buffer[0] * 6, 1);

	char output[num_freq * 3 * wstr_size];
	output[0] = '\0';
	for (int i = 1; i <= num_freq; i++) {
		float mag = mags[i] / magnitude_gain;
		// float mag = mags[i];

		// clamp<float>(0, mag, 1);
		mag = mag > 1 ? 1 : mag;
		mag = mag < 0 ? 0 : mag;

		float half = (mag * 16.0f) / 2.0f;
		strcat(output, hor_right[(int)std::ceil(half)]);
		strcat(output, hor_left[(int)std::floor(half)]);
	}
	// printf("%f", magnitude_gain);
	printf("%s\n", output);
}

// val must be a float between 0 and 1
char *create_horizontal_bar(float val) {
	static char output[bar_size];
	output[0] = '\0';

	float full_bar_size = 1.0 / max_bars;
	uint16_t full_bars = val / full_bar_size;

	float rest = ((float)val / full_bar_size) - full_bars;
	rest = rest < 0.1 && full_bars == 0 ? 0 : rest;

	// full bars
	for (int i = 0; i < full_bars; i++)
		strcat(output, horizontal_bars[8]);

	// remainder bar
	strcat(output, horizontal_bars[(int)(rest * 8)]);

	// // spaces after bar
	// for (int i = 0; i < (max_bars - full_bars - 1); i++)
	// 	strcat(output, " ");

	return output;
}

char *create_horizontal_vol_bar(uint16_t volume) {
	static char output[bar_size];
	output[0] = '\0';
	volume = volume + volume_offset < 0 ? 0 : volume + volume_offset;

	uint16_t full_bars = volume / bar_volume;

	float rest = ((float)volume / bar_volume) - full_bars;
	rest = rest < 0.1 && full_bars == 0 ? 0 : rest;

	// full bars
	for (int i = 0; i < full_bars; i++)
		strcat(output, horizontal_bars[8]);

	// remainder bar
	strcat(output, horizontal_bars[(int)(rest * 8)]);

	// spaces after bar
	for (int i = 0; i < (max_bars - full_bars - 1); i++)
		strcat(output, " ");

	return output;
}

void wrap_in_box(char *output, char *input) {
	strcat(output, box_top);
	strcat(output, input);
	strcat(output, box_bot);
}

void print_volume(uint16_t volume) {
	static uint16_t prev_vol = 0;

	if (abs(prev_vol - volume) <= 1)
		return;

	prev_vol = volume;

	char output[total_size];
	output[0] = '\0';

	wrap_in_box(output, create_horizontal_vol_bar(volume));

	printf("%s%s%s", clear_kitty_screen, new_lines, output);
	// printf("%d\r", volume);
}
