#pragma once

#include "core.hpp"

Entity createPlayer(Ecs* ecs, OrtographicCamera camera);
void inputPlayerSystem(Ecs* ecs, Input* input, float dt);