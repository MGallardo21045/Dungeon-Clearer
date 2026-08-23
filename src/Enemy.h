#pragma once

#include <SDL3/SDL.h>

class Enemy
{
public:
    Enemy(float startX, float startY);

    void render(SDL_Renderer* renderer) const;

    SDL_FRect getBounds() const;

    void setHit(bool value);

    void takeDamage(int amount);
    
    bool isAlive() const;

private:
    float x;
    float y;
    float width;
    float height;

    bool wasHit;

    int health;
};