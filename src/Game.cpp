#include "Game.h"

Game::Game()
    : window(nullptr),
      renderer(nullptr),
      running(false),
      enemy(550.0f, 275.0f)
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

    const bool* keyboard = SDL_GetKeyboardState(nullptr);
    
    float moveX = 0.0f;
    float moveY = 0.0f;
    
    if (keyboard[SDL_SCANCODE_W]) {
        moveY -= player.getSpeed() * deltaTime;
        player.setFacing(Direction::Up);
    }
    if (keyboard[SDL_SCANCODE_S]) {
        moveY += player.getSpeed() * deltaTime;
        player.setFacing(Direction::Down);
    }
    if (keyboard[SDL_SCANCODE_A]) {
        moveX -= player.getSpeed() * deltaTime;
        player.setFacing(Direction::Left);
    }
    if (keyboard[SDL_SCANCODE_D]) {
        moveX += player.getSpeed() * deltaTime;
        player.setFacing(Direction::Right);
    }

    float oldX = player.getX();

    player.moveX(moveX);

    SDL_FRect playerBounds = player.getBounds();
    SDL_FRect enemyBounds = enemy.getBounds();

    if (SDL_HasRectIntersectionFloat(&playerBounds, &enemyBounds))
    {
        player.setPosition(oldX, player.getY());
    }

    float oldY = player.getY();

    player.moveY(moveY);

    playerBounds = player.getBounds();

    if (SDL_HasRectIntersectionFloat(&playerBounds, &enemyBounds))
    {
        player.setPosition(player.getX(), oldY);
    }

    enemy.setHit(false);

    if (player.isPunchActive())
    {
        SDL_FRect punch = player.getPunchBounds();

        if (SDL_HasRectIntersectionFloat(&punch, &enemyBounds))
        {
            enemy.setHit(true);
        }
    }

    enemy.setHit(false);
    
    if (enemy.isAlive() && player.isPunchActive() && !player.hasPunchHit()) {
        SDL_FRect punch = player.getPunchBounds();
        SDL_FRect enemyBounds = enemy.getBounds();

    if (SDL_HasRectIntersectionFloat(&punch, &enemyBounds)) {
        enemy.setHit(true);
        enemy.takeDamage(10);
        player.markPunchHit();
    }
}
}

void Game::render()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    player.render(renderer);
    player.renderPunch(renderer);
    enemy.render(renderer);

    SDL_RenderPresent(renderer);
}

