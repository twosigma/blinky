#include <Arduino.h>
#include "Life.h"

Life::Life()
{
	reset();
}


void Life::begin()
{
	reset();
}

void Life::reset(void)
{
	for(int i = 0 ; i < 8 ; i++)
		board[i] = random(256);
	steps = 0;
	change_steps = 0;
}


int Life::get(int i, int j)
{
	if (i < 0 || i >= LED_ROWS)
		i = (i + LED_ROWS) % LED_ROWS;
	if (j < 0 || j >= LED_COLS)
		j = (j + LED_COLS) % LED_COLS;

	return (board[i] >> j) & 1;
}


bool Life::step(float ax, float ay, float az)
{
#if 0
	// blend out based on the tilt angle
	blend = 5 * (10 - sqrt(az*az));
#else
	blend = 50;
#endif

	return false;
}

void Life::draw(RGBMatrix &matrix)
{
	// only update the board every few steps to keep it slow
	const boolean do_update = steps++ % 16 == 0;

	// compute the new board position
	uint8_t temp[8] = {};
	int change = 0;
	int live = 0;

	// occasionally add a little something to the board
	if (random(100) == 0)
		board[random(8)] ^= 1 << random(8);

	for(int i = 0 ; i < LED_ROWS ; i++)
	{
		for(int j = 0 ; j < LED_COLS ; j++)
		{
			int count = 0
				+ get(i-1, j-1)
				+ get(i-0, j-1)
				+ get(i+1, j-1)
				+ get(i-1, j-0)
				//+ get(i-0, j-0)
				+ get(i+1, j-0)
				+ get(i-1, j+1)
				+ get(i-0, j+1)
				+ get(i+1, j+1)
				;

			if (count == 2)
				temp[i] |= get(i,j) << j;
			else
			if (count == 3)
				temp[i] |= 1 << j;
		}
	}

	for(int i = 0 ; i < LED_ROWS ; i++)
	{
		for(int j = 0 ; j < LED_COLS ; j++)
		{
			int o = (board[i] >> j) & 1;
			int n = (temp[i] >> j) & 1;

			if (n && !o)
			{
				// a brand new one, make it blue
				matrix.blend(i, j, blend/4, 250, 200, 200);
			} else
			if (n && o)
			{
				// an old one, blend it to white
				matrix.blend(i, j, blend/2, 250, 200, 200);
			} else
			if (!n && o)
			{
				// it just died, make it blue
				matrix.blend(i, j, blend/4, 0, 0, 200);
			} else {
				// it is dead, fade to black
				matrix.blend(i, j, blend/4, 0, 0, 0);
			}
		}

		if (board[i] != temp[i])
			change++;
		if (board[i])
			live++;

		if (do_update)
			board[i] = temp[i];
	}

	// if we are stuck on the same one for a while, reset the
	if (change == 0)
	{
		if (change_steps++ > 500)
			reset();
	} else {
		change_steps = 0;
	}

	// every so often shift the whole field diagonally
#if 0
	if (steps % 128 == 0)
	{
		uint8_t old = board[7];
		for(int i = 0 ; i < 7 ; i++)
		{
			uint8_t t = board[i];
			board[i] = (old >> 1) | (old & 1) << 7;
			old = t;
		}
	}
#endif
				

	// reset occasionally anyway
	if (steps > 16000 || live == 0)
		reset();
}
