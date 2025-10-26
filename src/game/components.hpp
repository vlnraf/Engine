#pragma once

#include "core/ecs.hpp"

extern ECS_DECLARE_COMPONENT(WallTag);
struct WallTag{};

extern ECS_DECLARE_COMPONENT(PortalTag);
struct PortalTag{};

extern ECS_DECLARE_COMPONENT(PortalTag2);
struct PortalTag2{};

extern ECS_DECLARE_COMPONENT(PlayerTag);
struct PlayerTag{
    float dmg = 1;
    float attackSpeed = 0.1f;
    float projectileCooldown = 0.0f;
    float radius = 5.0f;
};

extern ECS_DECLARE_COMPONENT(InputComponent);
struct InputComponent{
    bool fire = true;
    glm::vec2 direction = {0,0};
    bool pickUp = false;
};

extern ECS_DECLARE_COMPONENT(WeaponTag);
struct WeaponTag{};

extern ECS_DECLARE_COMPONENT(EnemyTag);
struct EnemyTag{
    Entity toFollow;
};