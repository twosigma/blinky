#include <Arduino.h>
#include "Badge.h"

const int i2c_scl = 14;
const int i2c_sda = 12;
const int BUTTON = 4;

void
Badge::begin()
{
#ifndef CONFIG_FEATHER
	// USB communication to PC via ARM coprocessor
	Serial.begin ( 460800 );
#else
	// USB communicatoin to PC via the adapter chip
	Serial.begin(115200);
#endif

	// setup the RGB pixel matrix
	matrix.begin();

	// Accelerometer setup
	Wire.begin(i2c_sda, i2c_scl);
	accel.setup();

	// Button
	pinMode(BUTTON, INPUT_PULLUP);
	last_button = 0;
}


boolean
Badge::button()
{
	return !digitalRead(BUTTON);
}


boolean
Badge::button_edge()
{
	int button = this->button();
	if (button == last_button)
		return 0;

	last_button = button;
	last_button_millis = millis();
	return button;
}


boolean
Badge::poll()
{
	// read the accelerometer, track the current forces
	accel.getXYZ(nx, ny, nz);
	ax = (ax * 7 + nx) / 8;
	ay = (ay * 7 + ny) / 8;
	az = (az * 7 + nz) / 8;

	g = sqrt(ax*ax + ay*ay + az*az);
	return g > 20;
}
