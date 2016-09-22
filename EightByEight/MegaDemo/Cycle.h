/** \file
 * Cycle an LED around the border.
 */

#include "Demo.h"

class Cycle : public Demo
{
private:
	static const int num_drops = 1;
	float pos[num_drops];
	float speeds[num_drops];
	uint32_t colors[num_drops];
	uint32_t last_flash_millis;

	float cylon;

public:
	Cycle();

	virtual void begin();

	virtual bool step(float ax, float ay, float az);

	virtual void draw(RGBMatrix &matrix);
};
