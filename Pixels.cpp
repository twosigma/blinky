#include <Arduino.h>
#include "Pixels.h"

void Pixels::begin()
{
	udp.begin(UDP_PORT);
}


bool Pixels::step(float ax, float ay, float az)
{
	int len = udp.parsePacket();
	if (len == 0)
		return false;

	udp.read((uint8_t*) &pixels, sizeof(pixels));

	Serial.print("RX: ");
	Serial.println(len);

	if (len != sizeof(pixels))
		return false;
	if (pixels.magic != PIXEL_MAGIC)
		return false;
	
	// we have a packet! start the drawing
	rx_millis = millis();

	if (pixels.blend_time == 0)
		pixels.blend_time = 1;
	return true;
}

void Pixels::draw(RGBMatrix &matrix)
{
	// interpolate from our last packet to the new packet
	// over the time frame in the packet
	long now = millis();
	long rx_delta = now - rx_millis;
	Serial.println(rx_delta);
	int blend = 0;
	if (rx_delta > pixels.blend_time)
	{
		blend = 255;
	} else
	if (rx_delta < 0)
	{
		blend = 0;
	} else {
		blend = (255 * rx_delta) / pixels.blend_time;
	}

	unsigned offset = 0;
	for(int row = 0 ; row < LED_ROWS ; row++)
	{
		for(int col = 0 ; col < LED_COLS ; col++, offset++)
		{
			int r = pixels.pixels[3*offset + 0];
			int g = pixels.pixels[3*offset + 1];
			int b = pixels.pixels[3*offset + 2];
			matrix.blend(row, col, blend, r, g, b);
		}
	}
}
