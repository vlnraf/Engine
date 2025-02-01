#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "shader.hpp"
#include "texture.hpp"
#include "fontmanager.hpp"
#include "core/camera.hpp"
#include "core/ecs.hpp"

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
    Shader shader;
    Shader lineShader;
    Shader textShader;
    Shader simpleShader;

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
void setShader(Renderer* renderer, const Shader shader);
void commandDrawQuad(Renderer* renderer, const uint32_t textureId, const QuadVertex* vertices, const size_t vertCount);
void commandDrawLine(Renderer* renderer, const LineVertex* vertices, const size_t vertCount);
void clearRenderer(float r, float g, float b, float a);



//------------------HIGH LEVEL RENDERER-----------------------------
//void renderDrawQuad(Renderer* renderer, OrtographicCamera camera, glm::vec3 position, const glm::vec3 scale, const glm::vec3 rotation, const Texture* texture);
void renderDrawQuad(Renderer* renderer, OrtographicCamera camera, glm::vec3 position, const glm::vec3 scale, const glm::vec3 rotation, const Texture* texture, glm::vec2 index, glm::vec2 spriteSize, bool ySort);
void renderDrawSprite(Renderer* renderer, OrtographicCamera camera, glm::vec3 position, const glm::vec3 scale, const glm::vec3 rotation, const SpriteComponent* sprite);
void renderDrawLine(Renderer* renderer, OrtographicCamera camera, const glm::vec2 p0, const glm::vec2 p1, const glm::vec4 color, const float layer);
void renderDrawRect(Renderer* renderer, OrtographicCamera camera, const glm::vec2 offset, const glm::vec2 size, const glm::vec4 color, const float layer);
void renderDrawFilledRect(Renderer* renderer, OrtographicCamera camera, glm::vec3 position, const glm::vec2 size, const glm::vec3 rotation, const glm::vec4 color);


//TODO: refactor this shit!!!
void renderDrawText(Renderer* renderer, Font* font, OrtographicCamera camera, const char* text, float x, float y, float scale);