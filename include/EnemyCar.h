#pragma once
#include "Car.h"
class EnemyCar : public Car{
    private:
    float screenHeight;
    public:
    EnemyCar();
    EnemyCar(Texture &carTexture, float x, float y, float spd, float screenH);
    void enforceBounds(float leftBound, float rightBound);
    void update(float dt);        // move downward
    bool isOffScreen();
};