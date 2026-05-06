#pragma once
#include "Car.h"

class PlayerCar : public Car
{
private:
    float leftBound;
    float rightBound;

public:
    PlayerCar(Texture &carTexture, float x, float y, float spd,float leftLimit, float rightLimit);
    void enforceBounds();
};