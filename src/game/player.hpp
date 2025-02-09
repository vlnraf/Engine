#pragma once

#include "core.hpp"

Entity createPlayer(Ecs* ecs, EngineState* engine, OrtographicCamera camera);
void inputPlayerSystem(Ecs* ecs, EngineState* engine, Input* input);