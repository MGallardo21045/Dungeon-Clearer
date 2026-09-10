#pragma once

#include <SDL3/SDL.h>

enum class StatusEffect {
    Burn,
    Shock,
    Freeze
};

class Enemy {
public:
    Enemy(float startX, float startY);

    void render(SDL_Renderer* renderer) const;

    SDL_FRect getBounds() const;

    void setHit(bool value);

    void takeDamage(int amount);

    void update(
        float deltaTime,
        float playerX,
        float playerY
    );

    bool isAlive() const;

    void moveX(float amount);

    void moveY(float amount);

    float getX() const;

    float getY() const;

    float getSpeed() const;

    void setPosition(
        float newX,
        float newY
    );

    int getLastSwordAttackHit() const;

    void setLastSwordAttackHit(int attackId);

    void applyBurn();
    bool isBurning() const;

    void applyShock();
    bool isShocked() const;

    void applyFreeze();
    bool isFrozen() const;

    void applyStatusEffect(StatusEffect effect);
    bool hasStatusEffect() const;

private:
    float x;
    float y;

    float width;
    float height;

    bool wasHit;

    int health;

    float speed;

    int lastSwordAttackHit;

    bool burning;

    float burnDurationTimer;
    const float burnDuration = 3.0f;

    float burnTickTimer;
    const float burnTickInterval = 0.5f;

    const int burnDamage = 2;

    bool shocked;

    float shockDurationTimer;
    const float shockDuration = 2.0f;

    const float shockSpeedMultiplier = 0.5f;

    bool frozen;

    float freezeDurationTimer;
    const float freezeDuration = 1.5f;
};