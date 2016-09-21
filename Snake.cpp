#include <Arduino.h>
#include <math.h>
#include "Snake.h"

Snake::Snake()
{
}

static const uint32_t palette[] = {
	0x008080, // teal, the traces
	0xa0a0a0, // the head
	0xa03f00, // food, dark orange
	// 0xff6080, // nice pink
};

void Snake::new_food()
{
	while(1)
	{
		fx = random(size);
		fy = random(size);

		if (age[fx][fy] != 0)
			continue;
		if (fx == px && fy == py)
			continue;

		// empty space!
		break;
	}
}

bool Snake::out_of_bounds(int x, int y)
{
	if (x < 0 || x >= size || y < 0 || y >= size)
		return true;
	return false;
}

bool Snake::space_free(int x, int y)
{
	if (out_of_bounds(x,y))
		return false;
	return age[x][y] == 0;
}

bool Snake::is_food(int x, int y)
{
	if (out_of_bounds(x,y))
		return false;
	return x == fx && y == fy;
}


void Snake::begin()
{
	for(int x = 0 ; x < size ; x++)
	{
		for(int y = 0 ; y < size ; y++)
			age[x][y] = 0;
	}

	last_draw = millis();
	length = 1;
	px = random(size);
	py = random(size);

	age[px][py] = 1;
	vx = 0;
	vy = 1;

	dead = 0;
	flash = 1;

	new_food();
}

bool Snake::step(float ax, float ay, float az)
{
	// don't update while doing animations
	if (flash || dead)
		return true;

	const unsigned long now = millis();
	int dt = now - last_draw;
	if (dt < 125)
		return true;
	last_draw = now;

	Serial.print(px); Serial.print(' ');
	Serial.print(py); Serial.print(' ');
	Serial.print(vx); Serial.print(' ');
	Serial.print(vy); Serial.print(' ');
	Serial.println();

	// is the snake on the food?
	if (px == fx && py == fy)
	{
		new_food();
		length++;
		flash = 8;
	}

	// age out any old squares
	for(int x = 0 ; x < size ; x++)
	{
		for(int y = 0 ; y < size ; y++)
		{
			uint8_t a = age[x][y];
			if (a == 0)
				continue;
			if (++a > length)
				a = 0;
			age[x][y] = a;
		}
	}

	// direction to food
	const int dx = fx - px;
	const int dy = fy - py;
	const int adx = abs(dx);
	const int ady = abs(dy);

	int vdx = vx;
	int vdy = vy;
	if (adx > ady)
	{
		vdx = dx > 0 ? 1 : -1;
		vdy = 0;
	} else
	{
		vdx = 0;
		vdy = dy > 0 ? 1 : -1;
	}

	// check for food near by
	if (is_food(px+0, py+1)) { vx =  0; vy = +1; } else
	if (is_food(px+0, py-1)) { vx =  0; vy = -1; } else
	if (is_food(px+1, py+0)) { vx = +1; vy =  0; } else
	if (is_food(px-1, py+0)) { vx = -1; vy =  0; } else
	if (random(4) != 0 && space_free(px + vdx, py + vdy)) { vx = vdx; vy = vdy; } else
	if (random(8) != 0 && space_free(px+0, py+1)) { vx =  0; vy = +1; } else
	if (random(8) != 0 && space_free(px+0, py-1)) { vx =  0; vy = -1; } else
	if (random(8) != 0 && space_free(px+1, py+0)) { vx = +1; vy =  0; } else
	if (random(8) != 0 && space_free(px-1, py+0)) { vx = -1; vy =  0; } else
	if (space_free(px+0, py+1)) { vx =  0; vy = +1; } else
	if (space_free(px+0, py-1)) { vx =  0; vy = -1; } else
	if (space_free(px+1, py+0)) { vx = +1; vy =  0; } else
	if (space_free(px-1, py+0)) { vx = -1; vy =  0; } else
	{
		// oh well, we are going to crash
	}


	int npx = px + vx;
	int npy = py + vy;

	if (space_free(npx, npy))
	{
		// we are good!
		px = npx;
		py = npy;
		age[px][py] = 1;
	} else {
		// out of bounds or hit something. dead!
		dead = 255;
	}

	return true;
}

void Snake::draw(RGBMatrix &matrix)
{
	if (dead)
	{
		// fade everything
		for(int x = 0 ; x < size ; x++)
			for(int y = 0 ; y < size ; y++)
				matrix.blend(x, y, 1, 0);
		if (--dead == 0)
			begin();
	} else
	if (flash)
	{
		// flash white
		for(int x = 0 ; x < size ; x++)
			for(int y = 0 ; y < size ; y++)
				matrix.blend(x, y, 8, 0xFFFFFF);
		flash--;
	} else {
		for(int x = 0 ; x < size ; x++)
		{
			for(int y = 0 ; y < size ; y++)
			{
				matrix.blend(x, y, 2, 0);

				int a = age[x][y];
				if (a == 0)
					continue;

				int dim = 128; //(255 * (length - a)) / length;
				matrix.blend(x, y, dim, palette[0]);
			}
		}

		matrix.blend(px, py, 128, palette[1]);
		matrix.blend(fx, fy, 128, palette[2]);
	}
}
