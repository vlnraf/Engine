#pragma once
#include "core/ecs.hpp"

struct WallTag{};

struct GamepadSpriteTag{};

struct PortalTag{};

struct PortalTag2{};

struct PlayerTag{};

struct WeaponTag{};

struct EnemyTag{
    Entity toFollow;
};