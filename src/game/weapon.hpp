#pragma once
#include "core.hpp"

struct GunComponent{
    int dmg = 1;
    float attackSpeed = 0.2f;
    float radius = 5.0f;
};

struct ShotgunComponent{
    int dmg = 1;
    float attackSpeed = 0.2f;
    float radius = 5.0f;
};

struct HasWeaponComponent{
    Entity weaponId;
};


struct CooldownComponent{
    float timeRemaining = 0;
};

Entity createGun(Ecs* ecs);
Entity createShotgun(Ecs* ecs);
void fireGun(Ecs* ecs, Entity weaponId, const glm::vec3 spawnPosition, const glm::vec2 direction);
void cooldownSystem(Ecs* ecs, float dt);
void weaponFireSystem(Ecs* ecs);