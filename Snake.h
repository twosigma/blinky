/** \file
 * Snake game
 */

#include "Demo.h"

class Snake : public Demo
{
private:
	static const int size = 8;
	uint8_t age[size][size];
	uint8_t length;
	int8_t vx, vy;
	int8_t px, py;
	int8_t fx, fy;

	unsigned long last_draw;
	int8_t flash;
	uint8_t dead;

	void new_food();
	bool out_of_bounds(int x, int y);
	bool space_free(int x, int y);
	bool is_food(int x, int y);

public:
	Snake();

	virtual void begin();

	virtual bool step(float ax, float ay, float az);

	virtual void draw(RGBMatrix &matrix);
};
