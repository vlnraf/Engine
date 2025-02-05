#pragma once

#include "core.hpp"

Entity createPlayer(Ecs* ecs, EngineState* engine);
void inputPlayerSystem(Ecs* ecs, Input* input);