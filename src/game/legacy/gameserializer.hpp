#pragma once

//#include "game/game.hpp"
#include "game/projectx.hpp"
#include "core.hpp"


void serializeGame(GameState* gameState, const char* name);

void deserializeGame(EngineState* engine, GameState* gameState, const char* filePath);