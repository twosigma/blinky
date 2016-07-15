/** \file
 * Driver for the Blinky Badge RGB matrix.
 *
 */
#include <Arduino.h>
#include "RGBMatrix.h"

#ifdef CONFIG_FEATHER
#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel neopixels = Adafruit_NeoPixel(LED_ROWS * LED_COLS, 15, NEO_GRB);
#endif

RGBMatrix::RGBMatrix()
{
	this->clear();
}


void RGBMatrix::clear()
{
	for(int i = 0 ; i < LED_ROWS ; i++)
		for(int j = 0 ; j < LED_COLS ; j++)
			set(i, j, 0, 0, 0);
}

void RGBMatrix::begin()
{
#ifndef CONFIG_FEATHER
	// Serial communication to RGB matrix
	Serial1.begin( 230400 );
	this->clear();
	this->show();
	this->show();
#else
	// feather uses NeoPixel hat
	neopixels.begin();
	neopixels.setBrightness(32);
#endif
}


void RGBMatrix::set(
	int row,
	int col,
	int r,
	int g,
	int b
)
{
	if (row < 0 || row >= LED_ROWS)
		return;

	if (col < 0 || col >= LED_COLS)
		return;
	
	if(r == 255)
		r = 254;
	if(g == 255)
		g = 254;
	if(b == 255)
		b = 254;

	uint8_t * const p = &data[3*(row*LED_COLS + col)];
	p[0] = r;
	p[1] = g;
	p[2] = b;
}

void RGBMatrix::blend(
	int row,
	int col,
	int blend,
	int r,
	int g,
	int b
)
{
	if (row < 0 || row >= LED_ROWS)
		return;

	if (col < 0 || col >= LED_COLS)
		return;

	if (blend < 0) blend = 0;
 	if (blend > 255) blend = 255;
	
	if(r >= 255) r = 254;
	if(g >= 255) g = 254;
	if(b >= 255) b = 254;
	if(r < 0) r = 0;
	if(g < 0) g = 0;
	if(b < 0) b = 0;

	uint8_t * const p = &data[3*(row*LED_COLS + col)];
	r = (p[0] * (255 - blend) + r * blend) / 256;
	g = (p[1] * (255 - blend) + g * blend) / 256;
	b = (p[2] * (255 - blend) + b * blend) / 256;

	if(r >= 255) r = 254;
	if(g >= 255) g = 254;
	if(b >= 255) b = 254;
	if(r < 0) r = 0;
	if(g < 0) g = 0;
	if(b < 0) b = 0;

	p[0] = r;
	p[1] = g;
	p[2] = b;
}

void RGBMatrix::blend(
	int row,
	int col,
	int blend,
	uint32_t rgb
)
{
	this->blend(
		row,
		col,
		blend,
		(rgb >> 16) & 0xFF,
		(rgb >>  8) & 0xFF,
		(rgb >>  0) & 0xFF
	);
}

void RGBMatrix::set(
	int row,
	int col,
	uint32_t rgb
)
{
	this->set(
		row,
		col,
		(rgb >> 16) & 0xFF,
		(rgb >>  8) & 0xFF,
		(rgb >>  0) & 0xFF
	);
}


void RGBMatrix::show()
{
#ifndef CONFIG_FEATHER
	// for the Blinkin Labs hardware
	for(int i = 0; i < LED_ROWS*LED_COLS*LED_BYTES_PER_PIXEL; i++)
	{
		int c = data[i];
		// scale to 0-170 to fix a PWM bug on rev B hardware
		//c = (170 * c) / 256;
		Serial1.print(char(c));
	}

	Serial1.print(char(255));
#else
	// for the adafruit feather
	unsigned offset = 0;
	for(int row = 0 ; row < LED_ROWS ; row++)
	{
		for(int col = 0 ; col < LED_COLS ; col++, offset += 3)
		{
			neopixels.setPixelColor(
				row + col * LED_ROWS,
				data[offset + 0],
				data[offset + 1],
				data[offset + 2]
			);
		}
	}

	neopixels.show();
#endif
}

void RGBMatrix::show8()
{
#ifndef CONFIG_FEATHER
	// for the Blinkin Labs hardware
	Serial1.write(&data[0], LED_ROWS * LED_BYTES_PER_PIXEL);
	Serial1.print(char(255));
#else
	// for the adafruit feather
	unsigned offset = 0;
	for(int row = 0 ; row < LED_ROWS ; row++)
	{
		for(int col = 0 ; col < LED_COLS ; col++, offset += 3)
		{
			neopixels.setPixelColor(
				row + col * LED_ROWS,
				data[offset + 0],
				data[offset + 1],
				data[offset + 2]
			);
		}
	}

	neopixels.show();
#endif
}
