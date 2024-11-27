#include "renderer.hpp"

#define MAX_TRIANGLES 2048
#define MAX_VERTICES MAX_TRIANGLES * 3


void initRenderer(Renderer* renderer, const uint32_t width, const uint32_t height){
    renderer->width = width;
    renderer->height = height;
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    genVertexArrayObject(renderer);
    genVertexBuffer(renderer);
}

void genVertexArrayObject(Renderer* renderer){
    glGenVertexArrays(1, &renderer->vao);
}

void genVertexBuffer(Renderer* renderer){
    glGenBuffers(1, &renderer->vbo);
}

void bindVertexArrayObject(Renderer* renderer){
    glBindVertexArray(renderer->vao);
}

void bindVertexArrayBuffer(Renderer* renderer, VertexBuffer vertices){
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.vertices, GL_STATIC_DRAW);
}

void setShader(Renderer* renderer, Shader shader){
    renderer->shader = shader;
}

void renderDraw(Renderer* renderer, VertexBuffer vertices){
    bindVertexArrayObject(renderer);
    bindVertexArrayBuffer(renderer, vertices);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    useShader(&renderer->shader);
    glBindVertexArray(renderer->vao);
    glDrawArrays(GL_TRIANGLES, 0, vertices.vertCount);
}

void clearRenderer(){
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}