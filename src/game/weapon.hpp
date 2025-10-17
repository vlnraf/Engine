#pragma once
#include "core.hpp"

enum WeaponType{
    WEAPON_NONE,
    WEAPON_GUN,
    WEAPON_SHOTGUN,
    WEAPON_SNIPER,
    WEAPON_ORBIT,
    WEAPON_GRANADE
};

struct GunComponent{
    float dmg = 1;
    float attackSpeed = 0.5f;
    float range = 200;
    float radius = 5.0f;
    bool piercing = false;
    bool automatic = false;
};

struct ShotgunComponent{
    float dmg = 1;
    float attackSpeed = 1.0f;
    float range = 50;
    float radius = 5.0f;
    bool piercing = false;
    bool automatic = false;
};

struct SniperComponent{
    float dmg = 1;
    float attackSpeed = 1.5f;
    float range = 300;
    float radius = 5.0f;
    bool piercing = true;
    bool automatic = false;
};

struct OrbitingWeaponComponent{
    Entity target; //entity to move around
    //glm::vec3 center;
    float dmg = 1;
    float range = 100;
    float angle = 0;
    bool piercing = true;
    bool automatic = false;
    size_t slotCount = 0;
};

struct OrbitingProjectile{
    size_t slotIndex = 0;
};

struct GranadeComponent{
    float dmg = 5;
    float attackSpeed = 1.0f;
    float range = 300;
    bool automatic = true;
};

struct ExplosionComponent{
    glm::vec3 targetPosition;
};

struct HasWeaponComponent{
    Entity weaponId[10]; //max 10 different weapons right now
    WeaponType weaponType[10];
    uint8_t weaponCount;
};


struct CooldownComponent{
    float timeRemaining = 0;
};

Entity createGun(Ecs* ecs);
Entity createShotgun(Ecs* ecs);
Entity createSniper(Ecs* ecs);
Entity createGranade(Ecs* ecs);
Entity createOrbitWeapon(Ecs* ecs);
void addOrbitProjectile(Ecs* ecs, Entity weaponId);
void fireGun(Ecs* ecs, Entity weaponId, const glm::vec3 spawnPosition, const glm::vec2 direction);
void cooldownSystem(Ecs* ecs, float dt);
void weaponFireSystem(Ecs* ecs);
void explosionSystem(Ecs* ecs);