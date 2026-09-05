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
      attackKeyHeld(false),
      playerClass(PlayerClass::None),
      isSwinging(false),
      swingTimer(0.0f),
      swordComboStep(0),
      swordComboTimer(0.0f),
      swordOnCooldown(false),
      swordCooldownTimer(0.0f),
      swordAttackId(0),
      wantsToFire(false),
      blasterFireTimer(0.0f),
      blasterFireHeld(false),
      wantsToFireCannon(false),
      cannonFireTimer(0.0f),
      cannonCharging(false),
      cannonChargeTimer(0.0f),
      wantsToFireMage(false),
      mageCharging(false),
      mageChargeTimer(0.0f),
      mageShotsRemaining(0),
      mageBurstTimer(0.0f),
      mageCooldownTimer(0.0f)
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

    if (isSwinging) {
        swingTimer -= deltaTime;

        if (swingTimer <= 0.0f) {
            isSwinging = false;
            swingTimer = 0.0f;
        }
    }

    if (swordComboTimer > 0.0f) {
        swordComboTimer -= deltaTime;

        if (swordComboTimer <= 0.0f) {
            swordComboTimer = 0.0f;
            swordComboStep = 0;
        }
    }

    if (swordOnCooldown) {
        swordCooldownTimer -= deltaTime;

        if (swordCooldownTimer <= 0.0f) {
            swordCooldownTimer = 0.0f;
            swordOnCooldown = false;

            SDL_Log("Sword ready!");
        }
    }

    if (blasterFireTimer > 0.0f) {
        blasterFireTimer -= deltaTime;

        if (blasterFireTimer < 0.0f) {
            blasterFireTimer = 0.0f;
        }
    }

    if (
        playerClass == PlayerClass::Blaster &&
        blasterFireHeld &&
        blasterFireTimer <= 0.0f
    ) {
        wantsToFire = true;
        blasterFireTimer = blasterFireRate;
    }

    if (cannonFireTimer > 0.0f) {
        cannonFireTimer -= deltaTime;

        if (cannonFireTimer < 0.0f) {
            cannonFireTimer = 0.0f;
        }
    }

    if (cannonCharging) {
        cannonChargeTimer -= deltaTime;

        if (cannonChargeTimer <= 0.0f) {
            cannonChargeTimer = 0.0f;
            cannonCharging = false;

            wantsToFireCannon = true;
            cannonFireTimer = cannonFireCooldown;
        }
    }

    if (mageCooldownTimer > 0.0f) {
        mageCooldownTimer -= deltaTime;

        if (mageCooldownTimer < 0.0f) {
            mageCooldownTimer = 0.0f;
        }
    }

    if (mageCharging) {
        mageChargeTimer -= deltaTime;

        if (mageChargeTimer <= 0.0f) {
            mageChargeTimer = 0.0f;
            mageCharging = false;

            mageShotsRemaining = 3;
            mageBurstTimer = 0.0f;
        }
    }

    if (mageShotsRemaining > 0) {
        mageBurstTimer -= deltaTime;

        if (
            mageBurstTimer <= 0.0f &&
            !wantsToFireMage
        ) {
            wantsToFireMage = true;

            mageShotsRemaining--;

            mageBurstTimer = mageBurstDelay;

            if (mageShotsRemaining == 0) {
                mageCooldownTimer = mageCooldownDuration;
            }
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
            SDL_SetRenderDrawColor(
                renderer,
                255,
                0,
                0,
                255
            );
            break;

        case Direction::Down:
            SDL_SetRenderDrawColor(
                renderer,
                0,
                255,
                0,
                255
            );
            break;

        case Direction::Left:
            SDL_SetRenderDrawColor(
                renderer,
                0,
                0,
                255,
                255
            );
            break;

        case Direction::Right:
            SDL_SetRenderDrawColor(
                renderer,
                255,
                255,
                0,
                255
            );
            break;
    }

    SDL_RenderFillRect(
        renderer,
        &playerRect
    );
}

void Player::renderPunch(SDL_Renderer* renderer) {
    if (!isPunching) {
        return;
    }

    SDL_FRect punchRect = getPunchBounds();

    SDL_SetRenderDrawColor(
        renderer,
        255,
        255,
        255,
        255
    );

    SDL_RenderFillRect(
        renderer,
        &punchRect
    );
}

