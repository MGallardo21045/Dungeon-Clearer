#pragma once

#include <SDL3/SDL.h>

enum class Direction {
    Up,
    Down,
    Left,
    Right
};

class Player {
public:
    Player();

    void update(float deltaTime);
    void render(SDL_Renderer* renderer);
    void renderPunch(SDL_Renderer* renderer);
    void handleEvent(const SDL_Event& event);
    bool isPunchActive() const;
    SDL_FRect getPunchBounds() const;
    SDL_FRect getBounds() const;
    float getX() const;
    float getY() const;
    void setPosition(float newX, float newY);
    void moveX(float amount);
    void moveY(float amount);
    float getSpeed() const;
    void setFacing(Direction newFacing);
    bool punchHasHit;
    bool hasPunchHit() const;
    void markPunchHit();
    void takeDamage(int amount);
    bool isAlive() const;
    int getHealth() const;
    int getComboStep() const;

private:
    float x;
    float y;
    float speed;
    Direction facing;
    bool isPunching;
    float punchTimer;
    const float punchDuration = 0.15f;
    int health;
    float invulnerabilityTimer;
    const float invulnerabilityDuration = 1.0f;
    int comboStep;
    float comboTimer;
    const float comboWindow = 0.5f;
    bool attackKeyHeld;
};