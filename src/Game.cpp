#include "Game.h"

#include <algorithm>
#include <cmath>

Game::Game() {
    enemies.emplace_back(550.0f, 275.0f);
    enemies.emplace_back(150.0f, 150.0f);
    enemies.emplace_back(600.0f, 450.0f);
}

Game::~Game() {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Game::initialize() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow("Dungeon Clearer", kWindowWidth, kWindowHeight, 0);
    if (!window) {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        return false;
    }

    running = true;
    return true;
}

void Game::run() {
    Uint64 previousTime = SDL_GetTicks();
    while (running) {
        const Uint64 currentTime = SDL_GetTicks();
        // CHANGE: Cap large frame jumps (e.g. dragging the window) to reduce tunneling.
        const float deltaTime = std::min((currentTime - previousTime) / 1000.0f, 0.05f);
        previousTime = currentTime;
        processInput();
        if (!running) break;
        update(deltaTime);
        render();
    }
}

void Game::processInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            running = false;
            continue;
        }
        if (!gameOver) player.handleEvent(event);
    }
}

void Game::update(float deltaTime) {
    if (gameOver) return;
    player.update(deltaTime);
    updatePlayerMovement(deltaTime);
    updateEnemies(deltaTime);
    if (gameOver) return;
    updateMeleeAttacks();
    spawnProjectiles();
    updateProjectiles(deltaTime);
}

bool Game::intersectsLivingEnemy(const SDL_FRect& bounds, const Enemy* ignored) const {
    for (const Enemy& enemy : enemies) {
        if (&enemy == ignored || !enemy.isAlive()) continue;
        const SDL_FRect other = enemy.getBounds();
        if (SDL_HasRectIntersectionFloat(&bounds, &other)) return true;
    }
    return false;
}

bool Game::intersectsPlayer(const SDL_FRect& bounds) const {
    const SDL_FRect playerBounds = player.getBounds();
    return SDL_HasRectIntersectionFloat(&bounds, &playerBounds);
}

void Game::applyContactDamage() {
    player.takeDamage(kContactDamage);
    if (!player.isAlive()) {
        gameOver = true;
        SDL_Log("GAME OVER");
    }
}

void Game::updatePlayerMovement(float deltaTime) {
    const bool* keys = SDL_GetKeyboardState(nullptr);
    float dx = 0.0f;
    float dy = 0.0f;
    const float step = player.getSpeed() * deltaTime;

    // Preserve the original facing priority: D > A > S > W.
    if (keys[SDL_SCANCODE_W]) { dy -= step; player.setFacing(Direction::Up); }
    if (keys[SDL_SCANCODE_S]) { dy += step; player.setFacing(Direction::Down); }
    if (keys[SDL_SCANCODE_A]) { dx -= step; player.setFacing(Direction::Left); }
    if (keys[SDL_SCANCODE_D]) { dx += step; player.setFacing(Direction::Right); }

    const float oldX = player.getX();
    player.moveX(dx);
    if (intersectsLivingEnemy(player.getBounds())) player.setPosition(oldX, player.getY());

    const float oldY = player.getY();
    player.moveY(dy);
    if (intersectsLivingEnemy(player.getBounds())) player.setPosition(player.getX(), oldY);
}

void Game::updateEnemies(float deltaTime) {
    for (Enemy& enemy : enemies) {
        if (!enemy.isAlive()) continue;
        enemy.update(deltaTime);
        if (!enemy.isAlive()) continue;

        float dx = player.getX() - enemy.getX();
        float dy = player.getY() - enemy.getY();
        const float distance = std::sqrt(dx * dx + dy * dy);
        if (distance <= 0.0f) continue;

        const float movement = enemy.getSpeed() * deltaTime;
        dx = dx / distance * movement;
        dy = dy / distance * movement;

        // CHANGE: Shared collision checks replace duplicated X/Y loops.
        const float oldX = enemy.getX();
        enemy.moveX(dx);
        SDL_FRect bounds = enemy.getBounds();
        const bool hitPlayerX = intersectsPlayer(bounds);
        if (hitPlayerX) applyContactDamage();
        if (gameOver) return;
        if (hitPlayerX || intersectsLivingEnemy(bounds, &enemy))
            enemy.setPosition(oldX, enemy.getY());

        const float oldY = enemy.getY();
        enemy.moveY(dy);
        bounds = enemy.getBounds();
        const bool hitPlayerY = intersectsPlayer(bounds);
        if (hitPlayerY) applyContactDamage();
        if (gameOver) return;
        if (hitPlayerY || intersectsLivingEnemy(bounds, &enemy))
            enemy.setPosition(enemy.getX(), oldY);
    }
}

