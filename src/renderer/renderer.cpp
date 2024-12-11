#include "renderer.hpp"
#include "core/tracelog.hpp"

#define MAX_TRIANGLES 2048
#define MAX_VERTICES MAX_TRIANGLES * 3

#define QUAD_VERTEX_SIZE 30


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

//TODO: instead of passing camera do a function beginScene to initilize the camera into the renderer??
void renderDrawQuad(Renderer* renderer, OrtographicCamera camera, const glm::vec3 position, const glm::vec3 size, const glm::vec3 rotation, const Texture* texture){
    glm::vec2 index = {0.0f, 0.0f};
    glm::vec2 spriteSize = {texture->width, texture->height};
    renderDrawQuad(renderer, camera, position, size, rotation, texture, index, spriteSize);
}

glm::vec4 calculateUV(const Texture* texture, glm::vec2 index, glm::vec2 size){

    float tileWidth = (float)size.x / texture->width;
    float tileHeight = (float)size.y / texture->height;

    float tileLeft = tileWidth * index.x;
    float tileRight = tileWidth * (index.x + 1);
    float tileBottom = tileHeight * index.y;
    float tileTop = tileHeight * (index.y + 1);

    return glm::vec4(tileTop, tileLeft, tileBottom, tileRight);
}

void renderDrawQuad(Renderer* renderer, OrtographicCamera camera, const glm::vec3 position, const glm::vec3 size, const glm::vec3 rotation, const Texture* texture,
                    glm::vec2 index, glm::vec2 spriteSize){

    glm::vec4 uv = calculateUV(texture, index, glm::vec2(spriteSize.x, spriteSize.y));
                        
    float vertices[QUAD_VERTEX_SIZE] = {
        // pos              // tex
        0.0f, 1.0f, 0.0f, uv.y, uv.z,
        1.0f, 0.0f, 0.0f, uv.w, uv.x,
        0.0f, 0.0f, 0.0f, uv.y, uv.x, 

        0.0f, 1.0f, 0.0f, uv.y, uv.z,
        1.0f, 1.0f, 0.0f, uv.w, uv.z,
        1.0f, 0.0f, 0.0f, uv.w, uv.x
    };

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, position);
    model = glm::scale(model, size);

    bindVertexArrayObject(renderer);
    bindVertexArrayBuffer(renderer, vertices, QUAD_VERTEX_SIZE);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    useShader(&renderer->shader);

    setUniform(&renderer->shader, "projection", camera.projection);
    setUniform(&renderer->shader, "model", model);
    setUniform(&renderer->shader, "view", camera.view);
    //setUniform(&renderer->shader, "layer", 1.0f + (1.0f - (t->position.y / 320.0f))); //1.0f is the "layer" and 320 the viewport height

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->id);
    glBindVertexArray(renderer->vao);
    glDrawArrays(GL_TRIANGLES, 0, QUAD_VERTEX_SIZE);

}

void clearRenderer(){
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
