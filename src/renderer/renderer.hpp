#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "shader.hpp"
#include "texture.hpp"
//struct VertexBuffer{
//    float vertices[18];
//    uint32_t vertCount;
//};

struct Renderer{
    uint32_t vao, vbo, ebo;
    Shader shader;

    uint32_t width, height;
};

Renderer* initRenderer(const uint32_t width, const uint32_t height);
void genVertexArrayObject(Renderer* renderer);
void genVertexBuffer(Renderer* renderer);
void bindVertexArrayBuffer(Renderer* renderer, float* vertices, uint32_t vertCount);
void bindVertexArrayBuffer(Renderer* renderer);
void setShader(Renderer* renderer, Shader shader);
void renderDraw(Renderer* renderer, const Texture* sprite, const float* vertices, const uint32_t vertCount);
//void renderDraw(Renderer* renderer, SpriteComponent* sprite);
void clearRenderer();