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
    Shader shader;

    uint32_t width, height;
};

Renderer* initRenderer(const uint32_t width, const uint32_t height);
void genVertexArrayObject(Renderer* renderer);
void genVertexBuffer(Renderer* renderer);
void bindVertexArrayBuffer(Renderer* renderer, const float* vertices, uint32_t vertCount);
//void bindVertexArrayBuffer(Renderer* renderer);
void setShader(Renderer* renderer, Shader shader);
void renderDraw(Renderer* renderer, const uint32_t sprite, const float* vertices, const uint32_t vertCount);
//void renderDrawQuad(Renderer* renderer, OrtographicCamera camera, const glm::vec3 position, const glm::vec3 size, const glm::vec3 rotation, const uint32_t texture);
void renderDrawQuad(Renderer* renderer, OrtographicCamera camera, const glm::vec3 position, const glm::vec3 size, const glm::vec3 rotation, const Texture* texture);
void renderDrawQuad(Renderer* renderer, OrtographicCamera camera, const glm::vec3 position, const glm::vec3 size, const glm::vec3 rotation, const Texture* texture, glm::vec2 index, glm::vec2 spriteSize);
//void renderDraw(Renderer* renderer, SpriteComponent* sprite);
void clearRenderer();