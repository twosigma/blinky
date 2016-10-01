/** \file
 * TextScroll demo
 */

#include <Arduino.h>
#include "TextScroll.h"
#include <math.h>
#include <bitset>

#include "font.h"

#define TEAL 0x008080

void TextScroll::begin()
{
	next_row = 0;
	last_time_scroll = millis();

	// max length == 256 characters
	load_string("Eight-by-eight is a neat little board!  ");
}

void TextScroll::load_string(const char* string) {
	buffer_end = *buffer;

	// TODO: kern the characters a little when inserting them into the buffer
	for (int c=0; c < strlen(string); c++) {
		char ord = string[c];
		for (int i=0; i < 8; i++) {
			for (int j=0; j < 8; j++) {
				*buffer_end = font8x8_basic[ord][j] & 1 << i;
				buffer_end++;
			}
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
	if (now - last_time_scroll > 60ul) {
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
				matrix.blend(x, y, 80, TEAL);
		}
	}
}
