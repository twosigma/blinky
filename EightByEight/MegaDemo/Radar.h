#include "Demo.h"

class Radar : public Demo
{
public:
  virtual void begin();
  virtual bool step(float ax, float ay, float az);

  virtual void draw(RGBMatrix &matrix);

private:
  float ax, ay, az;
  int ray[4][2];
  int ray_fade[4][2];
  int ray_fade2[4][2];
  int stp;
  int stp_fade;
  int stp_fade2;
  uint32_t last_move_time;
  int hitx;
  int hity;
};
