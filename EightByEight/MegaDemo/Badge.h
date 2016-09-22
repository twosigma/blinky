/** \file
 * Constants for the Blinky Badge.
 */
#ifndef _badge_h_
#define _badge_h_

// if we are testing with feather hardware
// programing is different
//#define CONFIG_FEATHER


#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <stdint.h>
#include <Wire.h>
#include "RGBMatrix.h"
#include "mma8653.h"


class Badge
{
public:
	MMA8653 accel;
	RGBMatrix matrix;

	void begin();

	// sample the accelerometers, returns true if G > 20
	boolean poll();

	// returns true on the rising edge of the button
	boolean button_edge();

	// returns true if the button is held down
	boolean button();

	// accelerometer measurements and smooth values
	float nx, ny, nz;
	float ax, ay, az, g;

private:
	boolean last_button;
	uint32_t last_button_millis;
};

#endif
