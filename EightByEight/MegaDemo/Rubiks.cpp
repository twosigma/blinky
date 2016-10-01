/** \file
 * Rubiks demo
 */

#include <Arduino.h>
#include "Rubiks.h"
#include <math.h>
#include <algorithm>

void Rubiks::begin()
{
    for (int f = 0; f < 6; f++) {
        for (int x = 0; x < 3; x++) {
            for (int y = 0; y < 3; y++) {
                faces[f][x][y] = f;
            }
        }
    }

    ax = 0.0;
    ay = 0.0;
    ay = 0.0;
    current_face = 0;
    last_tap_millis = millis();
    last_double_tap_millis = millis();
    z_offset = 0;
    corner_color = 0x000000;
    taps = 0;
}

bool Rubiks::step(float nx, float ny, float nz)
{
    // double tap detection
    z_samples[z_offset] = sqrt(nx*nx + ny*ny + nz*nz);
    uint32_t next_offset = (z_offset + 1) % RUBIKS_TAP_SAMPLES;
    double max_diff = 0.0;

    for (int i = 0; i < RUBIKS_TAP_SAMPLES; i++) {
        max_diff = std::max(max_diff, fabs(z_samples[i] - z_samples[z_offset]));
    }

    if (last_tap_millis < millis() - RUBIKS_TAP_WAIT) {
        taps = 0;
    }

    if (last_tap_millis < millis() - RUBIKS_TAP_LOCKOUT) {
        if (max_diff > RUBIKS_REQUIRED_DIFF && fabs(z_samples[next_offset] - z_samples[z_offset]) < RUBIKS_STATIONARY_DIFF) {
            last_tap_millis = millis();
            taps++;
        }
        if (taps == 2) {
            corner_color = 0xFF00FF;
            taps = 0;

            this->rotate(current_face); 
        } else if (taps == 1) {
            corner_color = 0x00FF00;
        } else {
            corner_color = 0x000000;
        }
    }
    z_offset = next_offset;

    // rotate the x y by 45 degrees
    const float rx = std::max(-10.0, std::min(10.0, nx * 0.851 - ny * 0.525));
    const float ry = std::max(-10.0, std::min(10.0, ny * 0.851 + nx * 0.525));
    const float rz = std::max(-10.0, std::min(10.0, 1.0*nz));

    const float weight = 0.5;
    ax = weight*ax + (1.0-weight)*rx;
    ay = weight*ay + (1.0-weight)*ry;
    az = weight*az + (1.0-weight)*rz;

    const float magnitude = sqrt(ax*ax + ay*ay + az*az);
    const float trigger_radians = 20 * 0.017;

    if (acos(ax / magnitude) < trigger_radians) {
        current_face = 0;
    } else if (acos(-ax / magnitude) < trigger_radians) {
        current_face = 2;
    } else if (acos(ay / magnitude) < trigger_radians) {
        current_face = 1;
    } else if (acos(-ay / magnitude) < trigger_radians) {
        current_face = 3;
    } else if (acos(az / magnitude) < trigger_radians) {
        current_face = 4;
    } else if (acos(-az / magnitude) < trigger_radians) {
        current_face = 5;
    }

    return false;
}

