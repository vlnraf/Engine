#pragma once
#include "core.hpp"

struct TelegraphAttack{
    float elapsedTime;
    float duration;
};

Entity createTelegraphAttack(Ecs* ecs, EngineState* engine, OrtographicCamera camera, glm::vec3 pos);