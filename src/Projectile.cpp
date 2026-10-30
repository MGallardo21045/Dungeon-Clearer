#include "Projectile.h"

Projectile::Projectile(
    float startX,
    float startY,
    float velocityX,
    float velocityY,
    float projectileWidth,
    float projectileHeight,
    int projectileDamage,
    ProjectileType projectileType
)
    : x(startX),
      y(startY),
      velocityX(velocityX),
      velocityY(velocityY),
      width(projectileWidth),
      height(projectileHeight),
      damage(projectileDamage),
      type(projectileType),
      mageShotNumber(0),
      mageStatusShot(0)
{
}

void Projectile::update(float deltaTime) {
    x += velocityX * deltaTime;
    y += velocityY * deltaTime;
}

void Projectile::render(SDL_Renderer* renderer) const {
    SDL_FRect projectileRect = getBounds();

    switch (type) {
        case ProjectileType::Blaster:
            SDL_SetRenderDrawColor(
                renderer, 0, 255, 255, 255
            );
            break;

        case ProjectileType::Cannon:
            SDL_SetRenderDrawColor(
                renderer, 0, 150, 255, 255
            );
            break;

        case ProjectileType::Mage:
            SDL_SetRenderDrawColor(
                renderer, 180, 0, 255, 255
            );
            break;
    }

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

bool Projectile::isOffScreen(
    float screenWidth,
    float screenHeight
) const {
    return (
        x + width < 0.0f ||
        x > screenWidth ||
        y + height < 0.0f ||
        y > screenHeight
    );
}

int Projectile::getDamage() const {
    return damage;
}

bool Projectile::isExplosive() const {
    return type == ProjectileType::Cannon;
}

ProjectileType Projectile::getType() const {
    return type;
}

void Projectile::setMageShotInfo(
    int shotNumber,
    int statusShot
) {
    mageShotNumber = shotNumber;
    mageStatusShot = statusShot;
}

int Projectile::getMageShotNumber() const {
    return mageShotNumber;
}

int Projectile::getMageStatusShot() const {
    return mageStatusShot;
}
