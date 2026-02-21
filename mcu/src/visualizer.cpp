#include "../include/visualizer.h"

// █▓▒░ transparency
// ▁▂▃▄▅▆▇██
// 123456789

const char bar[9][4] = {" ", "▏", "▎", "▍", "▌", "▋", "▊", "▉", "█"};

const int16_t volume_offset = -30;
const uint16_t max_bars = 64;
const uint16_t bar_volume = 4096 / max_bars;
const uint8_t empty_lines = 35;
const uint8_t wstr_size = sizeof(bar[0]);
const uint8_t nl_size = sizeof("\n");

static char new_lines[(empty_lines + 1) * nl_size];
static char clear_kitty_screen[8] = "\e[H\e[2J";
static char box_top[(max_bars + 3) * wstr_size + nl_size];
static char box_bot[(max_bars + 3) * wstr_size + nl_size];

const uint16_t bar_size = max_bars * wstr_size;
uint16_t total_size = sizeof(box_top) + sizeof(box_bot);

void init_utils() {
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

char *vol_bar(uint16_t volume) {
	static char output[bar_size];
	output[0] = '\0';
	volume = volume + volume_offset < 0 ? 0 : volume + volume_offset;

	uint16_t full_bars = volume / bar_volume;

	float rest = ((float) volume / bar_volume) - full_bars;
	rest = rest < 0.1 && full_bars == 0 ? 0 : rest;

	// full bars
	for (int i = 0; i < full_bars; i++)
		strcat(output, bar[8]);

	// remainder bar
	strcat(output, bar[(int)(rest * 8)]);

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

	wrap_in_box(output, vol_bar(volume));

	printf("%s%s%s", clear_kitty_screen, new_lines, output);
	// printf("%d\r", volume);
}
