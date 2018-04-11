/*
 * CM-2 random and pleasing simulator.
 *
 * Each cube has 20 slots with 1" spacing, 8 LEDs, 4 I/O, 8 LEDs.
 * Each LED board has 32 vertical LEDs with 0.5" spacing.
 * Reference photo: https://www.flickr.com/photos/osr/32907925763/in/album-72157680241484140/
 * Description from Dan Aronson:

Cliff lasser did the version for the cm1, I did the version for the cm2.
No communication. During the microprocessor idle loop there was code to
step through memory and then send the bitwise or of the memory for each
processor on the chip to the led. All random and pleasing did for the cm2
was to zero out all memory and them randomize each bit of memory (1 or 0)
in the memory for processor 0 on each chip (and the idle loop would take
care of the rest).

Pretty sure that p was 50% when I first adapted this, I would try t between
100 and 250 milliseconds.

 */

#include "RandomAndPleasing.h"
#include <Arduino.h>

RandomAndPleasing::RandomAndPleasing()
{
}

void RandomAndPleasing::begin()
{
	this->p = 40;
	this->led_rate_ms = 90;
}

bool RandomAndPleasing::step(float ax, float ay, float az)
{
	long now = millis();
	if (now - last_update < led_rate_ms)
		return true;

	last_update = now;
		
	for(int x = 0 ; x < LED_ROWS ; x++)
	{
		for(int y = 0 ; y < LED_COLS ; y++)
		{
			led[x][y] = random(100) > this->p;
		}
	}

	return true;
}


void RandomAndPleasing::draw(RGBMatrix &matrix)
{
	for(int x = 0 ; x < LED_ROWS ; x++)
	{
		for(int y = 0 ; y < LED_COLS ; y++)
		{
			if (led[x][y])
				//matrix.set(x, y, 0, 0, 0);
				matrix.blend(x, y, 40, 0, 0, 0);
			else
				matrix.set(x, y, 128, 0, 0);
		}
	}
}

