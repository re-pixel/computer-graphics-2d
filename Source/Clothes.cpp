#include "../Header/Clothes.h"

#define _USE_MATH_DEFINES
#include <cmath>

const float GRAVITY = 4.0f;

const float PATH_START = -M_PI / 2;
const float PATH_TOP = -3 * M_PI / 2;

void placeInDrum(Clothing& c, int slot, float cx, float cy, float pathR) {
    c.inMachine = true;
    c.falling = false;
    c.fallSpeed = 0.0f;
    c.angle = PATH_START - slot * (M_PI / 4);
    c.x = cx + cos(c.angle) * pathR;
    c.y = cy + sin(c.angle) * pathR;
}

void updateClothing(Clothing& c, float omega, float dt, float cx, float cy, float pathR) {
    if (!c.falling) {
        c.angle -= omega * dt;
        if (c.angle <= PATH_TOP) {
            c.angle = PATH_TOP;
            c.falling = true;
            c.fallSpeed = 0.0f;
        }
        c.x = cx + cos(c.angle) * pathR;
        c.y = cy + sin(c.angle) * pathR;
    }
    else {
        c.fallSpeed += GRAVITY * dt;
        c.y -= c.fallSpeed * dt;
        c.x = cx;
        if (c.y <= cy - pathR) {
            c.y = cy - pathR;
            c.falling = false;
            c.angle = PATH_START;
        }
    }
}
