#pragma once

#include <SDL3/SDL.h>

enum class ProjectileType {
    Blaster,
    Cannon,
    Mage
};

class Projectile {
public:
    Projectile(
        float startX,
        float startY,
        float velocityX,
        float velocityY,
        float projectileWidth,
        float projectileHeight,
        int projectileDamage,
        ProjectileType projectileType
    );

    void update(float deltaTime);
    void render(SDL_Renderer* renderer);

    SDL_FRect getBounds() const;
    bool isOffScreen() const;

    int getDamage() const;
    bool isExplosive() const;
    ProjectileType getType() const;
    void setMageShotInfo(
        int shotNumber,
        int statusShot
    );

    int getMageShotNumber() const;
    int getMageStatusShot() const;

private:
    float x;
    float y;
    float velocityX;
    float velocityY;
    float width;
    float height;

    int damage;

    ProjectileType type;

    int mageShotNumber;
    int mageStatusShot;
};