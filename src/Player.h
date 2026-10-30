#pragma once

#include <SDL3/SDL.h>

enum class Direction {
    Up,
    Down,
    Left,
    Right
};

enum class PlayerClass {
    None,
    Fighter,
    SwordUser,
    Blaster,
    Cannon,
    Mage
};

class Player {
public:
    Player();

    void update(float deltaTime);
    void handleEvent(const SDL_Event& event);
    void render(SDL_Renderer* renderer) const;

    SDL_FRect getBounds() const;
    float getX() const;
    float getY() const;
    void setPosition(float newX, float newY);
    void moveX(float amount);
    void moveY(float amount);
    float getSpeed() const;
    Direction getFacing() const;
    void setFacing(Direction newFacing);

    void takeDamage(int amount);
    bool isAlive() const;
    int getHealth() const;

    PlayerClass getPlayerClass() const;
    void setPlayerClass(PlayerClass newClass);

    bool isPunchActive() const;
    SDL_FRect getPunchBounds() const;
    void renderPunch(SDL_Renderer* renderer) const;
    bool hasPunchHit() const;
    void markPunchHit();
    int getComboStep() const;

    bool isSwordSwingActive() const;
    SDL_FRect getSwordBounds() const;
    void renderSword(SDL_Renderer* renderer) const;
    int getSwordAttackId() const;
    int getSwordComboStep() const;

    bool getWantsToFire() const;
    void clearWantsToFire();

    bool getWantsToFireCannon() const;
    void clearWantsToFireCannon();
    void renderCannonCharge(SDL_Renderer* renderer) const;

    bool getWantsToFireMage() const;
    void clearWantsToFireMage();
    void renderMageCharge(SDL_Renderer* renderer) const;
    int getMageCurrentShotNumber() const;
    int getMageStatusShot() const;

private:
    static constexpr float kArenaWidth = 800.0f;
    static constexpr float kArenaHeight = 600.0f;
    static constexpr float kPlayerSize = 50.0f;
    static constexpr float kPunchFinisherDuration = 0.10f;
    static constexpr float kPunchDuration = 0.15f;

    float x;
    float y;
    float speed;
    Direction facing;
    int health;
    float invulnerabilityTimer;
    const float invulnerabilityDuration = 1.0f;
    bool attackKeyHeld;
    PlayerClass playerClass;

    bool punchHasHit;
    bool isPunching;
    float punchTimer;
    int comboStep;
    float comboTimer;
    const float comboWindow = 0.5f;

    bool isSwinging;
    float swingTimer;
    const float swingDuration = 0.30f;
    int swordComboStep;
    float swordComboTimer;
    const float swordComboWindow = 0.7f;
    bool swordOnCooldown;
    float swordCooldownTimer;
    const float swordCooldownDuration = 0.5f;
    int swordAttackId;

    float blasterFireTimer;
    const float blasterFireRate = 0.15f;
    bool wantsToFire;
    bool blasterFireHeld;

    bool wantsToFireCannon;
    float cannonFireTimer;
    const float cannonFireCooldown = 1.0f;
    bool cannonCharging;
    float cannonChargeTimer;
    const float cannonChargeDuration = 0.5f;

    bool wantsToFireMage;
    bool mageCharging;
    float mageChargeTimer;
    const float mageChargeDuration = 0.25f;
    int mageShotsRemaining;
    float mageBurstTimer;
    const float mageBurstDelay = 0.10f;
    float mageCooldownTimer;
    const float mageCooldownDuration = 0.5f;
    int mageCurrentShotNumber;
    int mageStatusShot;

    void updateFighter(float deltaTime);
    void updateSword(float deltaTime);
    void updateBlaster(float deltaTime);
    void updateCannon(float deltaTime);
    void updateMage(float deltaTime);

    void handleClassSelection(SDL_Scancode key);
    void handleAttackInput();
    void startFighterAttack();
    void startSwordAttack();
    void startCannonAttack();
    void startMageAttack();

    void renderChargeEffect(
        SDL_Renderer* renderer,
        float progress,
        float minSize,
        float maxSize,
        Uint8 red,
        Uint8 green,
        Uint8 blue
    ) const;
};