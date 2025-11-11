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

extern ECS_DECLARE_COMPONENT(GunComponent)
struct GunComponent{
    float dmg = 1;
    float attackSpeed = 0.5f;
    float range = 200;
    float radius = 5.0f;
    bool piercing = false;
    bool automatic = false;
    uint16_t numProjectiles = 1;
    float delay = 0.2;
    float delayPassed = 0;
};

extern ECS_DECLARE_COMPONENT(ShotgunComponent)
struct ShotgunComponent{
    float dmg = 1;
    float attackSpeed = 1.0f;
    float range = 50;
    float radius = 5.0f;
    bool piercing = false;
    bool automatic = false;
};

extern ECS_DECLARE_COMPONENT(SniperComponent)
struct SniperComponent{
    float dmg = 1;
    float attackSpeed = 1.5f;
    float range = 300;
    float radius = 5.0f;
    bool piercing = true;
    bool automatic = false;
};

extern ECS_DECLARE_COMPONENT(OrbitingWeaponComponent)
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

extern ECS_DECLARE_COMPONENT(OrbitingProjectile)
struct OrbitingProjectile{
    size_t slotIndex = 0;
};

extern ECS_DECLARE_COMPONENT(GranadeComponent)
struct GranadeComponent{
    float dmg = 5;
    float attackSpeed = 1.0f;
    float range = 300;
    bool automatic = true;
};

extern ECS_DECLARE_COMPONENT(ExplosionComponent)
struct ExplosionComponent{
    glm::vec3 targetPosition;
};

extern ECS_DECLARE_COMPONENT(HasWeaponComponent)
struct HasWeaponComponent{
    Entity weaponId[10]; //max 10 different weapons right now
    WeaponType weaponType[10];
    uint8_t weaponCount;
};

extern ECS_DECLARE_COMPONENT(CooldownComponent)
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
void weaponFireSystem(Ecs* ecs, float dt);
void explosionSystem(Ecs* ecs);