#pragma once

#include <SDL3/SDL.h>

class Projectile {
public:
    Projectile(float startX, float startY, float velocityX, float velocityY);

    void update(float deltaTime);
    void render(SDL_Renderer* renderer);
    SDL_FRect getBounds() const;
    bool isOffScreen() const;

private:
    float x;
    float y;

    float velocityX;
    float velocityY;
    
    float width = 10.0f;
    float height = 10.0f;
};