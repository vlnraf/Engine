#pragma once

#include "core.hpp"

struct ProjectileTag{};

Entity createProjectile(Ecs* ecs, EngineState* engine, glm::vec3 pos, glm::vec2 dir);
void destroyProjectile(Ecs* ecs, Entity entity);