/** \file
 * TextScroll demo
 */

#include <Arduino.h>
#include "TextScroll.h"
#include <math.h>
#include <bitset>

#include "font.h"

// CUSTOMIZE: Set the text color here, in hex format (similar to coloring HTML.)
static const uint32_t color = 0x008070; // Teal Color 0x008080

void TextScroll::begin()
{
	next_row = 0;
	last_time_scroll = millis();

	// CUSTOMIZE: Set the message here, maximum length is 256 characters!
	load_string("imagine  build  realize  ");
}

void TextScroll::load_string(const char* string) {
	buffer_end = *buffer;
	int prev_col_total = -1;
	int col_total = 0;
	for (int c=0; c < strlen(string); c++) {
		char ord = string[c];
		for (int i=0; i < 8; i++) {
			col_total = 0;
			for (int j=0; j < 8; j++) {
				int val = font8x8_basic[ord][j] & 1 << i;
				col_total += val;
				*buffer_end = val;
				buffer_end++;
			}
			if (col_total == 0 && prev_col_total == 0 && ord != ' ') {
				// remove excessive whitespace between characters unless it's intentional
				buffer_end -= LED_ROWS;
			}
			prev_col_total = col_total;
		}
	}
}

void TextScroll::scroll_step()
{
	for (int i=LED_ROWS-1; i > 0; i--) {
		for (int j=0; j < LED_COLS; j++) {
			screen[i][j] = screen[i-1][j];
		}
	}
	for (int j=0; j < LED_COLS; j++) {
		screen[0][j] = buffer[next_row][j];
	}

	next_row++;
	if (buffer[next_row] > buffer_end) {
		next_row = 0;
	}
}

bool TextScroll::step(float nx, float ny, float nz)
{
	const uint32_t now = millis();
	if (now - last_time_scroll > 100ul) {
		scroll_step();
		last_time_scroll = now;
	}
	// never force redraw
	return false;
}

void TextScroll::draw(RGBMatrix &matrix)
{
	// decay everything to black
	for (int x = 0 ; x < LED_ROWS ; x++)
		for (int y = 0 ; y < LED_COLS ; y++)
			matrix.blend(x, y, 255, 0, 0, 0);

	for (int x = 0 ; x < LED_ROWS ; x++) {
		for (int y = 0 ; y < LED_COLS ; y++) {
			if (screen[x][y])
				matrix.blend(x, y, 80, color);
		}
	}
}
