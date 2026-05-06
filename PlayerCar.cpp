#include "include/PlayerCar.h"

PlayerCar::PlayerCar(Texture &carTexture, float x, float y, float spd, float leftLimit, float rightLimit)
    : Car(carTexture, x, y, spd)
{
    leftBound = leftLimit;
    rightBound = rightLimit;
}

void PlayerCar::enforceBounds()
{
    Vector2f pos = getPosition();

    if (pos.x < leftBound)
    {
        setPosition(leftBound, pos.y);
    }

    if (pos.x > rightBound)
    {
        setPosition(rightBound, pos.y);
    }
}