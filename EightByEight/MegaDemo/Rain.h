/** \file
 * Rain drops (or Matrix effect)
 */

#include "Demo.h"

class Rain : public Demo
{
private:
	static const int num_drops = 32;
	float drops[num_drops][2];
	uint32_t colors[num_drops];
	void new_drop(int i, float angle);

public:
	Rain();

	virtual void begin();

	virtual bool step(float ax, float ay, float az);

	virtual void draw(RGBMatrix &matrix);
};
