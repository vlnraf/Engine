#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "shader.hpp"
#include "texture.hpp"
#include "core/camera.hpp"

struct Renderer{
    uint32_t vao, vbo, ebo;
    uint32_t lineVao, lineVbo, lineEbo;
    Shader shader;
    Shader lineShader;

    uint32_t width, height;
    bool ySort = false;
};

Renderer initRenderer(const uint32_t width, const uint32_t height);
void setYsort(Renderer* renderer, bool flag);
void genVertexArrayObject(uint32_t* vao);
void genVertexBuffer(uint32_t* vbo);
void bindVertexArrayObject(uint32_t vao);
void bindVertexArrayBuffer(uint32_t vbo, const float* vertices, uint32_t vertCount);
void setShader(Renderer* renderer, const Shader shader);
void renderDraw(Renderer* renderer, const uint32_t sprite, const float* vertices, const uint32_t vertCount);
void renderDrawQuad(Renderer* renderer, OrtographicCamera camera, glm::vec3 position, const glm::vec3 scale, const glm::vec3 rotation, const Texture* texture, float layer);
void renderDrawQuad(Renderer* renderer, OrtographicCamera camera, glm::vec3 position, const glm::vec3 scale, const glm::vec3 rotation, const Texture* texture, glm::vec2 index, glm::vec2 spriteSize, float layer);
void renderDrawLine(Renderer* renderer, OrtographicCamera camera, const glm::vec2 p0, const glm::vec2 p1, const glm::vec4 color, const float layer);
void renderDrawRect(Renderer* renderer, OrtographicCamera camera, const glm::vec2 offset, const glm::vec2 size, const glm::vec4 color, const float layer);
void clearRenderer();