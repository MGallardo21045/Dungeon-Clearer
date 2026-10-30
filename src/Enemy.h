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

    void update(float deltaTime);
    void render(SDL_Renderer* renderer) const;

    SDL_FRect getBounds() const;

    void takeDamage(int amount);
    bool isAlive() const;

    void moveX(float amount);
    void moveY(float amount);

    float getX() const;
    float getY() const;
    float getSpeed() const;

    void setPosition(float newX, float newY);

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

    int health;
    float speed;

    int lastSwordAttackHit;

    float hitFlashTimer = 0.0f;
    static constexpr float hitFlashDuration = 0.12f;

    bool burning;
    float burnDurationTimer;
    float burnTickTimer;

    static constexpr float burnDuration = 3.0f;
    static constexpr float burnTickInterval = 0.5f;
    static constexpr int burnDamage = 2;

    bool shocked;
    static constexpr float shockSpeedMultiplier = 0.5f;

    bool frozen;
    float freezeDurationTimer;

    static constexpr float freezeDuration = 1.5f;
};