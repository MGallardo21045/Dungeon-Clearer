#include "Enemy.h"

Enemy::Enemy(float startX, float startY)
    : x(startX),
      y(startY),
      width(50.0f),
      height(50.0f),
      wasHit(false),
      health(50),
      speed(100.0f)
{
}

void Enemy::render(SDL_Renderer* renderer) const {
    if (!isAlive()) {
        return;
    }

    SDL_FRect enemyRect = {
        x,
        y,
        width,
        height
    };
    if (wasHit) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
    }

    SDL_RenderFillRect(renderer, &enemyRect);
}

SDL_FRect Enemy::getBounds() const {
    return { x, y, width, height };
}

void Enemy::setHit(bool value) {
    wasHit = value;
}

void Enemy::takeDamage(int amount) {
    health -= amount;

    if (health < 0) {
        health = 0;
    }

    SDL_Log("Enemy health: %d", health);
}

bool Enemy::isAlive() const {
    return health > 0;
}

void Enemy::update(float deltaTime, float playerX, float playerY) {
    if (!isAlive()) {
        return;
    }
}

void Enemy::moveX(float amount) {
    x += amount;
}

void Enemy::moveY(float amount) {
    y += amount;
}

float Enemy::getX() const {
    return x;
}

float Enemy::getY() const {
    return y;
}

float Enemy::getSpeed() const {
    return speed;
}

void Enemy::setPosition(float newX, float newY) {
    x = newX;
    y = newY;
}