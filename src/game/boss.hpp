#pragma once

#include "core.hpp"

struct BossTag{};

Entity createBoss(Ecs* ecs, EngineState* engine, OrtographicCamera camera);
void bossAiSystem(Ecs* ecs, float dt);
void bossActiveHurtBoxSystem(Ecs* ecs);