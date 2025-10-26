#pragma once

#include "core.hpp"
extern ECS_DECLARE_COMPONENT(ProjectileTag)
struct ProjectileTag{
    glm::vec3 initialPos;
    float range = 30;
    bool piercing = false;
};

Entity createProjectile(Ecs* ecs, glm::vec3 pos, glm::vec2 dir, float dmg, float range, float radius, bool piercing);
void destroyProjectile(Ecs* ecs, Entity entity);
void systemProjectileHit(Ecs* ecs);
void systemCheckRange(Ecs* ecs);