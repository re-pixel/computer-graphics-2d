#pragma once

struct Clothing {
    unsigned texture;
    float startX, startY;
    float x, y;
    bool isRed;
    bool inMachine;
    bool falling;
    float angle;
    float fallSpeed;
};

void placeInDrum(Clothing& c, int slot, float cx, float cy, float pathR);

void updateClothing(Clothing& c, float omega, float dt, float cx, float cy, float pathR);
