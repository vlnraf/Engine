#pragma once

#include "core.hpp"

struct SpikeTag{
    float lifeTime;
    float time = 0;
};

Entity createSpike(Ecs* ecs, EngineState* engine, glm::vec3 pos);
void systemSpikeHit(Ecs* ecs, const float dt);