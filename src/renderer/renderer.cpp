#include "renderer.hpp"
#include "core/tracelog.hpp"
#include <glm/gtx/string_cast.hpp>

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
    genVertexArrayObject(&renderer.vao);
    genVertexBuffer(&renderer.vbo);
    genVertexArrayObject(&renderer.lineVao);
    genVertexBuffer(&renderer.lineVbo);

    return renderer;
}

void setYsort(Renderer* renderer, bool flag){
    renderer->ySort = flag;
}

void genVertexArrayObject(uint32_t* vao){
    glGenVertexArrays(1, vao);
}

void genVertexBuffer(uint32_t* vbo){
    glGenBuffers(1, vbo);
}

void bindVertexArrayObject(uint32_t vao){
    glBindVertexArray(vao);
}

void bindVertexArrayBuffer(uint32_t vbo, const float* vertices, uint32_t vertCount){ //std::vector<float> vertices){
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertCount, vertices, GL_STATIC_DRAW);
}

void setShader(Renderer* renderer, const Shader shader){
    renderer->shader = shader;
}

void renderDraw(Renderer* renderer, const uint32_t sprite, const float* vertices, const uint32_t vertCount){ // SpriteComponent* sprite){ //std::vector<float> vertices){
    bindVertexArrayObject(renderer->vao);
    bindVertexArrayBuffer(renderer->vbo, vertices, vertCount);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    useShader(&renderer->shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sprite);
    bindVertexArrayObject(renderer->vao);
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
        0.0f, spriteSize.y, 0.0f, uv.y, uv.z,
        spriteSize.x, 0.0f, 0.0f, uv.w, uv.x,
        0.0f, 0.0f, 0.0f, uv.y, uv.x, 

        0.0f, spriteSize.y, 0.0f, uv.y, uv.z,
        spriteSize.x, spriteSize.y, 0.0f, uv.w, uv.z,
        spriteSize.x, 0.0f, 0.0f, uv.w, uv.x
    };
    //NOTE: y sort based on layer and y position of the quad
    //I normalize it to don't let layers explode and generate high numbers
    if(renderer->ySort){
        position.z = layer + (1.0f - (position.y / camera.height)); //1.0f is the "layer" and 320 the viewport height
    }

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, position);

    glm::vec3 modelCenter(0.5f * spriteSize.x, 0.5f * spriteSize.y, 0.0f);
    model = glm::translate(model, modelCenter);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); //rotate x axis
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); //rotate y axis
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); //rotate z axis
    model = glm::translate(model, -modelCenter);

    //TODO: scale inside model to flip in center
    //the problem is that if i do this the collider is missaligned
    model = glm::scale(model, glm::vec3(scale.x, scale.y, 1.0f));

    bindVertexArrayObject(renderer->vao);
    bindVertexArrayBuffer(renderer->vbo, vertices, QUAD_VERTEX_SIZE);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    useShader(&renderer->shader);

    setUniform(&renderer->shader, "projection", camera.projection);
    setUniform(&renderer->shader, "model", model);
    setUniform(&renderer->shader, "view", camera.view);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->id);
    bindVertexArrayObject(renderer->vao);
    glDrawArrays(GL_TRIANGLES, 0, QUAD_VERTEX_SIZE);

}

//NOTE: layer can be removed if ySort is disabled when drawing lines
void renderDrawLine(Renderer* renderer, OrtographicCamera camera, const glm::vec2 p0, const glm::vec2 p1, const glm::vec4 color, const float layer){

    useShader(&renderer->lineShader);

    float normLayer = layer + (1.0f - (1.0f / camera.height));
    float vertices[14] = {
        // pos              //Color
        p0.x, p0.y, normLayer, color.r, color.g, color.b, color.a,
        p1.x, p1.y, normLayer, color.r, color.g, color.b, color.a
    };

    glm::mat4 model = glm::mat4(1.0f);
    bindVertexArrayObject(renderer->lineVao);
    bindVertexArrayBuffer(renderer->lineVbo, vertices, 14);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    setUniform(&renderer->lineShader, "projection", camera.projection);
    setUniform(&renderer->lineShader, "model", model);
    setUniform(&renderer->lineShader, "view", camera.view);

    bindVertexArrayObject(renderer->lineVao);
    glDrawArrays(GL_LINES, 0, 2);
}

