#include "include/Car.h"
#include<iostream>
Car::Car(){
    speed = 0;
}
Car::Car(Texture &carTexture, float x, float y, float spd)
{
    carSprite.setTexture(carTexture);
    carSprite.setPosition(x, y);
    speed = spd;
}

Sprite &Car::getSprite()
{
    return carSprite;
}

void Car::setPosition(float x, float y)
{
    carSprite.setPosition(x, y);
}

Vector2f Car::getPosition()
{
    return carSprite.getPosition();
}

FloatRect Car::getBounds()
{
    return carSprite.getGlobalBounds();
}

float Car::getSpeed()
{
    return speed;
}

void Car::setSpeed(float spd)
{
    speed = spd;
}

void Car::update(float directionX, float directionY, float dt)
{
    Vector2f pos = carSprite.getPosition();

    pos.x += directionX * speed * dt;
    pos.y += directionY * speed * dt;

    carSprite.setPosition(pos);
}