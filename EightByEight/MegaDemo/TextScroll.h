#include "Demo.h"

class TextScroll : public Demo
{
public:
	virtual void begin();
	virtual bool step(float ax, float ay, float az);

	virtual void draw(RGBMatrix &matrix);

private:
	void load_string(const char* string);
	void scroll_step();

	bool screen[LED_ROWS][LED_COLS];
	uint16_t next_row;
	uint32_t last_time_scroll;

	bool buffer[8*256][LED_ROWS];
	bool *buffer_end;
};
