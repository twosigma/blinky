/** \file
 * Sketch demo
 * Demonstrate how to use the Image object to create an image
 */

#include <Arduino.h>
#include "Sketch.h"
#include "Image.h"

Image board;

// CUSTOMIZE: Set the speed at which to cycle between the images
static int speed = 3;
// CUSTOMIZE: Set the colors and pixels here, in hex format (similar to coloring HTML.)
// or use preset colors defined in Image.
static uint32_t palette[] = {Image::BLACK, Image::WHITE,
  Image::RED, Image::ORANGE, Image::YELLOW, Image::GREEN, Image::BLUE, Image::PURPLE, Image::PINK};
// This demo will cycle through the set of drawings in this array
static const int IMAGES = 5;
static int pixels[IMAGES][LED_ROWS][LED_COLS] = 
{{
 {1,0,0,0,0,0,0,0},
 {0,1,2,3,0,0,0,0},
 {0,2,1,2,3,0,0,0},
 {0,3,2,1,2,3,0,0},
 {0,0,3,2,1,2,3,0},
 {0,0,0,3,2,1,2,0},
 {0,0,0,0,3,2,1,0},
 {0,0,0,0,0,0,0,1},
 },
 {
 {1,0,0,0,0,0,0,0},
 {0,1,2,3,4,0,0,0},
 {0,2,1,2,3,4,0,0},
 {0,3,2,1,2,3,4,0},
 {0,4,3,2,1,2,3,0},
 {0,0,4,3,2,1,2,0},
 {0,0,0,4,3,2,1,0},
 {0,0,0,0,0,0,0,1},
 },
 {
 {1,0,0,0,0,0,0,0},
 {0,1,2,3,4,5,0,0},
 {0,2,1,2,3,4,5,0},
 {0,3,2,1,2,3,4,0},
 {0,4,3,2,1,2,3,0},
 {0,5,4,3,2,1,2,0},
 {0,0,5,4,3,2,1,0},
 {0,0,0,0,0,0,0,1},
 },
 {
 {1,0,0,0,0,0,0,0},
 {0,1,2,3,4,5,6,0},
 {0,2,1,2,3,4,5,0},
 {0,3,2,1,2,3,4,0},
 {0,4,3,2,1,2,3,0},
 {0,5,4,3,2,1,2,0},
 {0,6,5,4,3,2,1,0},
 {0,0,0,0,0,0,0,1},
 },
 {
 {1,8,8,8,8,8,8,8},
 {7,1,2,3,4,5,6,8},
 {7,2,1,2,3,4,5,8},
 {7,3,2,1,2,3,4,8},
 {7,4,3,2,1,2,3,8},
 {7,5,4,3,2,1,2,8},
 {7,6,5,4,3,2,1,8},
 {7,7,7,7,7,7,7,1},
 }
 };

void Sketch::begin()
{
  image_index = 0;
}


bool Sketch::step(float nx, float ny, float nz)
{
  const uint32_t now = millis();
  if (now - last_time_change > (1000ul / speed)) {
    image_index +=1;
    if (image_index >= IMAGES) {
      image_index = 0;
    }
    board.setimage(palette, pixels[image_index]);
    last_time_change = now;
  }
  // never force redraw
  return false;
}

void Sketch::draw(RGBMatrix &matrix)
{
  board.paint(matrix);
  
}
