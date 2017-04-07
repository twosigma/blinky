/** \file
 * TSLogo
 */
#include "Demo.h"

class TSLogo : public Demo
{
public:
  virtual void begin();
  virtual bool step(float ax, float ay, float az);

  virtual void draw(RGBMatrix &matrix);

private:
  uint32_t last_time_change;
  int image_index;
};
