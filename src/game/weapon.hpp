#pragma once
#include "core.hpp"

enum WeaponType{
    GUN,
    SHOTGUN,
    SNIPER
};

struct GunComponent{
    float dmg = 1;
    float attackSpeed = 0.2f;
    float radius = 5.0f;
    bool piercing = false;
};

struct ShotgunComponent{
    float dmg = 1;
    float attackSpeed = 0.4f;
    float radius = 5.0f;
    bool piercing = false;
};

struct SniperComponent{
    float dmg = 1;
    float attackSpeed = 0.6f;
    float radius = 5.0f;
    bool piercing = true;
};

struct HasWeaponComponent{
    Entity weaponId;
    WeaponType weaponType;
};


struct CooldownComponent{
    float timeRemaining = 0;
};

Entity createGun(Ecs* ecs);
Entity createShotgun(Ecs* ecs);
Entity createSniper(Ecs* ecs);
void fireGun(Ecs* ecs, Entity weaponId, const glm::vec3 spawnPosition, const glm::vec2 direction);
void cooldownSystem(Ecs* ecs, float dt);
void weaponFireSystem(Ecs* ecs);