#include "Player.h"

Player::Player()
    : x(375.0f),
      y(275.0f),
      speed(200.0f)
{
}

void Player::update(float deltaTime)
{
    const bool* keyboard = SDL_GetKeyboardState(nullptr);

    if (keyboard[SDL_SCANCODE_W]) y -= speed * deltaTime;
    if (keyboard[SDL_SCANCODE_S]) y += speed * deltaTime;
    if (keyboard[SDL_SCANCODE_A]) x -= speed * deltaTime;
    if (keyboard[SDL_SCANCODE_D]) x += speed * deltaTime;

    if (x < 0.0f) x = 0.0f;
    if (x > 750.0f) x = 750.0f;

    if (y < 0.0f) y = 0.0f;
    if (y > 550.0f) y = 550.0f;
}

void Player::render(SDL_Renderer* renderer)
{
    SDL_FRect playerRect = {
        x,
        y,
        50.0f,
        50.0f
    };

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &playerRect);
}