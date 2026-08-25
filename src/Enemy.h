#pragma once

#include <SDL3/SDL.h>

class Enemy {
public:
    Enemy(float startX, float startY);
    void render(SDL_Renderer* renderer) const;
    SDL_FRect getBounds() const;
    void setHit(bool value);
    void takeDamage(int amount);
    void update(float deltaTime, float playerX, float playerY);
    bool isAlive() const;
    void moveX(float amount);
    void moveY(float amount);
    float getX() const;
    float getY() const;
    float getSpeed() const;
    void setPosition(float newX, float newY);

private:
    float x;
    float y;
    float width;
    float height;
    bool wasHit;
    int health;
    float speed;
};