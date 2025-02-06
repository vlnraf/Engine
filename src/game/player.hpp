#pragma once

#include "core.hpp"

Entity createPlayer(Ecs* ecs, EngineState* engine);
void inputPlayerSystem(Ecs* ecs, EngineState* engine, Input* input);