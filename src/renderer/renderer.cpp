#include "renderer.hpp"
#include "core/tracelog.hpp"

#define MAX_TRIANGLES 2048
#define MAX_VERTICES MAX_TRIANGLES * 3


Renderer* initRenderer(const uint32_t width, const uint32_t height){
    Renderer* renderer = new Renderer();
    renderer->width = width;
    renderer->height = height;
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    genVertexArrayObject(renderer);
    genVertexBuffer(renderer);

    return renderer;
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

void bindVertexArrayBuffer(Renderer* renderer, const float* vertices, uint32_t vertCount){ //std::vector<float> vertices){
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertCount, vertices, GL_STATIC_DRAW);
}

void setShader(Renderer* renderer, Shader shader){
    renderer->shader = shader;
}

void renderDraw(Renderer* renderer, const uint32_t sprite, const float* vertices, const uint32_t vertCount){ // SpriteComponent* sprite){ //std::vector<float> vertices){
    bindVertexArrayObject(renderer);
    bindVertexArrayBuffer(renderer, vertices, vertCount);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    useShader(&renderer->shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sprite);
    glBindVertexArray(renderer->vao);
    glDrawArrays(GL_TRIANGLES, 0, vertCount);
}

void clearRenderer(){
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