void Rubiks::rotate(uint8_t face)
{
    for (int j = 0; j < ((face == 4) ? 3 : 1); j++) { // face 4 is displayed mirrored, so spin 3x to go clockwise like others
        for (int i = 0; i < 3; i++) {
            uint8_t *face_src;
            uint8_t *face_dst;
            uint8_t tmp;
    
            face_dst = connected[face][i];
            face_src = connected[face][i+9];
            tmp = faces[face_dst[0]][face_dst[1]][face_dst[2]];
            faces[face_dst[0]][face_dst[1]][face_dst[2]] = faces[face_src[0]][face_src[1]][face_src[2]];
    
            face_dst = face_src;
            face_src = connected[face][i+6];
            faces[face_dst[0]][face_dst[1]][face_dst[2]] = faces[face_src[0]][face_src[1]][face_src[2]];
    
            face_dst = face_src;
            face_src = connected[face][i+3];
            faces[face_dst[0]][face_dst[1]][face_dst[2]] = faces[face_src[0]][face_src[1]][face_src[2]];
    
            face_dst = face_src;
            faces[face_dst[0]][face_dst[1]][face_dst[2]] = tmp;
        }
    
    
        uint8_t tmp = faces[face][0][0];
        faces[face][0][0] = faces[face][2][0];
        faces[face][2][0] = faces[face][2][2];
        faces[face][2][2] = faces[face][0][2];
        faces[face][0][2] = tmp;
    
        tmp = faces[face][0][1];
        faces[face][0][1] = faces[face][1][0];
        faces[face][1][0] = faces[face][2][1];
        faces[face][2][1] = faces[face][1][2];
        faces[face][1][2] = tmp;
    }
}


// White, Blue, Yellow, Green, Red, Orange
static uint32_t colors[6] = {0xFFFFFF, 0x0000FF, 0xFFB000, 0x00FF00, 0xFF0000, 0xFF5000}; 
void draw_help(RGBMatrix &matrix, uint8_t current, uint8_t color, uint8_t x, uint8_t y) 
{
    // Fix the rotation of side faces so that orange center is always up
    if (current == 3) {
        x = 4 - x;
        y = 4 - y;
    } else if (current == 0) {
        uint8_t t = x;
        x = y;
        y = 4 - t;
    } else if (current == 2) {
        uint8_t t = x;
        x = 4 - y;
        y = t;
    } else if (current == 4) {
        x = 4 - x;
    }
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            const int xf = i + 2*x;
            const int yf = j + 2*y;
            if (xf > 0 && xf <= 8 && yf > 0 && yf <= 8) {
                matrix.set(xf-1, yf-1, colors[color]);
            }
        }
    }
}

void Rubiks::draw(RGBMatrix &matrix)
{
    matrix.setBrightness(100);
    matrix.set(0,0,corner_color);
    matrix.set(0,7,corner_color);
    matrix.set(7,0,corner_color);
    matrix.set(7,7,corner_color);

    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            draw_help(matrix, current_face, faces[current_face][x][y], x+1, y+1);
        }
    }

    uint8_t *face;

    face = connected[current_face][0];
    draw_help(matrix, current_face, faces[face[0]][face[1]][face[2]], 0, 1);

    face = connected[current_face][1];
    draw_help(matrix, current_face, faces[face[0]][face[1]][face[2]], 0, 2);

    face = connected[current_face][2];
    draw_help(matrix, current_face, faces[face[0]][face[1]][face[2]], 0, 3);

    face = connected[current_face][3];
    draw_help(matrix, current_face, faces[face[0]][face[1]][face[2]], 1, 4);

    face = connected[current_face][4];
    draw_help(matrix, current_face, faces[face[0]][face[1]][face[2]], 2, 4);

    face = connected[current_face][5];
    draw_help(matrix, current_face, faces[face[0]][face[1]][face[2]], 3, 4);

    face = connected[current_face][6];
    draw_help(matrix, current_face, faces[face[0]][face[1]][face[2]], 4, 3);

    face = connected[current_face][7];
    draw_help(matrix, current_face, faces[face[0]][face[1]][face[2]], 4, 2);

    face = connected[current_face][8];
    draw_help(matrix, current_face, faces[face[0]][face[1]][face[2]], 4, 1);

    face = connected[current_face][9];
    draw_help(matrix, current_face, faces[face[0]][face[1]][face[2]], 3, 0);

    face = connected[current_face][10];
    draw_help(matrix, current_face, faces[face[0]][face[1]][face[2]], 2, 0);

    face = connected[current_face][11];
    draw_help(matrix, current_face, faces[face[0]][face[1]][face[2]], 1, 0);
}
