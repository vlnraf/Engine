#pragma once 
#include "core.hpp"

extern ECS_DECLARE_COMPONENT(LifeTime)
struct LifeTime{
    float time = 0;
    float endTime;
};

void lifeTimeSystem(Ecs* ecs, float dt);