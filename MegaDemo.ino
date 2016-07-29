/** \file
 * Combine multiple demos for the Two Sigma / Blinklabs EightByEight badge.
 *
 * The button is used to cycle between the different demos.
 * 
 * The pixels one is not really a separate demo; we switch to it
 * automatically if it tells us that it has received a packet.
 * If we go more than ten seconds without a packet, the current demo
 * is restarted.
 */

#include "Badge.h"
#include "Life.h"
#include "Bubble.h"
#include "Pixels.h"
#include "Rain.h"
#include "Cycle.h"
#include "Pov.h"

Badge badge;

Pixels pixels;
Bubble bubble;
Life life;
Rain rain;
Cycle cycle;
Pov pov;

Demo * demos[] = {
  &cycle,
	&rain,
	&life,
	&bubble,
};

const unsigned num_demos = sizeof(demos) / sizeof(*demos);
static unsigned demo_num = 0;
static Demo * demo;

static char mac_buf[6*3+1];
static uint32_t last_draw_millis;
static uint32_t last_video_millis;
static bool draw_video;

void setup()
{
	badge.begin();
	badge.matrix.clear();
	badge.matrix.show();

	WiFi.persistent(false);

	// do not join any wifi networks if the button is held down
	// during startup
	if (!badge.button())
	{
#if 0
		WiFi.mode(WIFI_STA);
		WiFi.begin("twosigma-blinky", "blinkblinkblink");
#else
		WiFi.mode(WIFI_AP);
		WiFi.begin("mypocket", "BUBBLEmonkey");
		WiFi.config(IPAddress(192,168,1,4), IPAddress(0,0,0,0), IPAddress(255,255,255,0));
#endif
	}

	uint8_t mac[6];
	WiFi.macAddress(mac);
	sprintf(mac_buf, "%02x:%02x:%02x:%02x:%02x:%02x",
		mac[0],
		mac[1],
		mac[2],
		mac[3],
		mac[4],
		mac[5]
	);

	Serial.println(mac_buf);

	pixels.begin();
	pov.begin();


	// Initialize all of the demos and start at 0
	for(int i = 0 ; i < num_demos ; i++)
		demos[i]->begin();

	demo_num = 0;
	demo = demos[demo_num];
}


void loop()
{
	if (badge.poll())
	{
		// they have tapped fairly hard, should send this to demo
#if 0
		pov.enabled = true;
		badge.matrix.clear();
		badge.matrix.show();
#endif
	}

	if (pov.enabled)
	{
		if (badge.button())
		{
			pov.enabled = false;
			pov.begin();
			return;
		}

		// pov is the highest priority; we don't do anything
		// else until it disables itself
		pov.step(badge.ax, badge.ay, badge.az);
		pov.draw(badge.matrix);
		return;
	}

	if (badge.button_edge())
	{
		// should cycle to the next demo
		demo_num = (demo_num + 1) % num_demos;
		demo = demos[demo_num];
	}

	if (badge.button())
	{
		Serial.print(mac_buf); Serial.print(' ');
		Serial.println(WiFi.localIP());

		Serial.print(badge.nx); Serial.print(' ');
		Serial.print(badge.ny); Serial.print(' ');
		Serial.print(badge.nz); Serial.print(' ');
		Serial.println(badge.g);
	}
		

	const uint32_t now = millis();

	bool do_draw = demo->step(badge.ax, badge.ay, badge.az);

	if (pixels.step(0,0,0))
	{
		// we have received a video frame.  draw it, not
		// the demo that we're showing.
		do_draw = true;
		draw_video = true;
		last_video_millis = now;
	} else
	if (draw_video && now - last_video_millis > 10000ul)
	{
		// no video for ten seconds, go back to normal demo
		draw_video = false;
	}
	

	// only draw the LEDs at 30Hz
	if (!do_draw && now - last_draw_millis < 30)
		return;
	last_draw_millis = now;
	
	if (draw_video)
		pixels.draw(badge.matrix);
	else
		demo->draw(badge.matrix);

	badge.matrix.show();
}
