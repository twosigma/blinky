#include <Arduino.h>
#include <math.h>
#include "Cycle.h"

Cycle::Cycle()
{
}

static const uint32_t palette[] = {
	0xFFFFFF,
	0x0000FF,
	0x0000FF,
	0x8000FF,
};

static const int palette_size = sizeof(palette)/sizeof(*palette);
static const int MAX_POS = 2 * (LED_ROWS + LED_COLS) - 4;

void Cycle::begin()
{
	for(int i = 0 ; i < num_drops ; i++)
	{
		pos[i] = random(MAX_POS);
		colors[i] = palette[0]; // palette[random(palette_size)];
		speeds[i] = random(4,12) * 0.001;
	}
}

bool Cycle::step(float ax, float ay, float az)
{
	for(int i = 0 ; i < num_drops ; i++)
	{
		float p = pos[i] + speeds[i];

		if (p > MAX_POS)
			p = p - MAX_POS;
		
		pos[i] = p;
	}

	cylon += 0.005;

	return false;
}

void Cycle::draw(RGBMatrix &matrix)
{
	// fade out the whole board
	for(int i = 0 ; i < LED_ROWS ; i++)
		for(int j = 0 ; j < LED_COLS ; j++)
			matrix.blend(i, j, 16, 0, 0, 0);

	int do_flash = false;

	// draw the new positions
	for(int i = 0 ; i < num_drops ; i++)
	{
		int p = pos[i];
		int x = 0;
		int y = 0;

		if (p < LED_ROWS-1)
		{
			x = 0;
			y = p;
		} else
		if (p < LED_ROWS + LED_COLS - 2)
		{
			x = p - (LED_ROWS-1);
			y = LED_COLS-1;
		} else
		if (p < 2 * LED_ROWS + LED_COLS - 3)
		{
			x = LED_ROWS - 1;
			y = 2 * LED_ROWS + LED_COLS - 3 - p;
		} else
		if (p < 2 * LED_ROWS + 2 * LED_COLS - 4)
		{
			x = 2 * LED_ROWS + 2 * LED_COLS - 4 - p;
			y = 0;
		}

		// if one of the dots hits the top square, flash
		if (x == 0 && y == LED_COLS-1)
			do_flash = true;

		matrix.blend(x, y, 64, colors[i]);
	}

	// draw the cylon sweep
	int cpos = int(cylon) % 16;
	int delta = -1;
	if (cpos >= 8)
	{
		cpos = 16 - cpos;
		delta = +1;
	}
	matrix.blend(cpos, cpos+delta, 64, 0xFF0000);
	matrix.blend(cpos, cpos, 255, 0xFF0000);
	matrix.blend(cpos+delta, cpos, 64, 0xFF0000);

	const uint32_t now = millis();
	if (do_flash && now - last_flash_millis > 1000)
	{
		last_flash_millis = now;
		for(int x = 1 ; x < LED_ROWS-1 ; x++)
			for(int y = 1 ; y < LED_ROWS-1 ; y++)
				matrix.blend(x, y, 200, 255, 255, 255);
	}
}
