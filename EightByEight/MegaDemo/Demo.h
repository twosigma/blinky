/** \file
 * Base class for the badge demos.
 */
#ifndef _Demo_h_
#define _Demo_h_

#include "RGBMatrix.h"

class Demo
{
public:
	Demo() {};
	~Demo() {};

	virtual void begin() = 0;

	// returns true if we should force a redraw
	virtual bool step(float ax, float ay, float az) = 0;

	// draw into the frame buffer provided
	virtual void draw(RGBMatrix &matrix) = 0;
};

#endif
