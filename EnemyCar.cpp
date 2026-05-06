#include "include/EnemyCar.h"

EnemyCar::EnemyCar()
    : Car()
{
    screenHeight = 0;
}

EnemyCar::EnemyCar(Texture &carTexture, float x, float y, float spd,float screenH)
    : Car(carTexture, x, y, spd)
{
    screenHeight = screenH;
}

void EnemyCar::update(float dt)
{
    // Move downward
    Car::update(0, 1, dt);
}

bool EnemyCar::isOffScreen()
{
    Vector2f pos = getPosition();

    if (pos.y > screenHeight)
        return true;

    return false;
}
void EnemyCar::enforceBounds(float leftBound, float rightBound)
{
    Vector2f pos = getPosition();
    float halfWidth = getBounds().width / 2;

    if (pos.x - halfWidth < leftBound)
    {
        setPosition(leftBound + halfWidth, pos.y);
    }

    if (pos.x + halfWidth > rightBound)
    {
        setPosition(rightBound - halfWidth, pos.y);
    }
}