#include "Enemy.h"

Enemy::Enemy(float startX, float startY)
    : x(startX),
      y(startY),
      width(50.0f),
      height(50.0f),
      wasHit(false),
      health(50),
      speed(100.0f),
      lastSwordAttackHit(-1),
      burning(false),
      burnDurationTimer(0.0f),
      burnTickTimer(0.0f),
      shocked(false),
      shockDurationTimer(0.0f),
      frozen(false),
      freezeDurationTimer(0.0f)
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
        SDL_SetRenderDrawColor(
            renderer,
            255,
            255,
            255,
            255
        );
    }
    else if (frozen) {
        SDL_SetRenderDrawColor(
            renderer,
            100,
            200,
            255,
            255
        );
    }
    else if (burning) {
        SDL_SetRenderDrawColor(
            renderer,
            255,
            120,
            0,
            255
        );
    }
    else if (shocked) {
        SDL_SetRenderDrawColor(
            renderer,
            255,
            255,
            0,
            255
        );
    }
    else {
        SDL_SetRenderDrawColor(
            renderer,
            255,
            0,
            255,
            255
        );
    }

    SDL_RenderFillRect(
        renderer,
        &enemyRect
    );
}

SDL_FRect Enemy::getBounds() const {
    return {
        x,
        y,
        width,
        height
    };
}

void Enemy::setHit(bool value) {
    wasHit = value;
}

void Enemy::takeDamage(int amount) {
    health -= amount;

    if (health < 0) {
        health = 0;
    }

    SDL_Log(
        "Enemy health: %d",
        health
    );
}

bool Enemy::isAlive() const {
    return health > 0;
}

void Enemy::update(
    float deltaTime,
    float playerX,
    float playerY
) {
    if (!isAlive()) {
        return;
    }

    if (burning) {
        burnDurationTimer -= deltaTime;
        burnTickTimer -= deltaTime;

        if (burnTickTimer <= 0.0f) {
            takeDamage(burnDamage);

            burnTickTimer =
                burnTickInterval;
        }

        if (burnDurationTimer <= 0.0f) {
            burnDurationTimer = 0.0f;
            burning = false;

            SDL_Log("Burn ended");
        }
    }

    if (!isAlive()) {
        return;
    }

    if (shocked) {
        shockDurationTimer -= deltaTime;

        if (shockDurationTimer <= 0.0f) {
            shockDurationTimer = 0.0f;
            shocked = false;

            SDL_Log("Shock ended");
        }
    }

    if (frozen) {
        freezeDurationTimer -= deltaTime;

        if (freezeDurationTimer <= 0.0f) {
            freezeDurationTimer = 0.0f;
            frozen = false;

            SDL_Log("Freeze ended");
        }
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
    if (frozen) {
        return 0.0f;
    }

    if (shocked) {
        return speed * shockSpeedMultiplier;
    }

    return speed;
}

void Enemy::setPosition(
    float newX,
    float newY
) {
    x = newX;
    y = newY;
}

int Enemy::getLastSwordAttackHit() const {
    return lastSwordAttackHit;
}

void Enemy::setLastSwordAttackHit(
    int attackId
) {
    lastSwordAttackHit = attackId;
}

void Enemy::applyBurn() {
    if (!isAlive()) {
        return;
    }

    burning = true;

    burnDurationTimer =
        burnDuration;

    burnTickTimer =
        burnTickInterval;

    SDL_Log("Enemy is burning!");
}

bool Enemy::isBurning() const {
    return burning;
}

void Enemy::applyShock() {
    if (!isAlive()) {
        return;
    }

    shocked = true;

    shockDurationTimer =
        shockDuration;

    SDL_Log("Enemy is shocked!");
}

bool Enemy::isShocked() const {
    return shocked;
}

void Enemy::applyFreeze() {
    if (!isAlive()) {
        return;
    }

    frozen = true;

    freezeDurationTimer =
        freezeDuration;

    SDL_Log("Enemy is frozen!");
}

bool Enemy::isFrozen() const {
    return frozen;
}

void Enemy::applyStatusEffect(StatusEffect effect) {
    switch (effect) {
        case StatusEffect::Burn:
            applyBurn();
            break;

        case StatusEffect::Shock:
            applyShock();
            break;

        case StatusEffect::Freeze:
            applyFreeze();
            break;
    }
}

bool Enemy::hasStatusEffect() const {
    return burning || shocked || frozen;
}