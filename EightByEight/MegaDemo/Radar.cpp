/** \file
 * Radar demo
 * 
 * A little app that mimics a radar screen, with a white dot that lights up when it is "detected" by the radar.
 * The dot will move around as you tilt the blinky square. It moves in the direction that the board is tilted.
 */

#include <Arduino.h>
#include "Radar.h"
#include <math.h>

#define TEAL 0x008080
#define ORANGE 0xa03f00
#define RED 0xfc0000
#define RED_FADE 0x4f0b0b
#define WHITE 0xffffff

static const int positions[20][4][2] = {
  { {0,0}, {1,1}, {2,2}, {3,3} },
  { {0,1}, {1,2}, {-1,-1}, {3,3} },
  { {0,3}, {1,3}, {2,3}, {3,3} },
  { {0,4}, {1,4}, {2,4}, {3,4} },
  { {0,6}, {1,5}, {-1,-1}, {3,4} },
  { {0,7}, {1,6}, {2,5}, {3,4} },
  { {1,7}, {2,6}, {-1,-1}, {3,4} },
  { {3,4}, {3,5}, {3,6}, {3,7} },
  { {4,4}, {4,5}, {4,6}, {4,7} },
  { {6,7}, {5,6}, {-1,-1}, {4,4} },
  { {4,4}, {5,6}, {6,6}, {7,7} }, 
  { {7,6}, {6,5}, {-1,-1}, {4,4}},
  { {4,4}, {5,4}, {6,4}, {7,4} },
  { {4,3}, {5,3}, {6,3}, {7,3} },
  { {4,3}, {6,2}, {-1,-1}, {7,1} },
  { {4,3}, {5,2}, {6,1}, {7,0} },
  { {4,3}, {5,1}, {-1,-1}, {6,0} },
  { {4,3}, {4,2}, {4,1}, {4,0} },
  { {3,3}, {3,2}, {3,1}, {3,0} },
  { {3,3}, {2,1}, {-1,-1}, {1,0} }
};

void Radar::begin()
{
  stp=1;
  stp_fade=1;
  stp_fade2=0;
  ax=0;
  ay=0;
  az=0;

  hitx=0;
  hity=0;
  
  for(int dot=0; dot<4; dot++){
    ray[dot][0] = positions[stp][dot][0];
    ray[dot][1] = positions[stp][dot][1];
  }
  last_move_time = millis();
  stp++;
}

bool Radar::step(float nx, float ny, float nz)
{
  if(millis()-last_move_time > 150){
    if(stp==20)
      stp=0;
    if(stp_fade==20)
      stp_fade=0;
    if(stp_fade2==20)
      stp_fade2=0;
   
    for(int dot=0; dot<4; dot++){
      ray[dot][0] = positions[stp][dot][0];
      ray[dot][1] = positions[stp][dot][1];
      ray_fade[dot][0] = positions[stp_fade][dot][0];
      ray_fade[dot][1] = positions[stp_fade][dot][1];
      ray_fade2[dot][0] = positions[stp_fade2][dot][0];
      ray_fade2[dot][1] = positions[stp_fade2][dot][1];
    }

  stp++;
  stp_fade++;
  stp_fade2++;
  last_move_time=millis();

    // smooth the values
  ax = ((ax * 0.40) + (nx * 0.60));
  ay = ((ay * 0.40) + (ny * 0.60));
  az = ((az * 0.40) + (nz * 0.60));

  // move the "hit" around according to how the board is tilted
  if( (ax>3) & (ay<3) & (ay>-3) ) { // if we are only tilted on x axis
    hitx = hitx - 1;
    hity = hity -1;
  }else if( (ax<-3) & (ay<3) & (ay>-3) ){
    hitx = hitx + 1;
    hity = hity + 1;
  }else if( (ay>3) & (ax<3) & (ax>-3) ){ // if we are only titled on y-axis
    hitx = hitx + 1;
    hity = hity - 1;
  }else if( (ay<-3) & (ax<3) & (ax>-3) ){
    hitx = hitx - 1;
    hity = hity + 1;
  }else if( (ax>3) & (ay>3) ){ // diagnol tilt, on a plane parallel to x-axis
    hity = hity - 1;
  }else if( (ax<-3) & (ay<-3) ){
    hity = hity + 1;
  }else if( (ax>3) & (ay<-3) ){ // diagnol tilt, on a plane parallel to y-axis
    hitx = hitx - 1;
  }else if( (ax<-3) & (ay>3) ){
    hitx = hitx + 1;
  }

  // make sure hitx and hity stay in the screen
  if(hitx<0){
    hitx=0;
  }
  else if(hitx>7){
    hitx=7;
  }
  if(hity<0){
    hity=0;
  }else if(hity>7){
    hity=7;
  }
  
  Serial.print("\nhitx: ");
  Serial.print(hitx);
  Serial.print("\nhity: ");
  Serial.print(hity);
  }

  return false;
}


void Radar::draw(RGBMatrix &matrix)
{
  // decay everything to black
  for (int x = 0 ; x < LED_ROWS ; x++)
    for (int y = 0 ; y < LED_COLS ; y++)
      matrix.blend(x, y, 255, 0, 0, 0);

  for(int i = 0 ; i < 4 ; i++){
    if( (ray[i][0]>=0) & (ray[i][1]>=0) ){ 
      matrix.blend(ray_fade2[i][0], ray_fade2[i][1], 64, RED_FADE);
      matrix.blend(ray_fade[i][0], ray_fade[i][1], 64, RED_FADE);
      matrix.blend(ray[i][0], ray[i][1], 64, RED);

      // highlight the hit
      if( (ray[i][0]==hitx) & (ray[i][1]==hity))
        matrix.blend(ray[i][0], ray[i][1], 255, 250, 200, 200);

    }
  }
  // for debugging (always shows 'hit'): matrix.blend(hitx, hity, 255, 250, 200, 200);
}
