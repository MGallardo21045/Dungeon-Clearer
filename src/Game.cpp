#include "Game.h"
#include <cmath>

Game::Game()
    : window(nullptr),
      renderer(nullptr),
      running(false)
{
    enemies.emplace_back(550.0f, 275.0f);
    enemies.emplace_back(150.0f, 150.0f);
    enemies.emplace_back(600.0f, 450.0f);
}

Game::~Game() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }

    if (window) {
        SDL_DestroyWindow(window);
    }

    SDL_Quit();
}

bool Game::initialize() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow(
        "Dungeon Clearer",
        800,
        600,
        0
    );

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
        Uint64 currentTime = SDL_GetTicks();

        float deltaTime =
            (currentTime - previousTime) / 1000.0f;

        previousTime = currentTime;

        processInput();
        update(deltaTime);
        render();
    }
}

void Game::processInput() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            running = false;
        }

        player.handleEvent(event);
    }
}

void Game::update(float deltaTime) {
    player.update(deltaTime);

    const bool* keyboard = SDL_GetKeyboardState(nullptr);

    float moveX = 0.0f;
    float moveY = 0.0f;

    if (keyboard[SDL_SCANCODE_W]) {
        moveY -= player.getSpeed() * deltaTime;
        player.setFacing(Direction::Up);
    }

    if (keyboard[SDL_SCANCODE_S]) {
        moveY += player.getSpeed() * deltaTime;
        player.setFacing(Direction::Down);
    }

    if (keyboard[SDL_SCANCODE_A]) {
        moveX -= player.getSpeed() * deltaTime;
        player.setFacing(Direction::Left);
    }

    if (keyboard[SDL_SCANCODE_D]) {
        moveX += player.getSpeed() * deltaTime;
        player.setFacing(Direction::Right);
    }

    float oldX = player.getX();

    player.moveX(moveX);

    SDL_FRect playerBounds = player.getBounds();

    bool collidedOnX = false;

    for (Enemy& enemy : enemies) {
        if (!enemy.isAlive()) {
            continue;
        }

        SDL_FRect enemyBounds = enemy.getBounds();

        if (SDL_HasRectIntersectionFloat(
            &playerBounds,
            &enemyBounds)) {

            collidedOnX = true;
            break;
        }
    }

    if (collidedOnX) {
        player.setPosition(oldX, player.getY());
    }

    float oldY = player.getY();

    player.moveY(moveY);

    playerBounds = player.getBounds();

    bool collidedOnY = false;

    for (Enemy& enemy : enemies) {
        if (!enemy.isAlive()) {
            continue;
        }

        SDL_FRect enemyBounds = enemy.getBounds();

        if (SDL_HasRectIntersectionFloat(
            &playerBounds,
            &enemyBounds)) {

            collidedOnY = true;
            break;
        }
    }

    if (collidedOnY) {
        player.setPosition(player.getX(), oldY);
    }

    for (Enemy& enemy : enemies) {
        if (!enemy.isAlive()) {
            continue;
        }

        float directionX =
            player.getX() - enemy.getX();

        float directionY =
            player.getY() - enemy.getY();

        float length = std::sqrt(
            directionX * directionX +
            directionY * directionY
        );

        float enemyMoveX = 0.0f;
        float enemyMoveY = 0.0f;

        if (length > 0.0f) {
            directionX /= length;
            directionY /= length;

            enemyMoveX =
                directionX *
                enemy.getSpeed() *
                deltaTime;

            enemyMoveY =
                directionY *
                enemy.getSpeed() *
                deltaTime;
        }

        SDL_FRect currentPlayerBounds =
            player.getBounds();

        float oldEnemyX =
            enemy.getX();

        enemy.moveX(enemyMoveX);

        SDL_FRect enemyBounds =
            enemy.getBounds();

        bool collidedWithEnemyOnX =
            false;

        for (Enemy& otherEnemy : enemies) {
            if (&otherEnemy == &enemy) {
                continue;
            }

            if (!otherEnemy.isAlive()) {
                continue;
            }

            SDL_FRect otherBounds =
                otherEnemy.getBounds();

            if (SDL_HasRectIntersectionFloat(
                &enemyBounds,
                &otherBounds)) {

                collidedWithEnemyOnX =
                    true;

                break;
            }
        }

        bool collidedWithPlayerOnX =
            SDL_HasRectIntersectionFloat(
                &enemyBounds,
                &currentPlayerBounds
            );

        if (collidedWithPlayerOnX) {
            player.takeDamage(10);
        }

        if (collidedWithPlayerOnX ||
            collidedWithEnemyOnX) {

            enemy.setPosition(
                oldEnemyX,
                enemy.getY()
            );
        }

        float oldEnemyY =
            enemy.getY();

        enemy.moveY(enemyMoveY);

        enemyBounds =
            enemy.getBounds();

        bool collidedWithEnemyOnY =
            false;

        for (Enemy& otherEnemy : enemies) {
            if (&otherEnemy == &enemy) {
                continue;
            }

            if (!otherEnemy.isAlive()) {
                continue;
            }

            SDL_FRect otherBounds =
                otherEnemy.getBounds();

            if (SDL_HasRectIntersectionFloat(
                &enemyBounds,
                &otherBounds)) {

                collidedWithEnemyOnY =
                    true;

                break;
            }
        }

        bool collidedWithPlayerOnY =
            SDL_HasRectIntersectionFloat(
                &enemyBounds,
                &currentPlayerBounds
            );

        if (collidedWithPlayerOnY) {
            player.takeDamage(10);
        }

        if (collidedWithPlayerOnY ||
            collidedWithEnemyOnY) {

            enemy.setPosition(
                enemy.getX(),
                oldEnemyY
            );
        }
    }

    for (Enemy& enemy : enemies) {
        enemy.setHit(false);
    }

   if (player.isPunchActive() &&
        !player.hasPunchHit()) {

        SDL_FRect punchBounds =
            player.getPunchBounds();

        for (Enemy& enemy : enemies) {
            if (!enemy.isAlive()) {
                continue;
            }

            SDL_FRect enemyBounds =
                enemy.getBounds();

            if (SDL_HasRectIntersectionFloat(
                &punchBounds,
                &enemyBounds)) {

                enemy.setHit(true);

                int damage = 10;

                if (player.getComboStep() == 3) {
                    damage = 15;
                }

                enemy.takeDamage(damage);

                if (player.getComboStep() == 3) {
                    float knockbackDistance =
                        20.0f;

                    switch (player.getFacing()) {
                        case Direction::Up:
                            enemy.moveY(
                                -knockbackDistance
                            );
                            break;

                        case Direction::Down:
                            enemy.moveY(
                                knockbackDistance
                            );
                            break;

                        case Direction::Left:
                            enemy.moveX(
                                -knockbackDistance
                            );
                            break;

                        case Direction::Right:
                            enemy.moveX(
                                knockbackDistance
                            );
                            break;
                    }
                }

                player.markPunchHit();

                break;
            }
        }
    }

    if (player.isSwordSwingActive()) {
        SDL_FRect swordBounds =
            player.getSwordBounds();

        for (Enemy& enemy : enemies) {
            if (!enemy.isAlive()) {
                continue;
            }

            SDL_FRect enemyBounds =
                enemy.getBounds();

            if (SDL_HasRectIntersectionFloat(
                &swordBounds,
                &enemyBounds)) {

                if (
                    enemy.getLastSwordAttackHit() !=
                    player.getSwordAttackId()
                ) {

                    int swordDamage = 20;

                    switch (
                        player.getSwordComboStep()
                    ) {
                        case 1:
                            swordDamage = 20;
                            break;

                        case 2:
                            swordDamage = 20;
                            break;

                        case 3:
                            swordDamage = 15;
                            break;

                        case 4:
                            swordDamage = 30;
                            break;

                        default:
                            swordDamage = 20;
                            break;
                    }

                    enemy.takeDamage(
                        swordDamage
                    );

                    enemy.setLastSwordAttackHit(
                        player.getSwordAttackId()
                    );
                }
            }
        }
    }

    if (player.getWantsToFire()) {
        float projectileSpeed =
            500.0f;

        float velocityX = 0.0f;
        float velocityY = 0.0f;

        switch (player.getFacing()) {
            case Direction::Up:
                velocityY =
                    -projectileSpeed;
                break;

            case Direction::Down:
                velocityY =
                    projectileSpeed;
                break;

            case Direction::Left:
                velocityX =
                    -projectileSpeed;
                break;

            case Direction::Right:
                velocityX =
                    projectileSpeed;
                break;
        }

        projectiles.emplace_back(
            player.getX() + 20.0f,
            player.getY() + 20.0f,
            velocityX,
            velocityY,
            10.0f,
            10.0f,
            5,
            ProjectileType::Blaster
        );

        player.clearWantsToFire();
    }

    if (player.getWantsToFireCannon()) {
        float projectileSpeed =
            300.0f;

        float projectileSize =
            35.0f;

        float velocityX = 0.0f;
        float velocityY = 0.0f;

        float projectileX =
            player.getX();

        float projectileY =
            player.getY();

        switch (player.getFacing()) {
            case Direction::Up:
                velocityY =
                    -projectileSpeed;

                projectileX =
                    player.getX() +
                    (50.0f - projectileSize)
                    / 2.0f;

                projectileY =
                    player.getY() -
                    projectileSize;

                break;

            case Direction::Down:
                velocityY =
                    projectileSpeed;

                projectileX =
                    player.getX() +
                    (50.0f - projectileSize)
                    / 2.0f;

                projectileY =
                    player.getY() +
                    50.0f;

                break;

            case Direction::Left:
                velocityX =
                    -projectileSpeed;

                projectileX =
                    player.getX() -
                    projectileSize;

                projectileY =
                    player.getY() +
                    (50.0f - projectileSize)
                    / 2.0f;

                break;

            case Direction::Right:
                velocityX =
                    projectileSpeed;

                projectileX =
                    player.getX() +
                    50.0f;

                projectileY =
                    player.getY() +
                    (50.0f - projectileSize)
                    / 2.0f;

                break;
        }

        projectiles.emplace_back(
            projectileX,
            projectileY,
            velocityX,
            velocityY,
            projectileSize,
            projectileSize,
            40,
            ProjectileType::Cannon
        );

        player.clearWantsToFireCannon();
    }

    if (player.getWantsToFireMage()) {
        float projectileSpeed = 400.0f;
        float projectileSize = 18.0f;

        float velocityX = 0.0f;
        float velocityY = 0.0f;

        switch (player.getFacing()) {
            case Direction::Up:
            velocityY = -projectileSpeed;
            break;

            case Direction::Down:
            velocityY = projectileSpeed;
            break;

            case Direction::Left:
            velocityX = -projectileSpeed;
            break;

            case Direction::Right:
            velocityX = projectileSpeed;
            break;
        }

        float projectileX =
        player.getX() +
        (50.0f - projectileSize) / 2.0f;

        float projectileY =
        player.getY() +
        (50.0f - projectileSize) / 2.0f;

        projectiles.emplace_back(
        projectileX,
        projectileY,
        velocityX,
        velocityY,
        projectileSize,
        projectileSize,
        10,
        ProjectileType::Mage);

        player.clearWantsToFireMage();
    }

    auto projectileIt =
        projectiles.begin();

    while (
    projectileIt !=
    projectiles.end()
    ) {
        projectileIt->update(
            deltaTime
        );

        bool projectileDestroyed =
            false;

        SDL_FRect projectileBounds =
            projectileIt->getBounds();

        for (Enemy& enemy : enemies) {
            if (!enemy.isAlive()) {
                continue;
            }

            SDL_FRect enemyBounds =
                enemy.getBounds();
                
            if (
                SDL_HasRectIntersectionFloat(
                &projectileBounds,
                &enemyBounds)
            ) {
                if (projectileIt->isExplosive()) {
                    float explosionSize = 100.0f;

                    float explosionX =
                    projectileBounds.x +
                    projectileBounds.w / 2.0f -
                    explosionSize / 2.0f;

                    float explosionY =
                    projectileBounds.y +
                    projectileBounds.h / 2.0f -
                    explosionSize / 2.0f;

                    SDL_FRect explosionBounds = {
                        explosionX,
                        explosionY,
                        explosionSize,
                        explosionSize
                    };

                    for (Enemy& explosionEnemy : enemies) {
                        if (!explosionEnemy.isAlive()) {
                            continue;
                        }

                        SDL_FRect explosionEnemyBounds =
                        explosionEnemy.getBounds();

                        if (
                        SDL_HasRectIntersectionFloat(   
                        &explosionBounds,                        
                        &explosionEnemyBounds
                        )) {
                            explosionEnemy.takeDamage(
                            projectileIt->getDamage());
                        }
                    }
                }
                else {
                    enemy.takeDamage(
                    projectileIt->getDamage());
                }

                projectileIt =
                projectiles.erase(projectileIt);

                projectileDestroyed = true;

                break;
            }
        }

        if (
            !projectileDestroyed &&
            projectileIt->isOffScreen()
        ) {
            projectileIt =
                projectiles.erase(
                    projectileIt
                );

            projectileDestroyed =
                true;
        }

        if (!projectileDestroyed) {
            ++projectileIt;
        }
    }
}

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

    SDL_RenderPresent(renderer);
}