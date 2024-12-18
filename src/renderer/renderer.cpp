#include "renderer.hpp"
#include "core/tracelog.hpp"

#define MAX_TRIANGLES 2048
#define MAX_VERTICES MAX_TRIANGLES * 3

#define QUAD_VERTEX_SIZE 30


Renderer initRenderer(const uint32_t width, const uint32_t height){
    //Renderer* renderer = new Renderer();
    Renderer renderer = {};
    renderer.width = width;
    renderer.height = height;
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    genVertexArrayObject(&renderer);
    genVertexBuffer(&renderer);

    return renderer;
}

void setYsort(Renderer* renderer, bool flag){
    renderer->ySort = flag;
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

void setShader(Renderer* renderer, const Shader shader){
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
void renderDrawQuad(Renderer* renderer, OrtographicCamera camera, glm::vec3 position, const glm::vec3 scale, const glm::vec3 rotation, const Texture* texture, float layer){
    glm::vec2 index = {0.0f, 0.0f};
    glm::vec2 spriteSize = {texture->width, texture->height};
    renderDrawQuad(renderer, camera, position, scale, rotation, texture, index, spriteSize, layer);
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

void renderDrawQuad(Renderer* renderer, OrtographicCamera camera, glm::vec3 position, const glm::vec3 scale, const glm::vec3 rotation, const Texture* texture,
                    glm::vec2 index, glm::vec2 spriteSize, float layer){

    //TODO: batch rendering in future to improve performances

    glm::vec4 uv = calculateUV(texture, index, glm::vec2(spriteSize.x, spriteSize.y));
                        
    // returned a vec4 so i use x,y,z,w to map
    // TODO: make more redable
    float vertices[QUAD_VERTEX_SIZE] = {
        // pos              // tex
        0.0f, 1.0f, 0.0f, uv.y, uv.z,
        1.0f, 0.0f, 0.0f, uv.w, uv.x,
        0.0f, 0.0f, 0.0f, uv.y, uv.x, 

        0.0f, 1.0f, 0.0f, uv.y, uv.z,
        1.0f, 1.0f, 0.0f, uv.w, uv.z,
        1.0f, 0.0f, 0.0f, uv.w, uv.x
    };

    //NOTE: y sort based on layer and y position of the quad
    //I normalize it to don't let layers explode and generate high numbers
    if(renderer->ySort){
        position.z = layer + (1.0f - (position.y / camera.height)); //1.0f is the "layer" and 320 the viewport height
    }

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, position);

    glm::vec3 modelCenter(0.5f * spriteSize.x * fabs(scale.x), 0.5f * spriteSize.y * fabs(scale.y), 0.0f);
    model = glm::translate(model, modelCenter);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); //rotate x axis
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); //rotate x axis
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); //rotate x axis
    model = glm::scale(model, glm::vec3(scale.x, scale.y, 1.0f));
    model = glm::translate(model, -modelCenter);
    model = glm::scale(model, glm::vec3(spriteSize.x, spriteSize.y, 1.0f));

    //model = glm::scale(model, glm::vec3(spriteSize.x * scale.x, spriteSize.y * scale.y, 1.0f));

    //NOTE: to flip the quad in place
    // why the fck scaling with modelCenter translated doesn't work???
    //if(scale.x < 0){
    //  model = glm::translate(model, glm::vec3(-scale.x, 0.0f, 0.0f));
    //  model = glm::scale(model, glm::vec3(spriteSize, 1.0f)) * glm::scale(glm::mat4(1.0f), scale);
    //  model = glm::translate(model, glm::vec3(scale.x, 0.0f, 0.0f));

    //}else{
    //  model = glm::scale(model, glm::vec3(spriteSize, 1.0f)) * glm::scale(glm::mat4(1.0f), scale);
    //}
    
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
    //glBindVertexArray(renderer->vao);
    bindVertexArrayObject(renderer);
    glDrawArrays(GL_TRIANGLES, 0, QUAD_VERTEX_SIZE);

}

void clearRenderer(){
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
