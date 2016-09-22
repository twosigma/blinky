#include <Arduino.h>
#include <math.h>
#include "Rain.h"

Rain::Rain()
{
}

static const uint32_t palette[] = {
	0x008080,
	0xa03f00, // dark orange
	// 0xff6080, // nice pink
	0xa0a0a0,
};

static const int palette_size = sizeof(palette)/sizeof(*palette);

void Rain::new_drop(int i, float angle)
{
	// try to add the drops on the side that would be at the top
	float a = angle - 45*M_PI/180 + (random(120) - 60) * M_PI/180;

	int x = LED_ROWS * -cos(a) + LED_ROWS/2;
	int y = LED_COLS * -sin(a) + LED_COLS/2;

/*
	Serial.print("new: ");
	Serial.print(x); Serial.print(' ');
	Serial.print(y); Serial.print(' ');
	Serial.println(0); //angle);
*/

	drops[i][0] = x;
	drops[i][1] = y;
	colors[i] = palette[random(palette_size)];
}


void Rain::begin()
{
	for(int i = 0 ; i < num_drops ; i++)
	{
		drops[i][0] = random(LED_ROWS);
		drops[i][1] = random(LED_COLS);
		colors[i] = palette[0];
	}
}

bool Rain::step(float ax, float ay, float az)
{
	const float dt = 0.001;

	// rotate the angle by 45 degrees to match the LED matrix
	const float mag = sqrt(ax*ax+ay*ay) * dt;
	const float angle = atan2(ax, -ay);
	const float dx = sin(angle + 45 * M_PI/180) * mag;
	const float dy = -cos(angle + 45 * M_PI/180) * mag;

/*
	Serial.print(ax); Serial.print(' ');
	Serial.print(ay); Serial.print(' ');
	Serial.println(angle * 180 / M_PI);
*/

	for(int i = 0 ; i < num_drops ; i++)
	{
		float x = drops[i][0] += dx;
		float y = drops[i][1] += dy;

		// if they have drifted off screen, create a new one
		if (x < -8 || x > LED_ROWS+8
		||  y < -8 || y > LED_COLS+8)
			new_drop(i, angle);
	}

	return false;
}

void Rain::draw(RGBMatrix &matrix)
{
	// fade out the whole board
	for(int i = 0 ; i < LED_ROWS ; i++)
		for(int j = 0 ; j < LED_COLS ; j++)
			matrix.blend(i, j, 16, 0, 0, 0);

	// draw the new positions
	for(int i = 0 ; i < num_drops ; i++)
	{
		const float px = drops[i][0];
		const float py = drops[i][1];

		const int x = px;
		const int y = py;
/*
		const int xf = (px - x) * 256;
		const int yf = (py - y) * 256;

		matrix.blend(x+0, y+0, sqrt((256 - xf)*(256-yf)), colors[i]);
		matrix.blend(x+0, y+1, sqrt((256 - xf)*(yf)), colors[i]);
		matrix.blend(x+1, y+0, sqrt((xf)*(256-yf)), colors[i]);
		matrix.blend(x+1, y+1, sqrt((xf)*(yf)), colors[i]);
*/
		matrix.blend(x+0, y+0, 64, colors[i]);
	}
}
