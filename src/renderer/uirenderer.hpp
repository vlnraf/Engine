#pragma once


#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "core/coreapi.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "fontmanager.hpp"
#include "core/camera.hpp"
#include "core/ecs.hpp"
#include "renderer/renderer.hpp"


struct UIRenderer{
    Font* uiFont;
    uint32_t uiVao, uiVbo, uiEbo;
    uint32_t textVao, textVbo, textEbo;
    Shader uiShader;
    Shader uiTextShader;

    std::vector<UIVertex> vertices;
    std::vector<QuadVertex> textVertices;

    uint32_t width, height;
};

extern UIRenderer* uiRenderer;

extern "C"{
    CORE_API void initUIRenderer(const uint32_t width, const uint32_t height);
    CORE_API void destroyUIRenderer();

    CORE_API void renderUIElements();
    CORE_API void pushButton(const glm::vec2, const glm::vec2 size);
    CORE_API void pushText(const char* text, glm::vec2 pos, const float scale);

}