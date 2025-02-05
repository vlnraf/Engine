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


struct UIVertex{
    glm::vec3 pos;
    glm::vec4 color;
};

struct QuadVertex{
    glm::vec3 pos;
    glm::vec4 color;
    glm::vec2 texCoord;
};

struct SimpleVertex{
    glm::vec3 pos;
    glm::vec4 color;
};

struct LineVertex{
    glm::vec3 pos;
    glm::vec4 color;
};

struct Renderer{
    uint32_t vao, vbo, ebo;
    uint32_t lineVao, lineVbo, lineEbo;
    uint32_t textVao, textVbo, textEbo;
    uint32_t simpleVao, simpleVbo, simpleEbo;
    uint32_t uiVao, uiVbo, uiEbo;
    Shader shader;
    Shader lineShader;
    Shader textShader;
    Shader simpleShader;
    Shader uiShader;

    uint32_t width, height;
};

Renderer* initRenderer(const uint32_t width, const uint32_t height);
void destroyRenderer(Renderer* renderer);
//void setYsort(Renderer* renderer, bool flag);
void genVertexArrayObject(uint32_t* vao);
void genVertexBuffer(uint32_t* vbo);
void bindVertexArrayObject(uint32_t vao);
void bindVertexArrayBuffer(uint32_t vbo, const float* vertices, size_t vertCount);
void bindVertexArrayBuffer(uint32_t vbo, const QuadVertex* vertices, size_t vertCount);
void bindVertexArrayBuffer(uint32_t vbo, const LineVertex* vertices, size_t vertCount);
void bindVertexArrayBuffer(uint32_t vbo, const UIVertex* vertices, size_t vertCount);
void setShader(Renderer* renderer, const Shader shader);
void commandDrawQuad(const uint32_t textureId, const QuadVertex* vertices, const size_t vertCount);
void commandDrawLine(const LineVertex* vertices, const size_t vertCount);
void commandDrawQuad(const SimpleVertex* vertices, const size_t vertCount);
void commandDrawQuad(const UIVertex* vertices, const size_t vertCount);

glm::vec4 calculateUV(const Texture* texture, glm::vec2 index, glm::vec2 size, glm::vec2 offset);


//------------------HIGH LEVEL RENDERER-----------------------------
//void renderDrawQuad(Renderer* renderer, OrtographicCamera camera, glm::vec3 position, const glm::vec3 scale, const glm::vec3 rotation, const Texture* texture);
CORE_API void clearColor(float r, float g, float b, float a);
CORE_API void renderDrawQuad(Renderer* renderer, OrtographicCamera camera, glm::vec3 position, const glm::vec3 scale, const glm::vec3 rotation, const Texture* texture, glm::vec2 index, glm::vec2 spriteSize, bool ySort);
CORE_API void renderDrawSprite(Renderer* renderer, OrtographicCamera camera, glm::vec3 position, const glm::vec3 scale, const glm::vec3 rotation, const SpriteComponent* sprite);
CORE_API void renderDrawLine(Renderer* renderer, OrtographicCamera camera, const glm::vec2 p0, const glm::vec2 p1, const glm::vec4 color, const float layer);
CORE_API void renderDrawRect(Renderer* renderer, OrtographicCamera camera, const glm::vec2 offset, const glm::vec2 size, const glm::vec4 color, const float layer);
CORE_API void renderDrawFilledRect(Renderer* renderer, OrtographicCamera camera, const glm::vec3 position, const glm::vec2 size, const glm::vec3 rotation, const glm::vec4 color);
//void renderDrawFilledRect(Renderer* renderer, const glm::vec2 position, const glm::vec2 size);


//TODO: refactor this shit!!!
CORE_API void renderDrawText(Renderer* renderer, Font* font, OrtographicCamera camera, const char* text, float x, float y, float scale);