void Game::applyKnockback(Enemy& enemy, Direction direction, float distance) {
    // CHANGE/BUG FIX: Advance in small increments and stop at the first obstacle.
    // The old 20-pixel teleport could place one enemy inside another indefinitely.
    float dx = 0.0f;
    float dy = 0.0f;
    switch (direction) {
        case Direction::Up: dy = -1.0f; break;
        case Direction::Down: dy = 1.0f; break;
        case Direction::Left: dx = -1.0f; break;
        case Direction::Right: dx = 1.0f; break;
    }
    float remaining = distance;
    while (remaining > 0.0f) {
        const float step = std::min(remaining, 1.0f);
        const float oldX = enemy.getX();
        const float oldY = enemy.getY();
        enemy.moveX(dx * step);
        enemy.moveY(dy * step);
        const SDL_FRect bounds = enemy.getBounds();
        if (intersectsLivingEnemy(bounds, &enemy) || intersectsPlayer(bounds)) {
            enemy.setPosition(oldX, oldY);
            break;
        }
        // Enemy::moveX/moveY may clamp at the arena boundary.
        if (enemy.getX() == oldX && enemy.getY() == oldY) break;
        remaining -= step;
    }
}

void Game::updateMeleeAttacks() {
    if (player.isPunchActive() && !player.hasPunchHit()) {
        const SDL_FRect punch = player.getPunchBounds();
        for (Enemy& enemy : enemies) {
            if (!enemy.isAlive()) continue;
            const SDL_FRect bounds = enemy.getBounds();
            if (!SDL_HasRectIntersectionFloat(&punch, &bounds)) continue;

            const bool finisher = player.getComboStep() == 3;
            enemy.takeDamage(finisher ? 15 : 10);
            if (finisher && enemy.isAlive())
                applyKnockback(enemy, player.getFacing(), kKnockbackDistance);
            player.markPunchHit();
            break;
        }
    }

    if (player.isSwordSwingActive()) {
        const SDL_FRect sword = player.getSwordBounds();
        for (Enemy& enemy : enemies) {
            if (!enemy.isAlive()) continue;
            const SDL_FRect bounds = enemy.getBounds();
            if (!SDL_HasRectIntersectionFloat(&sword, &bounds)) continue;
            if (enemy.getLastSwordAttackHit() == player.getSwordAttackId()) continue;

            int damage = 20;
            switch (player.getSwordComboStep()) {
                case 3: damage = 15; break;
                case 4: damage = 30; break;
                default: break;
            }
            enemy.takeDamage(damage);
            enemy.setLastSwordAttackHit(player.getSwordAttackId());
        }
    }
}

void Game::spawnProjectile(ProjectileType type, float speed, float size, int damage, bool atEdge) {
    const SDL_FRect body = player.getBounds();
    float x = body.x + (body.w - size) / 2.0f;
    float y = body.y + (body.h - size) / 2.0f;
    float vx = 0.0f;
    float vy = 0.0f;

    switch (player.getFacing()) {
        case Direction::Up: vy = -speed; if (atEdge) y = body.y - size; break;
        case Direction::Down: vy = speed; if (atEdge) y = body.y + body.h; break;
        case Direction::Left: vx = -speed; if (atEdge) x = body.x - size; break;
        case Direction::Right: vx = speed; if (atEdge) x = body.x + body.w; break;
    }

    projectiles.emplace_back(x, y, vx, vy, size, size, damage, type);
}

