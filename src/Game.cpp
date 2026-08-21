#include "Game.h"

Game::Game()
    : window(nullptr),
      renderer(nullptr),
      running(false)
{
}

Game::~Game()
{
    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
    }

    if (window)
    {
        SDL_DestroyWindow(window);
    }

    SDL_Quit();
}

bool Game::initialize()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow(
        "Dungeon Clearer",
        800,
        600,
        0
    );

    if (!window)
    {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, nullptr);

    if (!renderer)
    {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        return false;
    }

    running = true;
    return true;
}

void Game::run()
{
    Uint64 previousTime = SDL_GetTicks();

    while (running)
    {
        Uint64 currentTime = SDL_GetTicks();

        float deltaTime =
            (currentTime - previousTime) / 1000.0f;

        previousTime = currentTime;

        processInput();
        update(deltaTime);
        render();
    }
}

void Game::processInput()
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            running = false;
        }
        
        player.handleEvent(event);
    }
}

void Game::update(float deltaTime)
{
    player.update(deltaTime);
}

void Game::render()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    player.render(renderer);
    player.renderPunch(renderer);

    SDL_RenderPresent(renderer);
}