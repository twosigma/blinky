#include "Demo.h"

class Bubble : public Demo
{
public:
	virtual void begin();
	virtual bool step(float ax, float ay, float az);

	virtual void draw(RGBMatrix &matrix);

private:
	float ax, ay, az;
};
