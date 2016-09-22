#include <Arduino.h>
#include <math.h>
#include "Snake.h"

Snake::Snake()
{
}

static const uint32_t palette[] = {
	0x008080, // teal, the traces
	0xffffff, // white for the head
	//0x8000c0, // purple for the head
	0xa03f00, // food, dark orange
	// 0xff6080, // nice pink
};

void Snake::new_food()
{
	int too_close_tries = 4;

	while(1)
	{
		fx = random(size);
		fy = random(size);

		if (age[fx][fy] != 0)
			continue;

		// don't put it on the same as the player
		if (fx == px && fy == py)
			continue;

		// try to avoid the edges
		if ((fx == 0 || fy == 0 || fx == size-1 || fy == size-1) && too_close_tries-- > 0)
			continue;

		// try to move it a ways away
		int dx = fx - px;
		int dy = fy - py;
		int dist2 = dx*dx + dy*dy;
		if (dist2 <= 2 && too_close_tries-- > 0)
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

	offset = 0;
	path[offset++] = px * size + py;
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
	if (dt < 250 - 4 * length)
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
		flash = 8;
		length++;

		// go ahead and signal that we've found it
		return true;
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
	if (random(8) != 0 && space_free(px + vdx, py + vdy)) { vx = vdx; vy = vdy; } else
	if (random(12) != 0 && space_free(px+0, py+1)) { vx =  0; vy = +1; } else
	if (random(12) != 0 && space_free(px+0, py-1)) { vx =  0; vy = -1; } else
	if (random(12) != 0 && space_free(px+1, py+0)) { vx = +1; vy =  0; } else
	if (random(12) != 0 && space_free(px-1, py+0)) { vx = -1; vy =  0; } else
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
		path[offset++] = px * size + py;
		if (offset > size * size)
			offset = 0;
	} else {
		// out of bounds or hit something. dead!
		dead = 255;
		flash = 16;
	}

	return true;
}


void Snake::draw_path(RGBMatrix &matrix, uint32_t color)
{
	for(int x = 0 ; x < size ; x++)
	{
		for(int y = 0 ; y < size ; y++)
		{
			matrix.blend(x, y, 2, 0);

			int a = age[x][y];
			if (a == 0)
				continue;

			int dim = 128; //(255 * (length - a)) / length;
			matrix.blend(x, y, dim, color);
		}
	}
}

void Snake::draw(RGBMatrix &matrix)
{
	if (flash)
	{
		// flash white around the 
		for(int x = -flash/3 ; x <= flash/3 ; x++)
			for(int y = -flash/3 ; y < flash/3 ; y++)
				matrix.blend(px+x, py+y, 16, dead ? 0xFF0000 : 0xff6080); // red if we're dead, a nice pink for success

/*
		// highlight the trail, starting at the current head
		// (at offset in the path)
		uint8_t pos = offset + size*size - (length - flash/4);
		pos %= size*size;
		uint8_t oxy = path[pos];
		matrix.blend(oxy / size, oxy % size, 200, palette[2]);
*/
		draw_path(matrix, dead ? 0xa08000 : 0x8000c0);
		flash--;
		delay(30);
	} else
	if (dead)
	{
		// fade everything
		for(int x = 0 ; x < size ; x++)
			for(int y = 0 ; y < size ; y++)
				matrix.blend(x, y, 1, 0);
		if (--dead == 0)
			begin();
	} else {
		draw_path(matrix, palette[0]);

		matrix.blend(px, py, 128, palette[1]);
		matrix.blend(fx, fy, 128, palette[2]);
	}
}
