#pragma once

#include "core.hpp"

struct BossTag{};

//Entity createBoss(Ecs* ecs, EngineState* engine, OrtographicCamera camera);
Entity createBoss(Ecs* ecs, EngineState* engine, OrtographicCamera camera, glm::vec3 pos);
void bossAiSystem(Ecs* ecs, EngineState* engine, float dt);
void bossActiveHurtBoxSystem(Ecs* ecs);
void systemRespondBossHitStaticEntity(Ecs* ecs, const float dt);