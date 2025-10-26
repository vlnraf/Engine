#pragma once

#include "core.hpp"

extern ECS_DECLARE_COMPONENT(BossTag);
struct BossTag{};

//Entity createBoss(Ecs* ecs, EngineState* engine, OrtographicCamera camera);
Entity createBoss(Ecs* ecs, OrtographicCamera camera);
void bossAiSystem(Ecs* ecs, float dt);
void bossActiveHurtBoxSystem(Ecs* ecs);
void changeBossTextureSystem(Ecs* ecs);
void systemRespondBossHitStaticEntity(Ecs* ecs);