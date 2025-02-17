#pragma once 
#include "core.hpp"

struct LifeTime{
    float time = 0;
    float endTime;
};

void lifeTimeSystem(Ecs* ecs, float dt);