#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Car{
    private:
        Sprite carSprite;
        float speed;
    public:
        Car();
        Car(Texture &carTexture, float x, float y, float spd);

        Sprite& getSprite();
        
        //for positions 
        void setPosition(float x, float y);
        Vector2f getPosition();
        FloatRect getBounds();
        
        float getSpeed();
        void setSpeed(float spd);

        //for updating positions 
        void update(float x, float y, float dt);
};