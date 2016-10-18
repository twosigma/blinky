/** \file
 * Sketch demo
 * Demonstrate how to use the Image object to create an image
 */
#include "Demo.h"

class Sketch : public Demo
{
public:
  virtual void begin();
  virtual bool step(float ax, float ay, float az);

  virtual void draw(RGBMatrix &matrix);

private:
  uint32_t last_time_change;
  int image_index;
};
