#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "Game.h"

int main(int argc, char* argv[])
{
    Game game;

    if (!game.initialize())
    {
        return 1;
    }

    game.run();

    return 0;
}