#pragma once

#include "Demo.h"

class RandomAndPleasing : public Demo
{
private:
	long last_update;
	int led_rate_ms;
	int p;
	bool led[LED_ROWS][LED_COLS];

public:
	RandomAndPleasing();

	virtual void begin();

	virtual bool step(float ax, float ay, float az);

	virtual void draw(RGBMatrix &matrix);
};
