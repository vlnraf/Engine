#pragma once
#include "core.hpp"

struct TelegraphAttack{
    float elapsedTime;
    float duration;
};

Entity createTelegraphAttack(Ecs* ecs, glm::vec3 pos);