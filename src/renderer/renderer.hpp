#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../shader.hpp"

struct VertexBuffer{
    float vertices[18];
    uint32_t vertCount;
};

struct Renderer{
    uint32_t vao, vbo, ebo;
    Shader shader;

    uint32_t width, height;
};

void initRenderer(Renderer* renderer, const uint32_t width, const uint32_t height);
void genVertexArrayObject(Renderer* renderer);
void genVertexBuffer(Renderer* renderer);
void bindVertexArrayObject(Renderer* renderer, VertexBuffer vertices);
void bindVertexArrayBuffer(Renderer* renderer);
void setShader(Renderer* renderer, Shader shader);
void renderDraw(Renderer* renderer, VertexBuffer vertices);
void clearRenderer();