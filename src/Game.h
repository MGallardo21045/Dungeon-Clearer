#pragma once

#include <SDL3/SDL.h>
#include <vector>

#include "Player.h"
#include "Enemy.h"
#include "Projectile.h"

class Game {
public:
    Game();
    ~Game();

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    bool initialize();
    void run();

private:
    static constexpr int kWindowWidth = 800;
    static constexpr int kWindowHeight = 600;
    static constexpr int kContactDamage = 10;
    static constexpr float kKnockbackDistance = 20.0f;

    void processInput();
    void update(float deltaTime);
    void render();

    void updatePlayerMovement(float deltaTime);
    void updateEnemies(float deltaTime);
    void updateMeleeAttacks();
    void spawnProjectiles();
    void updateProjectiles(float deltaTime);

    bool intersectsLivingEnemy(const SDL_FRect& bounds, const Enemy* ignored = nullptr) const;
    bool intersectsPlayer(const SDL_FRect& bounds) const;
    void applyContactDamage();
    void applyKnockback(Enemy& enemy, Direction direction, float distance);
    void spawnProjectile(ProjectileType type, float speed, float size, int damage, bool atEdge);
    void resolveProjectileHit(const Projectile& projectile, Enemy& target);

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    bool running = false;
    bool gameOver = false;

    Player player;
    std::vector<Enemy> enemies;
    std::vector<Projectile> projectiles;
};