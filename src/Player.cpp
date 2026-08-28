#include "Player.h"

Player::Player()
    : x(375.0f),
      y(275.0f),
      speed(200.0f),
      facing(Direction::Down),
      isPunching(false),
      punchTimer(0.0f),
      punchHasHit(false),
      health(100),
      invulnerabilityTimer(0.0f),
      comboStep(0),
      comboTimer(0.0f),
      attackKeyHeld(false)
{
}

void Player::update(float deltaTime) {
    if (isPunching) {
        punchTimer -= deltaTime;

        if (punchTimer <= 0.0f) {
            isPunching = false;
            punchTimer = 0.0f;
        }
    }
    if (invulnerabilityTimer > 0.0f) {
        invulnerabilityTimer -= deltaTime;

        if (invulnerabilityTimer < 0.0f) {
            invulnerabilityTimer = 0.0f;
        }
    }
    if (comboTimer > 0.0f) {
        comboTimer -= deltaTime;
        if (comboTimer <= 0.0f) {
            comboTimer = 0.0f;
            comboStep = 0;
        }
    }
}

void Player::render(SDL_Renderer* renderer) {
    SDL_FRect playerRect = {
        x,
        y,
        50.0f,
        50.0f
    };

    switch (facing) {
        case Direction::Up:          
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        break;
        
        case Direction::Down:
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        break;
        
        case Direction::Left:
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        break;
        
        case Direction::Right:
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        break;
    }

    SDL_RenderFillRect(renderer, &playerRect);
}

void Player::renderPunch(SDL_Renderer* renderer) {
    if (!isPunching) {
        return;
    }

    SDL_FRect punchRect = getPunchBounds();

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &punchRect);
}

void Player::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        if (event.key.scancode == SDL_SCANCODE_SPACE &&
        !attackKeyHeld &&
        !isPunching) {
            attackKeyHeld = true;
            comboStep++;
            
            if (comboStep > 3) {
                comboStep = 1;
            }

            isPunching = true;
            punchHasHit = false;

            if (comboStep == 3) {
                punchTimer = 0.10f;
            }
            else {
                punchTimer = 0.15f;
            }

            comboTimer = comboWindow;

            SDL_Log("Combo step: %d", comboStep);
        }
    }

    if (event.type == SDL_EVENT_KEY_UP) {
        if (event.key.scancode == SDL_SCANCODE_SPACE) {
            attackKeyHeld = false;
        }
    }
}

bool Player::isPunchActive() const {
    return isPunching;
}

SDL_FRect Player::getPunchBounds() const {
    float size = 30.0f;

    if (comboStep == 3) {
        size = 40.0f;
    }

    float centerOffset = (50.0f - size) / 2.0f;

    float handOffset = 0.0f;

    if (comboStep == 1) {
        handOffset = 5.0f;
    }
    else if (comboStep == 2) {
        handOffset = -5.0f;
    }

    switch (facing) {
        case Direction::Up:
            return {
                x + centerOffset + handOffset,
                y - size,
                size,
                size
            };

        case Direction::Down:
            return {
                x + centerOffset - handOffset,
                y + 50.0f,
                size,
                size
            };

        case Direction::Left:
            return {
                x - size,
                y + centerOffset - handOffset,
                size,
                size
            };

        case Direction::Right:
            return {
                x + 50.0f,
                y + centerOffset + handOffset,
                size,
                size
            };
    }

    return { 0, 0, 0, 0 };
}

SDL_FRect Player::getBounds() const {
    return { x, y, 50.0f, 50.0f };
}

float Player::getX() const {
    return x;
}

float Player::getY() const {
    return y;
}

void Player::setPosition(float newX, float newY) {
    x = newX;
    y = newY;
}

void Player::moveX(float amount) {
    x += amount;

    if (x < 0.0f) x = 0.0f;
    if (x > 750.0f) x = 750.0f;
}

void Player::moveY(float amount) {
    y += amount;

    if (y < 0.0f) y = 0.0f;
    if (y > 550.0f) y = 550.0f;
}

float Player::getSpeed() const {
    return speed;
}

void Player::setFacing(Direction newFacing) {
    facing = newFacing;
}

bool Player::hasPunchHit() const {
    return punchHasHit;
}

void Player::markPunchHit() {
    punchHasHit = true;
}

void Player::takeDamage(int amount) {
    if (invulnerabilityTimer > 0.0f) {
        return;
    }

    health -= amount;

    if (health < 0) {
        health = 0;
    }

    invulnerabilityTimer = invulnerabilityDuration;

    SDL_Log("Player health: %d", health);
}

bool Player::isAlive() const {
    return health > 0;
}

int Player::getHealth() const {
    return health;
}

int Player::getComboStep() const {
    return comboStep;
}

Direction Player::getFacing() const {
    return facing;
}