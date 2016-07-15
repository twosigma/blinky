#include <Arduino.h>
#include <math.h>
#include "Pov.h"

#if 1
// nyancat is 10 pixels tall
#include "nyan-cat.h"

#else
#define ROW(r,g,b) \
	r, g, b, \
	r, g, b, \
	r, g, b, \
	r, g, b, \
	r, g, b, \
	r, g, b, \
	r, g, b, \
	r, g, b, \
	r, g, b, \
	r, g, b \

static const uint8_t animation[] = {
	ROW(255,255,255),
	ROW(255,255,255),
	ROW(255,0,0),
	ROW(255,0,0),
	ROW(255,0,0),
	ROW(0,255,0),
	ROW(0,255,0),
	ROW(0,255,0),
	ROW(0,0,255),
	ROW(0,0,255),
	ROW(0,0,255),
	ROW(0,0,0),
	ROW(255,255,255),
	ROW(0,0,0),
	ROW(255,255,255),
};
#endif

static const unsigned animation_height = 10;
static const unsigned num_frames = sizeof(animation) / (animation_height*3);

Pov::Pov()
{
}

void Pov::begin()
{
	enabled = false;
	a_smooth = 0;
	pos = 0;
	vel = 0;
	last_dir = 0;
}

bool Pov::step(float ax, float ay, float az)
{
	const float a = (ax - ay) / 2;
	const uint32_t now = micros();
	const float dt = (now - last_step_micros) * 1e-6;
	last_step_micros = now;

	const float a_smooth_old = a_smooth;

	a_smooth = (a_smooth * 7 + a) / 8;

	// track if the acceleration direction has changed versus our
	// previous filter.  If so, then we reverse velocity
	int dir = last_dir;
	if (a_smooth - a_smooth_old > 0)
		dir = 1;
	else
	if (a_smooth - a_smooth_old < 0)
		dir = -1;

	if (dir != last_dir)
	{
		// they have switched directions; reset our
		// velocity estimate and start moving back
		vel = 0;
		if (dir < 0)
		{
			pos = 0;
		} else {
			// we've reached the right side of the stroke;
			// update our estimate for the length of the swipe
			max_pos = (max_pos * 1 + pos) / 2;
			pos = max_pos;
		}
	}

	last_dir = dir;

	// integrate the acclerometers to generate the 
	// velocity and position estimate
	vel += a_smooth * dt;
	pos += vel * dt;

	Serial.print(a_smooth); Serial.print(' ');
	Serial.print(vel); Serial.print(' ');
	Serial.print(pos); Serial.print(' ');
	Serial.print(dir); Serial.print(' ');

	return false;
}

void Pov::draw(RGBMatrix &matrix)
{
	const int frame_num = (pos * num_frames) / max_pos;
	Serial.println(frame_num);

	if (frame_num < 0 || frame_num >= num_frames)
	{
		// draw blank
		for(int i = 0 ; i < LED_ROWS ; i++)
			matrix.set(i, 0, 0x000000);
	} else {
		const uint8_t * const frame = &animation[frame_num * animation_height * 3];

		for(int i = 0 ; i < LED_COLS ; i++)
		{
			matrix.set(0, i,
				frame[(i+1) * 3 + 0],
				frame[(i+1) * 3 + 1],
				frame[(i+1) * 3 + 2]
			);
		}

		matrix.show8();
	}
}
