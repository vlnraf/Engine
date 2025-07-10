#pragma once
#include "core/ecs.hpp"

struct WallTag{};

struct GamepadSpriteTag{};

struct PortalTag{};

struct PortalTag2{};

struct PlayerTag{
    int dmg = 1;
    float attackSpeed = 0.1f;
    float projectileCooldown = 0.0f;
    float radius = 5.0f;
};

struct InputComponent{
    bool fire = true;
    glm::vec2 direction = {0,0};
    bool pickUp = false;
};

struct WeaponTag{};

struct EnemyTag{
    Entity toFollow;
};