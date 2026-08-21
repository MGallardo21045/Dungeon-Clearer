#pragma once

#include <SDL3/SDL.h>

enum class Direction
{
    Up,
    Down,
    Left,
    Right
};

class Player
{
public:
    Player();

    void update(float deltaTime);
    void render(SDL_Renderer* renderer);
    void renderPunch(SDL_Renderer* renderer);
    void handleEvent(const SDL_Event& event);

private:
    float x;
    float y;
    float speed;
    Direction facing;
    bool isPunching;
    float punchTimer;
    const float punchDuration = 0.25f;
};