void Player::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        if (event.key.scancode == SDL_SCANCODE_1) {
            setPlayerClass(PlayerClass::Fighter);
            SDL_Log("Class selected: Fighter");
        }
        else if (event.key.scancode == SDL_SCANCODE_2) {
            setPlayerClass(PlayerClass::SwordUser);
            SDL_Log("Class selected: Sword User");
        }
        else if (event.key.scancode == SDL_SCANCODE_3) {
            setPlayerClass(PlayerClass::Blaster);
            SDL_Log("Class selected: Blaster");
        }
        else if (event.key.scancode == SDL_SCANCODE_4) {
            setPlayerClass(PlayerClass::Cannon);
            SDL_Log("Class selected: Cannon");
        }
        else if (event.key.scancode == SDL_SCANCODE_5) {
            setPlayerClass(PlayerClass::Mage);
            SDL_Log("Class selected: Mage");
        }

        if (
            event.key.scancode == SDL_SCANCODE_SPACE &&
            !attackKeyHeld
        ) {
            attackKeyHeld = true;

            if (
                playerClass == PlayerClass::Fighter &&
                !isPunching
            ) {
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

                SDL_Log(
                    "Combo step: %d",
                    comboStep
                );
            }
            else if (
                playerClass == PlayerClass::SwordUser &&
                !isSwinging &&
                !swordOnCooldown
            ) {
                swordComboStep++;

                if (swordComboStep > 4) {
                    swordComboStep = 1;
                }

                swordAttackId++;

                isSwinging = true;
                swingTimer = swingDuration;
                swordComboTimer = swordComboWindow;

                SDL_Log(
                    "Sword combo: %d",
                    swordComboStep
                );

                if (swordComboStep == 4) {
                    swordOnCooldown = true;
                    swordCooldownTimer =
                        swordCooldownDuration;
                }
            }
            else if (
                playerClass == PlayerClass::Cannon &&
                cannonFireTimer <= 0.0f &&
                !cannonCharging
            ) {
                cannonCharging = true;
                cannonChargeTimer =
                    cannonChargeDuration;
            }
            else if (
                playerClass == PlayerClass::Mage &&
                !mageCharging &&
                mageShotsRemaining <= 0 &&
                mageCooldownTimer <= 0.0f
            ) {
                mageCharging = true;
                mageChargeTimer =
                    mageChargeDuration;
            }
        }

        if (
            playerClass == PlayerClass::Blaster &&
            event.key.scancode ==
                SDL_SCANCODE_SPACE
        ) {
            blasterFireHeld = true;
        }
    }

    if (event.type == SDL_EVENT_KEY_UP) {
        if (
            event.key.scancode ==
            SDL_SCANCODE_SPACE
        ) {
            attackKeyHeld = false;
            blasterFireHeld = false;
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

    float centerOffset =
        (50.0f - size) / 2.0f;

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
                x + centerOffset +
                    handOffset,
                y - size,
                size,
                size
            };

        case Direction::Down:
            return {
                x + centerOffset -
                    handOffset,
                y + 50.0f,
                size,
                size
            };

        case Direction::Left:
            return {
                x - size,
                y + centerOffset -
                    handOffset,
                size,
                size
            };

        case Direction::Right:
            return {
                x + 50.0f,
                y + centerOffset +
                    handOffset,
                size,
                size
            };
    }

    return { 0, 0, 0, 0 };
}

SDL_FRect Player::getSwordBounds() const {
    float width = 70.0f;
    float depth = 35.0f;

    float swingOffset = 0.0f;

    if (swordComboStep == 1) {
        swingOffset = 10.0f;
    }
    else if (swordComboStep == 2) {
        swingOffset = -10.0f;
    }

    if (swordComboStep == 3) {
        return {
            x - 25.0f,
            y - 25.0f,
            100.0f,
            100.0f
        };
    }

    if (swordComboStep == 4) {
        float finisherWidth = 90.0f;
        float finisherDepth = 50.0f;

        switch (facing) {
            case Direction::Up:
                return {
                    x - 20.0f,
                    y - finisherDepth,
                    finisherWidth,
                    finisherDepth
                };

            case Direction::Down:
                return {
                    x - 20.0f,
                    y + 50.0f,
                    finisherWidth,
                    finisherDepth
                };

            case Direction::Left:
                return {
                    x - finisherDepth,
                    y - 20.0f,
                    finisherDepth,
                    finisherWidth
                };

            case Direction::Right:
                return {
                    x + 50.0f,
                    y - 20.0f,
                    finisherDepth,
                    finisherWidth
                };
        }
    }

    switch (facing) {
        case Direction::Up:
            return {
                x - 10.0f + swingOffset,
                y - depth,
                width,
                depth
            };

        case Direction::Down:
            return {
                x - 10.0f - swingOffset,
                y + 50.0f,
                width,
                depth
            };

        case Direction::Left:
            return {
                x - depth,
                y - 10.0f - swingOffset,
                depth,
                width
            };

        case Direction::Right:
            return {
                x + 50.0f,
                y - 10.0f + swingOffset,
                depth,
                width
            };
    }

    return { 0, 0, 0, 0 };
}

SDL_FRect Player::getBounds() const {
    return {
        x,
        y,
        50.0f,
        50.0f
    };
}

