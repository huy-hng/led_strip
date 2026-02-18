#include <cstdlib>
#include <cstring>
#include <iostream>
#include <pico/time.h>

#include "../include/visualizer.h"

// █▓▒░ transparency
// ▁▂▃▄▅▆▇██
// 123456789

const char bar[9][4] = {" ", "▏", "▎", "▍", "▌", "▋", "▊", "▉", "█"};
const uint16_t max_bars = 32;
const uint16_t bar_volume = 4096 / max_bars;
const uint8_t empty_lines = 70;
const uint8_t wstr_size = sizeof(bar[0]);
const uint8_t nl_size = sizeof("\n");

static char new_lines[(empty_lines + 1) * nl_size];
static char clear_kitty_screen[8] = "\e[H\e[2J";
static char box_top[(max_bars + 3) * wstr_size + nl_size];
static char box_bot[(max_bars + 3) * wstr_size + nl_size];

uint16_t decoration_size = sizeof(box_top) + sizeof(box_bot);

void init_utils() {
	// decoration_size += sizeof(new_lines);
	// decoration_size += sizeof(clear_kitty_screen);

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
	strcat(box_bot, "╯");
}


void print_volume(uint16_t vol) {
	static uint16_t prev_vol = 0;
	
	if (abs(prev_vol - vol) <= 1) return;
	prev_vol = vol;

	int full_bars = vol / bar_volume;

	float remainder = ((float)vol / bar_volume) - full_bars;
	remainder = remainder < 0.1 && full_bars == 0 ? 0 : remainder;
	bool display_remainder = remainder == 0 ? 0 : 1;

	uint16_t bars = (full_bars + display_remainder);
	
	uint16_t bar_size = bars * wstr_size;
	uint16_t spacer_size = (max_bars - bars) * 2;
	uint16_t output_size = decoration_size + bar_size + spacer_size;

	// malloc(size_t size)
	char output[output_size];
	output[0] = '\0';
	strcat(output, box_top);

	// bars
	for (int i = 0; i < full_bars; i++)
		strcat(output, bar[8]);

	// small remainder bar
	strcat(output, bar[(int)(remainder * 8)]);

	// spaces after bar
	for (int i = 0; i < (max_bars - full_bars - display_remainder); i++)
		strcat(output, " ");

	strcat(output, box_bot);

	printf("%s", clear_kitty_screen);
	printf("\n\n\n\n\n\n\n%s\n", output);
}
