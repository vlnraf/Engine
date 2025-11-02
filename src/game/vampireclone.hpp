#pragma once

#include "core.hpp"
#include "projectx.hpp"

extern ECS_DECLARE_COMPONENT(ExperienceComponent)
struct ExperienceComponent{
    float currentXp = 0.0f;
    float maxXp = 100.0f;
    int currentLevel = 1.0f;
};

extern ECS_DECLARE_COMPONENT(ExperienceDrop)
struct ExperienceDrop{
    float xpDrop = 1.0f;
};


void spawnEnemy(Ecs* ecs, const TransformComponent* playerTransform);
void systemSpawnEnemies(Ecs* ecs, float dt);
void systemUpdateEnemyDirection(Ecs* ecs);
void systemEnemyHitPlayer(Ecs* ecs);
void spawnExperience(Ecs* ecs, glm::vec3 position);
void gatherExperienceSystem(Ecs* ecs, GameState* gameState);
void levelUp(GameState* gameState, ExperienceComponent* playerXp);
void renderPowerUpCards();