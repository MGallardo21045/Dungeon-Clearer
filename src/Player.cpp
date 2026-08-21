#include "Player.h"

Player::Player()
    : x(375.0f),
      y(275.0f),
      speed(200.0f),
      facing(Direction::Down),
      isPunching(false),
      punchTimer(0.0f)
{
}

void Player::update(float deltaTime)
{
    const bool* keyboard = SDL_GetKeyboardState(nullptr);

    if (keyboard[SDL_SCANCODE_W]) {
        y -= speed * deltaTime; 
        facing = Direction::Up;
    }
    if (keyboard[SDL_SCANCODE_S]) {
        y += speed * deltaTime; 
        facing = Direction::Down;
    }
    if (keyboard[SDL_SCANCODE_A]) {
        x -= speed * deltaTime; 
        facing = Direction::Left;
    }
    if (keyboard[SDL_SCANCODE_D]) {
        x += speed * deltaTime; 
        facing = Direction::Right;
    }

    if (x < 0.0f) x = 0.0f;
    if (x > 750.0f) x = 750.0f;

    if (y < 0.0f) y = 0.0f;
    if (y > 550.0f) y = 550.0f;

    if (isPunching) {
        punchTimer -= deltaTime;
        if (punchTimer <= 0.0f) {
            isPunching = false;
            punchTimer = 0.0f;
        }
    }
}

void Player::render(SDL_Renderer* renderer)
{
    SDL_FRect playerRect = {
        x,
        y,
        50.0f,
        50.0f
    };

    switch (facing) {
        case Direction::Up:          
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        break;
        
        case Direction::Down:
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        break;
        
        case Direction::Left:
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        break;
        
        case Direction::Right:
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        break;
    }

    SDL_RenderFillRect(renderer, &playerRect);
}

void Player::renderPunch(SDL_Renderer* renderer)
{
    if (!isPunching) {
        return;
    }

    SDL_FRect punchRect;

    switch (facing)
    {
        case Direction::Up:
            punchRect = { x + 10.0f, y - 30.0f, 30.0f, 30.0f };
            break;

        case Direction::Down:
            punchRect = { x + 10.0f, y + 50.0f, 30.0f, 30.0f };
            break;

        case Direction::Left:
            punchRect = { x - 30.0f, y + 10.0f, 30.0f, 30.0f };
            break;

        case Direction::Right:
            punchRect = { x + 50.0f, y + 10.0f, 30.0f, 30.0f };
            break;
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &punchRect);
}

void Player::handleEvent(const SDL_Event& event)
{
    if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat)
    {
        if (event.key.scancode == SDL_SCANCODE_SPACE && !isPunching)
        {
            isPunching = true;
            punchTimer = punchDuration;
        }
    }
}