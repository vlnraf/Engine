#pragma once

#include "core.hpp"
#include "components.hpp"
#include "projectx.hpp"

struct ExperienceComponent{
    float currentXp = 0.0f;
    float xpDrop = 1.0f;
    float maxXp = 100.0f;
    int currentLevel = 1.0f;
};

void spawnEnemy(Ecs* ecs, const TransformComponent* playerTransform);
void systemSpawnEnemies(Ecs* ecs, float dt);
void systemUpdateEnemyDirection(Ecs* ecs);
void systemEnemyHitPlayer(Ecs* ecs);
void deathEnemySystem(Ecs* ecs);
void gatherExperienceSystem(Ecs* ecs, GameState* gameState);
void renderPowerUpCards();