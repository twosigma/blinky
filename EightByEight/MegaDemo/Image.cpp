/** \file
 * Image object that draws pixels as you see them in the code
 */

#include <Arduino.h>
#include "Image.h"
#include <stdint.h>


// set the image data
void Image::setimage(uint32_t colors[], int pixels[LED_ROWS][LED_COLS]) {
  for (int x = 0 ; x < LED_ROWS ; x++)
    for (int y = 0 ; y < LED_COLS ; y++)
      current_pixels[x][y] = colors[pixels[x][y]];
}

// paint the current image
void Image::paint(RGBMatrix &matrix) {
  for (int x = 0 ; x < LED_ROWS ; x++)
    for (int y = 0 ; y < LED_COLS ; y++)
      matrix.set(x, y, current_pixels[x][y]);
}

// fade in from black to the current image
void Image::fadein(RGBMatrix &matrix) {
  for (int x = 0 ; x < LED_ROWS ; x++)
    for (int y = 0 ; y < LED_COLS ; y++)
      matrix.blend(x, y, 64, current_pixels[x][y]);
}

// fade out from the current image to black
void Image::fadeout(RGBMatrix &matrix) {
  for (int x = 0 ; x < LED_ROWS ; x++)
    for (int y = 0 ; y < LED_COLS ; y++)
      matrix.blend(x, y, 16, 0, 0, 0);
}

