#include "Demo.h"

#define RUBIKS_TAP_LOCKOUT (50)
#define RUBIKS_TAP_WAIT (400)
#define RUBIKS_TAP_SAMPLES (10)
#define RUBIKS_STATIONARY_DIFF (0.3)
#define RUBIKS_REQUIRED_DIFF (0.8)

class Rubiks : public Demo
{
    public:
        virtual void begin();
        virtual bool step(float ax, float ay, float az);

        virtual void draw(RGBMatrix &matrix);

    private:
        uint8_t faces[6][3][3];
        float ax, ay, az;
        uint8_t current_face;
        uint8_t connected[6][12][3] = { // Connected faces starting from above the top left block of this face and going clockwise
            {{3,2,0}, {3,2,1}, {3,2,2}, {5,0,2}, {5,0,1}, {5,0,0}, {1,0,2}, {1,0,1}, {1,0,0}, {4,0,2}, {4,0,1}, {4,0,0}},
            {{0,2,0}, {0,2,1}, {0,2,2}, {5,0,0}, {5,1,0}, {5,2,0}, {2,0,2}, {2,0,1}, {2,0,0}, {4,2,2}, {4,1,2}, {4,0,2}},
            {{1,2,0}, {1,2,1}, {1,2,2}, {5,2,0}, {5,2,1}, {5,2,2}, {3,0,2}, {3,0,1}, {3,0,0}, {4,2,0}, {4,2,1}, {4,2,2}},
            {{2,2,0}, {2,2,1}, {2,2,2}, {5,2,2}, {5,1,2}, {5,0,2}, {0,0,2}, {0,0,1}, {0,0,0}, {4,2,0}, {4,1,0}, {4,0,0}},
            {{0,0,0}, {0,1,0}, {0,2,0}, {1,0,0}, {1,1,0}, {1,2,0}, {2,0,0}, {2,1,0}, {2,2,0}, {3,0,0}, {3,1,0}, {3,2,0}},
            {{0,2,2}, {0,1,2}, {0,0,2}, {3,2,2}, {3,1,2}, {3,0,2}, {2,2,2}, {2,1,2}, {2,0,2}, {1,2,2}, {1,1,2}, {1,0,2}}
        };
        uint32_t last_tap_millis;
        uint32_t last_double_tap_millis;
        float z_samples[RUBIKS_TAP_SAMPLES];
        uint32_t z_offset;
        uint32_t taps;
        uint32_t corner_color;
        void rotate(uint8_t face);
};

/*
 *                            0,0,0   0,0,1   0,0,2
 *                            0,1,0   0,1,1   0,1,2
 *                            0,2,0   0,2,1   0,2,2
 *           
 * 4,0,0   4,0,1   4,0,2      1,0,0   1,0,1   1,0,2      5,0,0   5,0,1   5,0,2
 * 4,1,0   4,1,1   4,1,2      1,1,0   1,1,1   1,1,2      5,1,0   5,1,1   5,1,2
 * 4,2,0   4,2,1   4,2,2      1,2,0   1,2,1   1,2,2      5,2,0   5,2,1   5,2,2
 *
 *                            2,0,0   2,0,1   2,0,2
 *                            2,1,0   2,1,1   2,1,2
 *                            2,2,0   2,2,1   2,2,2
 *
 *                            3,0,0   3,0,1   3,0,2
 *                            3,1,0   3,1,1   3,1,2
 *                            3,2,0   3,2,1   3,2,2
 *
 */
