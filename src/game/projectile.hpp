#pragma once

#include "core.hpp"

struct ProjectileTag{
    glm::vec3 initialPos;
    int dmg = 1;
    float attckSpeed = 1.0f;
    float range = 30;
    float radius = 1;
    bool piercing = false;
};

Entity createProjectile(Ecs* ecs, glm::vec3 pos, glm::vec2 dir, int dmg, float radius, bool piercing);
void destroyProjectile(Ecs* ecs, Entity entity);
void systemProjectileHit(Ecs* ecs);
void systemCheckRange(Ecs* ecs);