//NOTE: layer can be removed if ySort is disabled when drawing rects
void renderDrawRect(Renderer* renderer, OrtographicCamera camera, const glm::vec2 offset, const glm::vec2 size, const glm::vec4 color, const float layer){
    glm::vec2 p0 = {offset.x , offset.y};
    glm::vec2 p1 = {offset.x + size.x, offset.y};
    glm::vec2 p2 = {offset.x + size.x, offset.y + size.y};
    glm::vec2 p3 = {offset.x, offset.y + size.y};

    renderDrawLine(renderer, camera, p0, p1, color, layer);
    renderDrawLine(renderer, camera, p1, p2, color, layer);
    renderDrawLine(renderer, camera, p2, p3, color, layer);
    renderDrawLine(renderer, camera, p3, p0, color, layer);
}

void clearRenderer(){
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void renderDrawSprite(Renderer* renderer, OrtographicCamera camera, glm::vec3 position, const glm::vec3 scale, const glm::vec3 rotation, const SpriteComponent* sprite){

    //TODO: batch rendering in future to improve performances
    glm::vec4 uv = calculateUV(sprite->texture, sprite->index, glm::vec2(sprite->size.x, sprite->size.y));

    if(sprite->flipX){
        glm::vec4 newUv = uv;
        uv.y = newUv.w;
        uv.w = newUv.y;
    }
    if(sprite->flipY){
        glm::vec4 newUv = uv;
        uv.x = newUv.z;
        uv.z = newUv.x;
    }
                        
    // returned a vec4 so i use x,y,z,w to map
    // TODO: make more redable
    float vertices[QUAD_VERTEX_SIZE] = {
        // pos              // tex
        0.0f, sprite->size.y, 0.0f, uv.y, uv.z,
        sprite->size.x, 0.0f, 0.0f, uv.w, uv.x,
        0.0f, 0.0f, 0.0f, uv.y, uv.x, 

        0.0f, sprite->size.y, 0.0f, uv.y, uv.z,
        sprite->size.x, sprite->size.y, 0.0f, uv.w, uv.z,
        sprite->size.x, 0.0f, 0.0f, uv.w, uv.x
    };
    //NOTE: y sort based on layer and y position of the quad
    //I normalize it to don't let layers explode and generate high numbers
    if(renderer->ySort){
        position.z = sprite->layer + (1.0f - (position.y / camera.height)); //1.0f is the "layer" and 320 the viewport height
    }

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, position);

    glm::vec3 modelCenter(0.5f * sprite->size.x, 0.5f * sprite->size.y, 0.0f);
    model = glm::translate(model, modelCenter);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); //rotate x axis
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); //rotate y axis
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); //rotate z axis
    model = glm::translate(model, -modelCenter);

    //TODO: scale inside model to flip in center
    //the problem is that if i do this the collider is missaligned
    model = glm::scale(model, glm::vec3(scale.x, scale.y, 1.0f));

    bindVertexArrayObject(renderer->vao);
    bindVertexArrayBuffer(renderer->vbo, vertices, QUAD_VERTEX_SIZE);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    useShader(&renderer->shader);

    setUniform(&renderer->shader, "projection", camera.projection);
    setUniform(&renderer->shader, "model", model);
    setUniform(&renderer->shader, "view", camera.view);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sprite->texture->id);
    bindVertexArrayObject(renderer->vao);
    glDrawArrays(GL_TRIANGLES, 0, QUAD_VERTEX_SIZE);

}