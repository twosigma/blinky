/** \file
 * Color switching game for three or five players.
 *
 * (c) 2016 Trammell Hudson
 *
 *
One badge is the leader and creates a wifi network.

It starts drawing a circular pattern around its border to indicate
that it is leading.

The other badges join the network and send hellos to the leader.

The leader sends updates to each of the new clients to let them know
how many clients there are.  Everyone increases the number of clients
on their pattern and flashes in sync to indicate that they are in the
same game.

Once there are three or five players, the game begins.  The leader
assigns random colors to each client and sends a packet to tell them
what color they all have.

When a client (or the leader) senses a tap (G > 20), they notify the game
leader, which toggles the colors on either side and sends the updated
colors.

When all clients reach white, they celebrate with a fireworks display.


 */
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <stdint.h>

class ColorGame
{
public:
	ColorGame() { reset(); }

	void reset();
	void display();

private:
	boolean leader;
	boolean playing;
	boolean won;
	boolean lost;
	int num_players;

	// how many phases do the badges cycle through?
	static const int num_states = 7;
	static const uint32_t colors[num_states];

	// which slot and color am I right now?
	int my_id;
	int my_state;
	IPAddress leader_ip;
	int leader_port;

	static const int max_players = 5;

	struct player_t {
		int state;
		IPAddress ip;
		int port;
		uint32_t last_packet;
	};

	player_t players[max_players];

	void draw_game_screen();
	void draw_wait_screen();
	void draw_win_screen();
	void draw_loss_screen();
	void poll_network();
	void poll_accelerometer();

	static const int udp_port = 9999;
	WiFiUDP udp;
	uint32_t last_packet_millis; // rx
	uint32_t last_state_millis; // tx
	uint32_t last_tapped_millis;

	void send_state(boolean force = false);

	/* Network messages */
	enum {
		JOIN_MSG = 0xF00D0001,
		WAIT_MSG,
		STATE_MSG,
		PLAY_MSG,
	};

	// Sent by new followers when they join a leader
	struct join_msg
	{
		uint32_t magic;
	};

	// Sent at 1 Hz by the leader to notify the followers
	struct wait_msg
	{
		uint32_t magic;
		uint32_t num_players;
	};

	// Sent by the leader at at least 1 Hz
	// Special states:
	enum {
		STATE_LOST = 0xFE,
		STATE_WON = 0xFF,
	};
	struct state_msg
	{
		uint32_t magic;
		uint32_t state;
	};

	// Sent by the player at at least 1 Hz to stay in the game
	struct play_msg
	{
		uint32_t magic;
		uint32_t tapped; // non-zero if the player has hit the button
	};

	void handle_join(IPAddress ip, int port, const uint8_t * buf);
	void handle_play(IPAddress ip, int port, const uint8_t * buf);
	void handle_wait(IPAddress ip, int port, const uint8_t * buf);
	void handle_state(IPAddress ip, int port, const uint8_t * buf);
	void play(int id, int tapped);
	void check_for_win(void);
};

const uint32_t ColorGame::colors[] = {
	0xFFFFFF,
	0xFF0000,
	0x00FF00,
	0x0000FF,
	0xFFFF00,
	0xFF00FF,
	0x00FFFF,
};

void ColorGame::reset()
{
#if 0
	// do not write Wifi config to flash; this prevents
	// corrupting it when things go wrong.
	WiFi.persistent(false);
	//WiFi.mode(WIFI_STA);

	// hard code ssid and password for now;
	// this device will create the network and become the leader
	const char * ssid = "blinky-12345";
	WiFi.mode(WIFI_AP_STA);
	IPAddress ip(192,168,4,1);
	IPAddress gw(192,168,4,1);
	IPAddress subnet(255,255,255,0);
	WiFi.softAP(ssid);
	WiFi.config(ip, gw, subnet);
#endif

	udp.begin(udp_port);

	playing = false;
	leader = false;
	my_id = -1;
	my_state = 0;
	num_players = 1;
	won = false;
	lost = false;
}

