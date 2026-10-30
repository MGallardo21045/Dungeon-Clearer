#include "Player.h"
#include <random>

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
      mageCooldownTimer(0.0f),
      mageCurrentShotNumber(0),
      mageStatusShot(0)
{
}

void Player::update(float deltaTime) {
    updateFighter(deltaTime);
    updateSword(deltaTime);
    updateBlaster(deltaTime);
    updateCannon(deltaTime);
    updateMage(deltaTime);

    if (invulnerabilityTimer > 0.0f) {
        invulnerabilityTimer -= deltaTime;

        if (invulnerabilityTimer < 0.0f) {
            invulnerabilityTimer = 0.0f;
        }
    }
}

void Player::render(SDL_Renderer* renderer) const {
    SDL_FRect playerRect = {
        x,
        y,
        kPlayerSize,
        kPlayerSize
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

void Player::renderPunch(SDL_Renderer* renderer) const {
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
    if (event.type == SDL_EVENT_KEY_UP &&
        event.key.scancode == SDL_SCANCODE_SPACE) {
        attackKeyHeld = false;
        blasterFireHeld = false;
        return;
    }

    if (event.type != SDL_EVENT_KEY_DOWN) {
        return;
    }

    // Ignore key-repeat events: a held key should not start a new attack.
    if (event.key.repeat) {
        return;
    }

    handleClassSelection(event.key.scancode);

    if (event.key.scancode != SDL_SCANCODE_SPACE || attackKeyHeld) {
        return;
    }

    attackKeyHeld = true;
    if (playerClass == PlayerClass::Blaster) {
        blasterFireHeld = true;
    } else {
        handleAttackInput();
    }
}

void Player::handleClassSelection(SDL_Scancode key) {
    switch (key) {
        case SDL_SCANCODE_1:
            setPlayerClass(PlayerClass::Fighter);
            SDL_Log("Class selected: Fighter");
            break;
        case SDL_SCANCODE_2:
            setPlayerClass(PlayerClass::SwordUser);
            SDL_Log("Class selected: Sword User");
            break;
        case SDL_SCANCODE_3:
            setPlayerClass(PlayerClass::Blaster);
            SDL_Log("Class selected: Blaster");
            break;
        case SDL_SCANCODE_4:
            setPlayerClass(PlayerClass::Cannon);
            SDL_Log("Class selected: Cannon");
            break;
        case SDL_SCANCODE_5:
            setPlayerClass(PlayerClass::Mage);
            SDL_Log("Class selected: Mage");
            break;
        default:
            break;
    }
}

void Player::handleAttackInput() {
    switch (playerClass) {
        case PlayerClass::Fighter:   startFighterAttack(); break;
        case PlayerClass::SwordUser: startSwordAttack(); break;
        case PlayerClass::Cannon:    startCannonAttack(); break;
        case PlayerClass::Mage:      startMageAttack(); break;
        case PlayerClass::Blaster:
        case PlayerClass::None:
            break;
    }
}

void Player::startFighterAttack() {
    if (isPunching) return;

    ++comboStep;
    if (comboStep > 3) comboStep = 1;

    isPunching = true;
    punchHasHit = false;
    punchTimer = (comboStep == 3) ? kPunchFinisherDuration : kPunchDuration;
    comboTimer = comboWindow;
    SDL_Log("Combo step: %d", comboStep);
}

void Player::startSwordAttack() {
    if (isSwinging || swordOnCooldown) return;

    ++swordComboStep;
    if (swordComboStep > 4) swordComboStep = 1;

    ++swordAttackId;
    isSwinging = true;
    swingTimer = swingDuration;
    swordComboTimer = swordComboWindow;
    SDL_Log("Sword combo: %d", swordComboStep);

    if (swordComboStep == 4) {
        swordOnCooldown = true;
        swordCooldownTimer = swordCooldownDuration;
    }
}

void Player::startCannonAttack() {
    if (cannonFireTimer > 0.0f || cannonCharging) return;
    cannonCharging = true;
    cannonChargeTimer = cannonChargeDuration;
}

void Player::startMageAttack() {
    if (mageCharging || mageShotsRemaining > 0 || mageCooldownTimer > 0.0f) {
        return;
    }
    mageCharging = true;
    mageChargeTimer = mageChargeDuration;
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
        (kPlayerSize - size) / 2.0f;

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
                y + kPlayerSize,
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
                x + kPlayerSize,
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
                    y + kPlayerSize,
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
                    x + kPlayerSize,
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
                y + kPlayerSize,
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
                x + kPlayerSize,
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
        kPlayerSize,
        kPlayerSize
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

    if (x > kArenaWidth - kPlayerSize) {
        x = kArenaWidth - kPlayerSize;
    }
}

void Player::moveY(float amount) {
    y += amount;

    if (y < 0.0f) {
        y = 0.0f;
    }

    if (y > kArenaHeight - kPlayerSize) {
        y = kArenaHeight - kPlayerSize;
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


void Player::setPlayerClass(PlayerClass newClass) {
    if (playerClass == newClass) {
        return;
    }

    isPunching = false;
    punchTimer = 0.0f;
    punchHasHit = false;
    comboStep = 0;
    comboTimer = 0.0f;

    isSwinging = false;
    swingTimer = 0.0f;
    swordComboStep = 0;
    swordComboTimer = 0.0f;
    swordOnCooldown = false;
    swordCooldownTimer = 0.0f;

    wantsToFire = false;
    blasterFireHeld = false;
    blasterFireTimer = 0.0f;

    wantsToFireCannon = false;
    cannonCharging = false;
    cannonChargeTimer = 0.0f;
    cannonFireTimer = 0.0f;

    wantsToFireMage = false;
    mageCharging = false;
    mageChargeTimer = 0.0f;
    mageShotsRemaining = 0;
    mageBurstTimer = 0.0f;
    mageCooldownTimer = 0.0f;
    mageCurrentShotNumber = 0;
    mageStatusShot = 0;

    playerClass = newClass;
}


bool Player::isSwordSwingActive() const {
    return isSwinging;
}

void Player::renderSword(SDL_Renderer* renderer) const {
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

void Player::renderCannonCharge(SDL_Renderer* renderer) const {
    if (!cannonCharging) {
        return;
    }

    float progress = 1.0f -
        (cannonChargeTimer / cannonChargeDuration);

    renderChargeEffect(
        renderer,
        progress,
        10.0f,
        35.0f,
        0, 255, 255
    );
}

void Player::renderMageCharge(SDL_Renderer* renderer) const {
    if (!mageCharging) {
        return;
    }

    float progress = 1.0f -
        (mageChargeTimer / mageChargeDuration);

    renderChargeEffect(
        renderer,
        progress,
        6.0f,
        18.0f,
        180, 0, 255
    );
}

bool Player::getWantsToFireMage() const {
    return wantsToFireMage;
}

void Player::clearWantsToFireMage() {
    wantsToFireMage = false;
}

int Player::getMageCurrentShotNumber() const {
    return mageCurrentShotNumber;
}

int Player::getMageStatusShot() const {
    return mageStatusShot;
}


void Player::renderChargeEffect(
    SDL_Renderer* renderer,
    float progress,
    float minSize,
    float maxSize,
    Uint8 red,
    Uint8 green,
    Uint8 blue
) const {
    float currentSize =
        minSize + (maxSize - minSize) * progress;

    float chargeX = x;
    float chargeY = y;

    switch (facing) {
        case Direction::Up:
            chargeX = x + (kPlayerSize - currentSize) / 2.0f;
            chargeY = y - currentSize;
            break;

        case Direction::Down:
            chargeX = x + (kPlayerSize - currentSize) / 2.0f;
            chargeY = y + kPlayerSize;
            break;

        case Direction::Left:
            chargeX = x - currentSize;
            chargeY = y + (kPlayerSize - currentSize) / 2.0f;
            break;

        case Direction::Right:
            chargeX = x + kPlayerSize;
            chargeY = y + (kPlayerSize - currentSize) / 2.0f;
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
        red,
        green,
        blue,
        255
    );

    SDL_RenderFillRect(renderer, &chargeRect);
}

void Player::updateFighter(float deltaTime) {
    if (isPunching) {
        punchTimer -= deltaTime;

        if (punchTimer <= 0.0f) {
            isPunching = false;
            punchTimer = 0.0f;
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

void Player::updateSword(float deltaTime) {
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
}

void Player::updateBlaster(float deltaTime) {
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
}

void Player::updateCannon(float deltaTime) {
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
}


void Player::updateMage(float deltaTime) {
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

            mageCurrentShotNumber = 0;
            mageStatusShot = 0;

            static std::random_device randomDevice;
            static std::mt19937 randomGenerator(
                randomDevice()
            );

            static std::uniform_int_distribution<int>
                statusChanceRoll(1, 100);

            int roll = statusChanceRoll(randomGenerator);

            if (roll <= 10) {
                static std::uniform_int_distribution<int>
                    statusShotRoll(1, 3);

                mageStatusShot = statusShotRoll(randomGenerator);

                SDL_Log(
                    "Mage status burst! Special shot: %d",
                    mageStatusShot
                );
            }
        }
    }

    if (mageShotsRemaining > 0) {
        mageBurstTimer -= deltaTime;

        if (
            mageBurstTimer <= 0.0f &&
            !wantsToFireMage
        ) {
            mageCurrentShotNumber = 4 - mageShotsRemaining;

            wantsToFireMage = true;
            mageShotsRemaining--;

            mageBurstTimer = mageBurstDelay;

            if (mageShotsRemaining == 0) {
                mageCooldownTimer = mageCooldownDuration;
            }
        }
    }
}