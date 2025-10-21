#pragma once

#include "core.hpp"

struct SpikeTag{
    //float lifeTime;
    //float time = 0;
};

struct TEST{
    int x = 0;
    int y = 10;
};

Entity createSpike(Ecs* ecs, glm::vec3 pos);
void systemSpikeHit(Ecs* ecs);