void ColorGame::handle_join(IPAddress ip, int port, const uint8_t * buf)
{
	// if we are already playing a game, don't accept new players
	if (playing)
		return;

	// if someone sends us a join message, we become a leader
	if (!leader)
		leader = true;

	// if we have too many players, don't accept new ones
	if (num_players >= max_players)
		return;

	// insert the player into the player map
	player_t * const player = &players[num_players++];
	player->ip = ip;
	player->port = port;
	player->state = random(num_states);
	player->last_packet = millis();
}


void ColorGame::handle_wait(IPAddress ip, int port, const uint8_t * buf)
{
	// leader should never receive this message
	if (leader)
		return;

	const wait_msg * const msg = (const wait_msg*) buf;

	num_players = msg->num_players;
	leader_ip = ip;
	leader_port = port;
}


void ColorGame::handle_state(IPAddress ip, int port, const uint8_t * buf)
{
	const state_msg * msg = (const state_msg*) buf;
	leader_ip = ip;
	leader_port = port;

	if (msg->state == STATE_WON)
	{
		won = true;
		return;
	}
	if (msg->state == STATE_LOST)
	{
		lost = true;
		return;
	}

	if (msg->state >= num_states)
	{
		// wtf?
		return;
	}

	// when we receive a state message we are finally playing
	my_state = msg->state;
	playing = true;
}


void ColorGame::check_for_win(void)
{
	for(int i = 0 ; i < num_players; i++)
	{
		if (players[i].state != 0)
			return;
	}

	for(int i = 0 ; i < num_players ; i++)
		players[i].state = STATE_WON;
}


// update the state for the players on either side when tapped
void ColorGame::play(int id, int tapped)
{
	// track how long since we've seen the player
	player_t *const player = &players[id];
	player->last_packet = millis();

	// if we aren't playing yet, ignore this message
	if (!tapped || !playing)
		return;

	int id1 = (id+1) % num_players;
	int id2 = (id-1 + num_players) % num_players;
	player_t * const p1 = &players[id1];
	player_t * const p2 = &players[id2];
	p1->state = (p1->state + 1) % num_states;
	p2->state = (p2->state + 1) % num_states;

	check_for_win();

	// send the new states to everyone.
	send_state(true);
}


void ColorGame::handle_play(IPAddress ip, int port, const uint8_t * buf)
{
	// player should never receive this message
	if (!leader)
		return;

	const play_msg * msg = (const play_msg *) buf;

	// find the player in the map
	for(int i = 0 ; i < num_players ; i++)
	{
		player_t * const player = &players[i];
		if (player->ip != ip || player->port != port)
			continue;

		play(i, msg->tapped);
		return;
	}

	// player not found.  wtf?
}


void ColorGame::poll_network()
{
	int len = udp.parsePacket();
	if (len == 0)
		return;

	IPAddress ip = udp.remoteIP();
	int port = udp.remotePort();
	uint8_t buf[128];
	udp.read(buf, sizeof(buf));
	last_packet_millis = millis();

	const uint32_t magic = *(uint32_t*) buf;
	switch(magic)
	{
	case JOIN_MSG: handle_join(ip, port, buf); break;
	case WAIT_MSG: handle_wait(ip, port, buf); break;
	case STATE_MSG: handle_state(ip, port, buf); break;
	case PLAY_MSG: handle_play(ip, port, buf); break;
	default:
		// wtf?
		break;
	}
}


void ColorGame::poll_accelerometer()
{
	float nx, ny, nz;
	mma8653.getXYZ(nx, ny, nz);
	int ix = nx;
	int iy = ny;
	int iz = nz;
	int total_g = ix*ix + iy*iy + iz*iz;
	uint32_t now = millis();

#if 0
	Serial.print(ix); Serial.print(' ');
	Serial.print(iy); Serial.print(' ');
	Serial.print(iz); Serial.print(' ');
	Serial.println(total_g);
#endif

	// if we're tapped, fake the last packet millis
	boolean tapped = total_g > 400;
	//if (!playing && tapped)
	if (tapped)
		last_packet_millis = now;

	// send a message as soon as we're tapped or
	// at least 1 Hz
	if (!tapped && now - last_tapped_millis < 1000)
		return;
	last_tapped_millis = now;
	
	if (!leader)
	{
		// send the message to the leader
		static play_msg msg;
		msg.magic = PLAY_MSG;
		msg.tapped = tapped;
		udp.beginPacket(leader_ip, leader_port);
		udp.write((const uint8_t*) &msg, sizeof(msg));
		udp.endPacket();
		return;
	}

	// if I'm the leader and we haven't started the game yet,
	// go ahead and start it.
	if (!playing && tapped)
		playing = true;

	play(0, tapped);
}


