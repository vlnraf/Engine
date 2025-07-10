#pragma once
#include "core.hpp"

struct GunComponent{
    int dmg = 1;
    float attackSpeed = 0.2f;
    float radius = 5.0f;
    bool piercing = false;
};

struct ShotgunComponent{
    int dmg = 1;
    float attackSpeed = 0.4f;
    float radius = 5.0f;
    bool piercing = false;
};

struct SniperComponent{
    int dmg = 1;
    float attackSpeed = 0.6f;
    float radius = 5.0f;
    bool piercing = true;
};

struct HasWeaponComponent{
    Entity weaponId;
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