/** \file
 * Image object that draws pixels as you see them in the code
 */

#include "RGBMatrix.h"

#include <stdint.h>

class Image
{
public:
  // set the image data
  void setimage(uint32_t colors[], int pixels[LED_ROWS][LED_COLS]);
  // paint the current image
  void paint(RGBMatrix &matrix);
  // fade in from black to the current image
  void fadein(RGBMatrix &matrix);
  // fade out from the current image to black
  void fadeout(RGBMatrix &matrix);
  

  static const uint32_t BLACK = 0x000000;
  static const uint32_t WHITE = 0x666666;
  static const uint32_t RED = 0x660000;
  static const uint32_t GREEN = 0x006600;
  static const uint32_t BLUE = 0x000066;
  static const uint32_t TEEL = 0x008080;
  static const uint32_t PURPLE = 0x430c70;
  static const uint32_t YELLOW = 0x7d7f18;
  static const uint32_t ORANGE = 0x684213;
  static const uint32_t PINK = 0x7f1172;

  static const uint32_t LIGHTER = 0x444444;

private:
  uint32_t current_pixels[LED_ROWS][LED_COLS];
};
