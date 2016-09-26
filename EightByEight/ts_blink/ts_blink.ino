#include <FastLED.h>

// How many leds in your strip?
#define NUM_LEDS 12

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 5

// Minimum and maximum brightness for the HSV values that will be displayed on the LED
int MIN_V = 40;
int MAX_V = 128;
// Delay is milliseconds between changes in brightness
int DELAY = 10;

// Set up all the colors
CHSV ts_blue(125, 227, MIN_V);
CHSV ts_orange(28, 198, MIN_V);
CHSV colors[2] = { ts_blue, ts_orange };
CHSV color;

// Define the array of leds
CRGB color_rgb;
CRGB leds[NUM_LEDS];

// Rate of brightness change
uint8_t fade_delta = 1;
// Direction of brightness change
bool increasing = true;
// Used to track which color is currently selected in the colors array
int color_num = 0;
// Set this to true to change colors on next loop
bool change_color = false;

void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  // Set the start color
  color = ts_blue;
}

void loop() {

  // Begin increasing brightness and switch color every other loop
  if (color.val < MIN_V) {
    increasing = true;
    if (change_color) {
      change_color = false;
      color_num = (color_num + 1) % 2;
      color = colors[color_num];
    }
  }

  // Begin decreasing brightness and indicate color should change on next run
  if (color.val > MAX_V) {
    increasing = false;
    change_color = true;
  }

  if (increasing) {
    color.val += fade_delta;
  }
  else {
    color.val -= fade_delta;
  }

  // Convert to the more balanced 'rainbow' color scale which is better at
  // preserving yellow and the Two Sigma orange
  hsv2rgb_rainbow(color, color_rgb);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color_rgb;
  }
  FastLED.show();
  delay(DELAY);
}
