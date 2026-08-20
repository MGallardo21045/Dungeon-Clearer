#pragma once

#include <SDL3/SDL.h>

class Player
{
public:
    Player();

    void update(float deltaTime);
    void render(SDL_Renderer* renderer);

private:
    float x;
    float y;
    float speed;
};