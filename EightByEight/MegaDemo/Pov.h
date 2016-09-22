/** \file POV easter egg
 *
 * Draws on the first 
 */

#include "Demo.h"

class Pov : public Demo
{
private:
	float a_smooth;
	float vel;
	float pos;
	float max_pos;

	int last_dir;
	uint32_t last_step_micros;

public:
	Pov();

	bool enabled;

	virtual void begin();

	virtual bool step(float ax, float ay, float az);

	virtual void draw(RGBMatrix &matrix);
};
