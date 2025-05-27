#pragma once

#include <glm/glm.hpp>

#include "core/coreapi.hpp"

struct UIState{
    glm::vec2 mousePos;
    glm::vec2 screenSize;
    glm::vec2 canvasSize;

    uint32_t active;
    uint32_t hot;
    uint32_t id = 1;
};

void initUI(glm::vec2 screenSize);
void destroyUI();

CORE_API void beginUIFrame(glm::vec2 canvasPos, glm::vec2 canvasSize);
CORE_API void endUIFrame();
CORE_API bool pointRectIntersection(glm::vec2 mousePos, glm::vec2 pos, glm::vec2 size);
CORE_API bool UIButton(const char* fmt, glm::vec2 pos, glm::vec2 size, glm::vec3 rotation);