#pragma once

#include "game/game.hpp"
#include "core.hpp"

void serializeGame(GameState* gameState, const char* name);

void deserializeGame(GameState* gameState, const char* filePath);