float Player::getX() const {
    return x;
}

float Player::getY() const {
    return y;
}

void Player::setPosition(
    float newX,
    float newY
) {
    x = newX;
    y = newY;
}

void Player::moveX(float amount) {
    x += amount;

    if (x < 0.0f) {
        x = 0.0f;
    }

    if (x > 750.0f) {
        x = 750.0f;
    }
}

void Player::moveY(float amount) {
    y += amount;

    if (y < 0.0f) {
        y = 0.0f;
    }

    if (y > 550.0f) {
        y = 550.0f;
    }
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

    invulnerabilityTimer =
        invulnerabilityDuration;

    SDL_Log(
        "Player health: %d",
        health
    );
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

PlayerClass Player::getPlayerClass() const {
    return playerClass;
}

void Player::setPlayerClass(
    PlayerClass newClass
) {
    playerClass = newClass;
}

bool Player::isSwordSwingActive() const {
    return isSwinging;
}

void Player::renderSword(
    SDL_Renderer* renderer
) {
    if (!isSwinging) {
        return;
    }

    SDL_FRect swordRect =
        getSwordBounds();

    SDL_SetRenderDrawColor(
        renderer,
        200,
        200,
        200,
        255
    );

    SDL_RenderFillRect(
        renderer,
        &swordRect
    );
}

int Player::getSwordAttackId() const {
    return swordAttackId;
}

int Player::getSwordComboStep() const {
    return swordComboStep;
}

bool Player::getWantsToFire() const {
    return wantsToFire;
}

void Player::clearWantsToFire() {
    wantsToFire = false;
}

bool Player::getWantsToFireCannon() const {
    return wantsToFireCannon;
}

void Player::clearWantsToFireCannon() {
    wantsToFireCannon = false;
}

void Player::renderCannonCharge(
    SDL_Renderer* renderer
) {
    if (!cannonCharging) {
        return;
    }

    float chargeProgress =
        1.0f -
        (cannonChargeTimer /
         cannonChargeDuration);

    float minSize = 10.0f;
    float maxSize = 35.0f;

    float currentSize =
        minSize +
        (maxSize - minSize) *
        chargeProgress;

    float chargeX = x;
    float chargeY = y;

    switch (facing) {
        case Direction::Up:
            chargeX =
                x +
                (50.0f - currentSize) /
                2.0f;

            chargeY =
                y - currentSize;
            break;

        case Direction::Down:
            chargeX =
                x +
                (50.0f - currentSize) /
                2.0f;

            chargeY =
                y + 50.0f;
            break;

        case Direction::Left:
            chargeX =
                x - currentSize;

            chargeY =
                y +
                (50.0f - currentSize) /
                2.0f;
            break;

        case Direction::Right:
            chargeX =
                x + 50.0f;

            chargeY =
                y +
                (50.0f - currentSize) /
                2.0f;
            break;
    }

    SDL_FRect chargeRect = {
        chargeX,
        chargeY,
        currentSize,
        currentSize
    };

    SDL_SetRenderDrawColor(
        renderer,
        0,
        255,
        255,
        255
    );

    SDL_RenderFillRect(
        renderer,
        &chargeRect
    );
}

void Player::renderMageCharge(
    SDL_Renderer* renderer
) {
    if (!mageCharging) {
        return;
    }

    float chargeProgress =
        1.0f -
        (mageChargeTimer /
         mageChargeDuration);

    float minSize = 6.0f;
    float maxSize = 18.0f;

    float currentSize =
        minSize +
        (maxSize - minSize) *
        chargeProgress;

    float chargeX = x;
    float chargeY = y;

    switch (facing) {
        case Direction::Up:
            chargeX =
                x +
                (50.0f - currentSize) /
                2.0f;

            chargeY =
                y - currentSize;
            break;

        case Direction::Down:
            chargeX =
                x +
                (50.0f - currentSize) /
                2.0f;

            chargeY =
                y + 50.0f;
            break;

        case Direction::Left:
            chargeX =
                x - currentSize;

            chargeY =
                y +
                (50.0f - currentSize) /
                2.0f;
            break;

        case Direction::Right:
            chargeX =
                x + 50.0f;

            chargeY =
                y +
                (50.0f - currentSize) /
                2.0f;
            break;
    }

    SDL_FRect chargeRect = {
        chargeX,
        chargeY,
        currentSize,
        currentSize
    };

    SDL_SetRenderDrawColor(
        renderer,
        180,
        0,
        255,
        255
    );

    SDL_RenderFillRect(
        renderer,
        &chargeRect
    );
}

bool Player::getWantsToFireMage() const {
    return wantsToFireMage;
}

void Player::clearWantsToFireMage() {
    wantsToFireMage = false;
}