/* Draw one pixel on the border; there are 28 border pixels, not 32 */
static void
draw_border_pixel(int p, uint32_t rgb)
{
	p = p % 28;

	int row = 0;
	int col = 0;
	if (p < 7)
	{
		row = 0;
		col = p;
	} else
	if (p < 14)
	{
		row = p - 7;
		col = 7;
	} else
	if (p < 21)
	{
		row = 7;
		col = 21 - p;
	} else
	if (p < 28)
	{
		row = 28 - p;
		col = 0;
	}

	matrix.blend(row, col, 200, rgb);
}

void ColorGame::draw_wait_screen()
{
	// the game hasn't started yet, so we are waiting
	// for a message from the leader to start.  until then
	// we display our current id and cycle the colors
	const uint32_t now = millis();

	int age = now - last_packet_millis;
	int bright = age > 1000 ? 0 : (1000 - age) / 4;

	for(int i = 1 ; i < LED_COLS-1 ; i++)
		for(int j = 1 ; j < LED_ROWS-1 ; j++)
			matrix.set(i, j, 0, 0, bright);

	// fade out the border
	for(int i = 0 ; i < 8 ; i++)
	{
		matrix.blend(i, 0, 4, 0, 0, 0);
		matrix.blend(0, i, 4, 0, 0, 0);
		matrix.blend(i, 7, 4, 0, 0, 0);
		matrix.blend(7, i, 4, 0, 0, 0);
	}

	// draw a tracer around the boarder based on the number of players
	for(int i = 0 ; i < num_players ; i++)
		draw_border_pixel(my_state + i * 28 / num_players, colors[i]);

	static uint32_t last_state;
	if (now - last_state > 50)
	{
		my_state = (my_state + 1) % 28;
		last_state = now;
	}
	

}


void ColorGame::draw_game_screen()
{
	int age = millis() - last_packet_millis;
	int bright = age > 5000 ? 0 : (5000 - age) / 20;

	uint32_t color = colors[my_state];
	int r = (color >> 16) & 0xFF;
	int g = (color >>  8) & 0xFF;
	int b = (color >>  0) & 0xFF;

	r = (r * bright) / 256;
	g = (g * bright) / 256;
	b = (b * bright) / 256;

	for(int i = 0 ; i < LED_COLS ; i++)
		for(int j = 0 ; j < LED_ROWS ; j++)
			matrix.set(i, j, r, g, b);
}


void ColorGame::send_state(boolean force)
{
	uint32_t now = millis();
	if (!force && now - last_state_millis < 1000)
		return;
	last_state_millis = now;

	if (playing)
	{
		// send each player their current state
		state_msg msg;
		msg.magic = STATE_MSG;
		msg.state = players[0].state;

		// fake the message to ourselves
		handle_state(IPAddress(0,0,0,0), 0, (const uint8_t*) &msg);

		for(int i = 1 ; i < num_players ; i++)
		{
			const player_t * const player = &players[i];
			msg.state = player->state;
			udp.beginPacket(player->ip, player->port);
			udp.write((const uint8_t*) &msg, sizeof(msg));
			udp.endPacket();
		}
	} else {
		// still waiting for players
		wait_msg msg;
		msg.magic = WAIT_MSG;
		msg.num_players = num_players;

		for(int i = 1 ; i < num_players ; i++)
		{
			const player_t * const player = &players[i];
			udp.beginPacket(player->ip, player->port);
			udp.write((const uint8_t*) &msg, sizeof(msg));
			udp.endPacket();
		}
	}
}


void ColorGame::draw_win_screen()
{
	// do stuff, then reset the game
	reset();
}


void ColorGame::draw_loss_screen()
{
	// eh
	reset();
}


void ColorGame::display()
{
	poll_network();
	poll_accelerometer();

	if (leader)
		send_state();

	if (won)
		draw_win_screen();
	else
	if (lost)
		draw_loss_screen();
	else
	if (playing)
		draw_game_screen();
	else
		draw_wait_screen();
}
