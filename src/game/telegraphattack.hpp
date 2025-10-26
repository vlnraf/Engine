#pragma once
#include "core.hpp"

extern ECS_DECLARE_COMPONENT(TelegraphAttack)
struct TelegraphAttack{
    float elapsedTime;
    float duration;
};

Entity createTelegraphAttack(Ecs* ecs, glm::vec3 pos);