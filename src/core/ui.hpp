#pragma once

#include <glm/glm.hpp>

#include "coreapi.hpp"
#include "core.hpp"

struct UIState{
};

static UIState* uiState;

CORE_API void button(const char* name, const glm::vec2 pos, const glm::vec2 size);