#include "Projectile.h"

Projectile::Projectile(
    float startX,
    float startY,
    float velocityX,
    float velocityY
)
    : x(startX),
      y(startY),
      velocityX(velocityX),
      velocityY(velocityY)
{
}

void Projectile::update(float deltaTime) {
    x += velocityX * deltaTime;
    y += velocityY * deltaTime;
}

void Projectile::render(SDL_Renderer* renderer) {
    SDL_FRect projectileRect = {
        x,
        y,
        width,
        height
    };

    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
    SDL_RenderFillRect(renderer, &projectileRect);
}

SDL_FRect Projectile::getBounds() const {
    return {
        x,
        y,
        width,
        height
    };
}

bool Projectile::isOffScreen() const {
    return (
        x + width < 0.0f ||
        x > 800.0f ||
        y + height < 0.0f ||
        y > 600.0f
    );
}