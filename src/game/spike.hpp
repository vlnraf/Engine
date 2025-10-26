#pragma once

#include "core.hpp"

extern ECS_DECLARE_COMPONENT(SpikeTag);
struct SpikeTag{};

Entity createSpike(Ecs* ecs, glm::vec3 pos);
void systemSpikeHit(Ecs* ecs);