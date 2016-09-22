/** \file
 * UDP Pixel data receiver.
 */

#include "Demo.h"
#include <WiFiUdp.h>

class Pixels : public Demo
{
public:
	Pixels() {}
	~Pixels() {}

	virtual void begin();

	virtual bool step(float ax, float ay, float az);
	virtual void draw(RGBMatrix &matrix);

	static const uint32_t PIXEL_MAGIC = 0x54534c50;
	static const int UDP_PORT = 5453;

	struct pixel_packet_t
	{
		uint32_t magic;
		uint32_t blend_time; // ms;
		uint8_t pixels[3 * LED_ROWS * LED_COLS];
	};

private:
	WiFiUDP udp;
	long rx_millis;

	pixel_packet_t pixels;
};
