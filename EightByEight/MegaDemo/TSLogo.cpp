/** \file
 * TSLogo dem
 */

#include <Arduino.h>
#include "TSLogo.h"
#include "Image.h"

Image logo;

// CUSTOMIZE: Set the speed at which to cycle between the images
static int speed = 1;
// CUSTOMIZE: Set the colors and pixels here, in hex format (similar to coloring HTML.)
// or use preset colors defined in Image.
static const uint32_t TSteal = 0x008070; // Teal Color 0x008080

static uint32_t palette[] = {Image::BLACK, Image::WHITE,
  Image::RED, Image::ORANGE, Image::YELLOW, Image::GREEN, Image::BLUE, Image::PURPLE, Image::PINK,
  0x008070};


// This demo will cycle through the set of drawings in this array
static const int IMAGES = 2;
static int pixels[IMAGES][LED_ROWS][LED_COLS] = 
{{
 {0,0,0,0,0,0,9,0},
 {0,0,0,0,0,9,0,0},
 {0,0,0,0,9,0,9,0},
 {0,9,9,0,9,0,9,0},
 {9,0,9,0,0,9,0,0},
 {9,0,9,0,0,0,0,0},
 {0,0,9,0,9,0,0,0},
 {0,0,9,9,0,0,0,0},
 },
 {
 {0,0,0,0,0,0,3,0},
 {0,0,0,0,0,3,0,0},
 {0,0,0,0,3,0,3,0},
 {0,3,3,0,3,0,3,0},
 {3,0,3,0,0,3,0,0},
 {3,0,3,0,0,0,0,0},
 {0,0,3,0,3,0,0,0},
 {0,0,3,3,0,0,0,0},
 },
 };

void TSLogo::begin()
{
  image_index = 0;
}


bool TSLogo::step(float nx, float ny, float nz)
{
  const uint32_t now = millis();
  if (now - last_time_change > (1000ul / speed)) {
    image_index +=1;
    if (image_index >= IMAGES) {
      image_index = 0;
    }
    logo.setimage(palette, pixels[image_index]);
    last_time_change = now;
  }
  // never force redraw
  return false;
}

void TSLogo::draw(RGBMatrix &matrix)
{
  logo.paint(matrix);
}