void Game::spawnProjectiles() {
    if (player.getWantsToFire()) {
        // Preserve Blaster's original +20 spawn offset on both axes.
        const SDL_FRect body = player.getBounds();
        float vx = 0.0f;
        float vy = 0.0f;
        switch (player.getFacing()) {
            case Direction::Up: vy = -500.0f; break;
            case Direction::Down: vy = 500.0f; break;
            case Direction::Left: vx = -500.0f; break;
            case Direction::Right: vx = 500.0f; break;
        }
        projectiles.emplace_back(body.x + 20.0f, body.y + 20.0f,
                                 vx, vy, 10.0f, 10.0f, 5, ProjectileType::Blaster);
        player.clearWantsToFire();
    }
    if (player.getWantsToFireCannon()) {
        spawnProjectile(ProjectileType::Cannon, 300.0f, 35.0f, 40, true);
        player.clearWantsToFireCannon();
    }
    if (player.getWantsToFireMage()) {
        spawnProjectile(ProjectileType::Mage, 400.0f, 18.0f, 10, false);
        projectiles.back().setMageShotInfo(player.getMageCurrentShotNumber(),
                                           player.getMageStatusShot());
        player.clearWantsToFireMage();
    }
}

void Game::resolveProjectileHit(const Projectile& projectile, Enemy& target) {
    if (projectile.isExplosive()) {
        const SDL_FRect bounds = projectile.getBounds();
        constexpr float explosionSize = 100.0f;
        const SDL_FRect explosion = {
            bounds.x + (bounds.w - explosionSize) / 2.0f,
            bounds.y + (bounds.h - explosionSize) / 2.0f,
            explosionSize, explosionSize
        };
        for (Enemy& enemy : enemies) {
            if (!enemy.isAlive()) continue;
            const SDL_FRect enemyBounds = enemy.getBounds();
            if (SDL_HasRectIntersectionFloat(&explosion, &enemyBounds))
                enemy.takeDamage(projectile.getDamage());
        }
        return;
    }

    // CHANGE: Status effects apply only if the enemy had no effect before this hit.
    const bool canApplyStatus = !target.hasStatusEffect();
    target.takeDamage(projectile.getDamage());
    if (projectile.getType() != ProjectileType::Mage || !canApplyStatus || !target.isAlive())
        return;
    const int shot = projectile.getMageShotNumber();
    if (shot == 0 || shot != projectile.getMageStatusShot()) return;
    switch (shot) {
        case 1: target.applyBurn(); break;
        case 2: target.applyShock(); break;
        case 3: target.applyFreeze(); break;
        default: break;
    }
}

void Game::updateProjectiles(float deltaTime) {
    auto it = projectiles.begin();
    while (it != projectiles.end()) {
        it->update(deltaTime);
        const SDL_FRect bounds = it->getBounds();
        bool hit = false;
        for (Enemy& enemy : enemies) {
            if (!enemy.isAlive()) continue;
            const SDL_FRect enemyBounds = enemy.getBounds();
            if (!SDL_HasRectIntersectionFloat(&bounds, &enemyBounds)) continue;
            resolveProjectileHit(*it, enemy);
            hit = true;
            break;
        }
        if (hit || it->isOffScreen(static_cast<float>(kWindowWidth),
                                   static_cast<float>(kWindowHeight))) {
            it = projectiles.erase(it);
        } else {
            ++it;
        }
    }
}

// Rendering order remains unchanged: player, attacks, enemies, projectiles, overlay.
void Game::render() {
    SDL_SetRenderDrawColor(
        renderer,
        0,
        0,
        0,
        255
    );

    SDL_RenderClear(renderer);

    player.render(renderer);
    player.renderPunch(renderer);
    player.renderSword(renderer);
    player.renderCannonCharge(renderer);
    player.renderMageCharge(renderer);

    for (Enemy& enemy : enemies) {
        enemy.render(renderer);
    }

    for (Projectile& projectile : projectiles) {
        projectile.render(renderer);
    }

    if (gameOver) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_SetRenderScale(renderer, 4.0f, 4.0f);
        SDL_RenderDebugText(renderer, 64.0f, 71.0f, "GAME OVER");
    }

    SDL_SetRenderScale(renderer, 1.0f, 1.0f);

    SDL_RenderPresent(renderer);
}