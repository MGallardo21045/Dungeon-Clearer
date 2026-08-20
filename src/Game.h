#pragma once

#include <SDL3/SDL.h>
#include "Player.h"

class Game
{
public:
    Game();
    ~Game();

    bool initialize();
    void run();

private:
    void processInput();
    void update(float deltaTime);
    void render();

    SDL_Window* window;
    SDL_Renderer* renderer;

    bool running;

    Player player;
};