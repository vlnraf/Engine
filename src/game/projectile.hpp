#pragma once

#include "core.hpp"

struct ProjectileTag{
    glm::vec3 initialPos;
    float range;
};

Entity createProjectile(Ecs* ecs, EngineState* engine, glm::vec3 pos, glm::vec2 dir);
void destroyProjectile(Ecs* ecs, Entity entity);
void systemProjectileHit(Ecs* ecs, const float dt);
void systemCheckRange(Ecs* ecs, const float dt);
void setProjectileDmg(int newDmg);