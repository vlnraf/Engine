#pragma once

#include <glm/glm.hpp>

#include "core/coreapi.hpp"
#include "renderer/fontmanager.hpp"
#include "renderer/texture.hpp"

struct UIState{
    glm::vec2 mousePos;
    glm::vec2 screenSize;
    glm::vec2 canvasSize;
    Font* font;

    uint32_t active;
    uint32_t hot;
    uint32_t id = 1;
};

UIState* initUI(glm::vec2 screenSize);
void destroyUI();

CORE_API void beginUiFrame(glm::vec2 canvasPos, glm::vec2 canvasSize);
CORE_API void endUiFrame();
CORE_API bool pointRectIntersection(glm::vec2 mousePos, glm::vec2 pos, glm::vec2 size);
CORE_API bool UiButton(const char* text, glm::vec2 pos, glm::vec2 size, glm::vec2 rotation);
CORE_API void UiText(const char* text, glm::vec2 pos, float scale);
CORE_API void UiImage(Texture* texture, glm::vec2 pos, glm::vec2 rotation);
CORE_API void UiImage(Texture* texture, glm::vec2 pos, glm::vec2 size, glm::vec2 rotation, glm::vec2 index, glm::vec2 offset);
CORE_API int UigetTextHeight(const char* text, float scale);