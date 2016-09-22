/** \file
 * Bubble level demo
 */

#include "Bubble.h"
#include <math.h>


void Bubble::begin()
{
}

bool Bubble::step(float nx, float ny, float nz)
{
	// smooth the values
	const float smooth = 1;
	ax = (ax * smooth + nx) / (smooth + 1);
	ay = (ay * smooth + ny) / (smooth + 1);
	az = (az * smooth + nz) / (smooth + 1);

	// never force redraw
	return false;
}

void Bubble::draw(RGBMatrix &matrix)
{
	// don't draw the bubble once we pass 45 degree or so
	int blend = (fabs(az)) * 50 / (2 * 9.81);

	// rotate the x y by 45 degrees
	const float rx = ax * 0.851 - ay * 0.525;
	const float ry = ay * 0.851 + ax * 0.525;

	const float row = rx * (LED_ROWS / (25));
	const float col = ry * (LED_COLS / (25));

	// decay everything to black
	for(int x = 0 ; x < LED_ROWS ; x++)
		for(int y = 0 ; y < LED_COLS ; y++)
			matrix.blend(x, y, 8, 0, 0, 0);

	for(int x = 0 ; x < LED_ROWS ; x++)
	{
		for(int y = 0 ; y < LED_COLS ; y++)
		{
			float dx = rx - (x - LED_ROWS/2);
			float dy = ry - (y - LED_COLS/2);
			int dist = 255 - sqrt(dx*dx + dy*dy) * 128;
			if (dist < 0)
				continue;

			matrix.blend(x, y, dist, 255, 0, 0);
		}
	}
}
