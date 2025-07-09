#pragma once

#include "core.hpp"

struct ProjectileTag{
    glm::vec3 initialPos;
    int dmg = 1;
    float attckSpeed = 1.0f;
    float range = 30;
    float radius = 1;
};

Entity createProjectile(Ecs* ecs, glm::vec3 pos, glm::vec2 dir, int dmg, float radius);
void destroyProjectile(Ecs* ecs, Entity entity);
void systemProjectileHit(Ecs* ecs);
void systemCheckRange(Ecs* ecs);