#include "renderer.hpp"
#include "fontmanager.hpp"
#include "core/tracelog.hpp"
#include <glm/gtx/string_cast.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H  

#include <map> //TODO: Remove and refactor

//#define MAX_TRIANGLES 2048
//#define MAX_VERTICES MAX_TRIANGLES * 3

//#define QUAD_VERTEX_SIZE 30

static Renderer* renderer;


void initRenderer(const uint32_t width, const uint32_t height){
    renderer = new Renderer();
    //Renderer renderer = {};
    renderer->width = width;
    renderer->height = height;
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    genVertexArrayObject(&renderer->vao);
    genVertexBuffer(&renderer->vbo);
    genVertexArrayObject(&renderer->lineVao);
    genVertexBuffer(&renderer->lineVbo);
    genVertexArrayObject(&renderer->textVao);
    genVertexBuffer(&renderer->textVbo);
    genVertexArrayObject(&renderer->simpleVao);
    genVertexBuffer(&renderer->simpleVbo);
    //genVertexArrayObject(&renderer->uiVao);
    //genVertexBuffer(&renderer->uiVbo);
    renderer->shader = createShader("shaders/quad-shader.vs", "shaders/quad-shader.fs");
    renderer->lineShader = createShader("shaders/line-shader.vs", "shaders/line-shader.fs");
    renderer->textShader = createShader("shaders/text-shader.vs", "shaders/text-shader.fs");
    renderer->simpleShader = createShader("shaders/simple-shader.vs", "shaders/simple-shader.fs");

    renderer->quadVertices.reserve(MAX_QUADS);
    renderer->lineVertices.reserve(MAX_LINES);
}

void destroyRenderer(){
    delete renderer;
}

//void setYsort(Renderer* renderer, bool flag){
//    renderer->ySort = flag;
//}

void genVertexArrayObject(uint32_t* vao){
    glGenVertexArrays(1, vao);
}

void genVertexBuffer(uint32_t* vbo){
    glGenBuffers(1, vbo);
}

void bindVertexArrayObject(uint32_t vao){
    glBindVertexArray(vao);
}

void bindVertexArrayBuffer(uint32_t vbo, const UIVertex* vertices, size_t vertCount){ //std::vector<float> vertices){
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(UIVertex) * vertCount, vertices, GL_STATIC_DRAW);
}

void bindVertexArrayBuffer(uint32_t vbo, const SimpleVertex* vertices, size_t vertCount){ //std::vector<float> vertices){
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SimpleVertex) * vertCount, vertices, GL_STATIC_DRAW);
}

void bindVertexArrayBuffer(uint32_t vbo, const float* vertices, size_t vertCount){
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertCount, vertices, GL_STATIC_DRAW);
}

void bindVertexArrayBuffer(uint32_t vbo, const LineVertex* vertices, size_t vertCount){
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(LineVertex) * vertCount, vertices, GL_STATIC_DRAW);
}

void bindVertexArrayBuffer(uint32_t vbo, const QuadVertex* vertices, size_t vertCount){ //std::vector<float> vertices){
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVertex) * vertCount, vertices, GL_STATIC_DRAW);
}

void setShader(Renderer* renderer, const Shader shader){
    renderer->shader = shader;
}

void commandDrawQuad(const uint32_t textureId, const QuadVertex* vertices, const size_t vertCount){ // SpriteComponent* sprite){ //std::vector<float> vertices){
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)offsetof(QuadVertex, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)offsetof(QuadVertex, texCoord));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)offsetof(QuadVertex, color));
    glEnableVertexAttribArray(2);

    glVertexAttribIPointer(3, 1, GL_UNSIGNED_BYTE, sizeof(QuadVertex), (void*)offsetof(QuadVertex, texIndex));
    glEnableVertexAttribArray(3);

    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, textureId);
    glDrawArrays(GL_TRIANGLES, 0, vertCount);
}

void commandDrawLine(const LineVertex* vertices, const size_t vertCount){
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)offsetof(LineVertex, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)offsetof(LineVertex, color));
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_LINES, 0, vertCount);
}

void commandDrawQuad(const SimpleVertex* vertices, const size_t vertCount){
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), (void*)offsetof(SimpleVertex, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), (void*)offsetof(SimpleVertex, color));
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, vertCount);
}

void commandDrawQuad(const UIVertex* vertices, const size_t vertCount){
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (void*)offsetof(UIVertex, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (void*)offsetof(UIVertex, color));
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, vertCount);
}

void clearColor(float r, float g, float b, float a){
    //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


//-------------------------HIGH LEVEL RENDERERER------------------------------------

//TODO: instead of passing camera do a function beginScene to initilize the camera into the renderer??
//void renderDrawQuad(Renderer* renderer, OrtographicCamera camera, glm::vec3 position, const glm::vec3 scale, const glm::vec3 rotation, const Texture* texture){
//    glm::vec2 index = {0.0f, 0.0f};
//    glm::vec2 spriteSize = {texture->width, texture->height};
//    renderDrawQuad(renderer, camera, position, scale, rotation, texture, index, spriteSize);
//}

glm::vec4 calculateUV(const Texture* texture, glm::vec2 index, glm::vec2 size, glm::vec2 offset){
    float tileWidth = (float)size.x / texture->width;
    float tileHeight = (float)size.y / texture->height;
    glm::vec2 normalizedOffset = {offset.x / size.x, offset.y / size.y};

    glm::vec2 offIndex = index + normalizedOffset;

    float tileLeft = tileWidth * offIndex.x;
    float tileRight = tileWidth * (offIndex.x + 1);
    float tileBottom = tileHeight * offIndex.y;
    float tileTop = tileHeight * (offIndex.y + 1);

    return glm::vec4(tileTop, tileLeft, tileBottom, tileRight);
}

glm::vec4 calculateSpriteUV(const Texture* texture, glm::vec2 index, glm::vec2 size, glm::vec2 tileSize){
    float tileWidth = (float)tileSize.x / texture->width;
    float tileHeight = (float)tileSize.y / texture->height;
    glm::vec2 nextIndex = {size.x / tileSize.x, size.y / tileSize.y};

    float tileLeft =    tileWidth * index.x;
    float tileRight =   tileWidth * (index.x + nextIndex.x);
    float tileBottom =  tileHeight * index.y;
    float tileTop =     tileHeight * (index.y + nextIndex.y);

    return glm::vec4(tileTop, tileLeft, tileBottom, tileRight);
}

void renderStartBatch();
void renderFlush();

void beginScene(const OrtographicCamera* camera){
    renderer->camera = camera;
    renderStartBatch();
}

void endScene(){
    renderFlush();
}

void renderStartBatch(){
    renderer->quadVertices.clear();
    renderer->quadVertexCount = 0;

    renderer->lineVertices.clear();
    renderer->lineVertexCount = 0;

    renderer->textures.clear();
    renderer->textures.push_back(*getTexture("default"));
    renderer->textureIndex = 1;

    renderer->texture = *getTexture("default");
}

void renderFlush(){
    if(renderer->quadVertexCount){
        bindVertexArrayObject(renderer->vao);
        bindVertexArrayBuffer(renderer->vbo, renderer->quadVertices.data(), renderer->quadVertices.size());
        useShader(&renderer->shader);
        setUniform(&renderer->shader, "projection", renderer->camera->projection);
        setUniform(&renderer->shader, "view", renderer->camera->view);
        for(size_t i = 0; i < renderer->textureIndex; i++){
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, renderer->textures[i].id);
        }
        commandDrawQuad(renderer->texture.id, renderer->quadVertices.data(), renderer->quadVertices.size());
        renderer->drawCalls++;
    }
    if(renderer->lineVertexCount){

        bindVertexArrayObject(renderer->lineVao);
        bindVertexArrayBuffer(renderer->lineVbo, renderer->lineVertices.data(), renderer->lineVertices.size());
        useShader(&renderer->lineShader);
        setUniform(&renderer->lineShader, "projection", renderer->camera->projection);
        setUniform(&renderer->lineShader, "view", renderer->camera->view);
        commandDrawLine(renderer->lineVertices.data(), renderer->lineVertices.size());
        renderer->drawCalls++;
    }
    //renderStartBatch();
}

//TODO: used in tilemap renderer, but it's deprecated
void renderDrawQuad(glm::vec3 position, const glm::vec3 scale, const glm::vec3 rotation, const Texture* texture,
                    glm::vec2 index, glm::vec2 spriteSize, bool ySort){

    uint8_t textureIndex = 0;

    if(renderer->quadVertexCount >= MAX_VERTICES){
        renderFlush();
        renderStartBatch();
    }

    for(size_t i = 1; i < renderer->textures.size(); i++){
        if(renderer->textures[i].id == texture->id){
            textureIndex = i;
            break;
        }
    }
    if(textureIndex == 0){
        if(renderer->textures.size() >= MAX_TEXTURES_BIND){
            renderFlush();
            renderStartBatch();
        }
        renderer->textures.push_back(*texture);
        textureIndex = renderer->textureIndex;
        renderer->textureIndex++;
    }

    // returned a vec4 so i use x,y,z,w to map
    // TODO: make more redable
    glm::vec4 uv = calculateSpriteUV(texture, index, spriteSize, spriteSize);

    const size_t vertSize = 6;
    //QuadVertex vertices[vertSize];
    //constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
    glm::vec2 textureCoords[] = { { uv.y, uv.z }, { uv.w, uv.x }, {uv.y, uv.x}, {uv.y, uv.z}, { uv.w, uv.z }, { uv.w, uv.x } };
    glm::vec4 verterxColor[] = { {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };
    glm::vec4 vertexPosition[] = {{0.0f, 1.0f, 0.0f, 1.0f},
                                  {1.0f, 0.0f, 0.0f, 1.0f},
                                  {0.0f, 0.0f, 0.0f, 1.0f}, 
                                  {0.0f, 1.0f, 0.0f, 1.0f},
                                  {1.0f, 1.0f, 0.0f, 1.0f},
                                  {1.0f, 0.0f, 0.0f, 1.0f}};

    if(ySort){
        position.z = position.z + (1.0f - (position.y / (renderer->camera->position.y + renderer->camera->height))); 
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
    model = glm::scale(model, glm::vec3(spriteSize.x, spriteSize.y, 1.0f));

    for(int i = 0; i < vertSize; i++){
        QuadVertex v = {};
        v.pos = model * vertexPosition[i];
        v.texCoord = textureCoords[i];
        v.color = verterxColor[i];
        v.texIndex = textureIndex;
        //vertices[i] = v;
        renderer->quadVertices.push_back(v);
    }
    renderer->quadVertexCount += 6;
}

void renderDrawLine(const glm::vec2 p0, const glm::vec2 p1, const glm::vec4 color, const float layer){
    //float normLayer = layer + (1.0f - (1.0f / camera.height));

    glm::vec4 verterxColor[] = { {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };
    glm::vec3 vertexPosition[] = {{p0.x, p0.y, layer},
                                  {p1.x ,p1.y, layer}};

    const size_t vertSize = 2;

    LineVertex vertices[vertSize];
    for(int i = 0; i < vertSize; i++){
        LineVertex v = {};
        v.pos = vertexPosition[i];
        v.color = verterxColor[i] * color;
        vertices[i] = v;
        renderer->lineVertices.push_back(v);
    }
    renderer->lineVertexCount += 2;
}

void renderDrawRect(const glm::vec2 offset, const glm::vec2 size, const glm::vec4 color, const float layer){
    glm::vec2 p0 = {offset.x , offset.y};
    glm::vec2 p1 = {offset.x + size.x, offset.y};
    glm::vec2 p2 = {offset.x + size.x, offset.y + size.y};
    glm::vec2 p3 = {offset.x, offset.y + size.y};

    renderDrawLine(p0, p1, color, layer);
    renderDrawLine(p1, p2, color, layer);
    renderDrawLine(p2, p3, color, layer);
    renderDrawLine(p3, p0, color, layer);
}

void renderDrawRect(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, const glm::vec2 offset, const glm::vec2 size, const glm::vec4 color, const float layer){
    glm::vec2 p0 = {offset.x , offset.y};
    glm::vec2 p1 = {offset.x + size.x, offset.y};
    glm::vec2 p2 = {offset.x + size.x, offset.y + size.y};
    glm::vec2 p3 = {offset.x, offset.y + size.y};

    renderDrawLine(p0, p1, color, layer);
    renderDrawLine(p1, p2, color, layer);
    renderDrawLine(p2, p3, color, layer);
    renderDrawLine(p3, p0, color, layer);
}

void renderDrawSprite(glm::vec3 position, const glm::vec3 scale, const glm::vec3 rotation, const SpriteComponent* sprite){
    if(renderer->quadVertexCount >= MAX_VERTICES){
        renderFlush();
        renderStartBatch();
    }

    uint8_t textureIndex = 0;

    for(size_t i = 1; i < renderer->textures.size(); i++){
        if(renderer->textures[i].id == sprite->texture->id){
            textureIndex = i;
            break;
        }
    }
    if(textureIndex == 0){
        if(renderer->textures.size() >= MAX_TEXTURES_BIND){
            renderFlush();
            renderStartBatch();
        }
        renderer->textures.push_back(*sprite->texture);
        textureIndex = renderer->textureIndex;
        renderer->textureIndex++;
    }

    glm::vec4 uv;
    if(glm::length(sprite->tileSize) == 0){
        uv = calculateSpriteUV(sprite->texture, sprite->index, sprite->size, sprite->size);
    }else{
        uv = calculateSpriteUV(sprite->texture, sprite->index, sprite->size, sprite->tileSize);
    }

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
    const size_t vertSize = 6;
    QuadVertex vertices[vertSize];
    //constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
    glm::vec2 textureCoords[] = { { uv.y, uv.z }, { uv.w, uv.x }, {uv.y, uv.x}, {uv.y, uv.z}, { uv.w, uv.z }, { uv.w, uv.x } };
    glm::vec4 verterxColor[] = { {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };

    //Bot left origin
    glm::vec4 vertexPosition[] = {{0.0f, 1.0f, 0.0f, 1.0f},
                                  {1.0f, 0.0f, 0.0f, 1.0f},
                                  {0.0f, 0.0f, 0.0f, 1.0f}, 
                                  {0.0f, 1.0f, 0.0f, 1.0f},
                                  {1.0f, 1.0f, 0.0f, 1.0f},
                                  {1.0f, 0.0f, 0.0f, 1.0f}};

    //Center origin
    //glm::vec3 vertexPosition[] = {{-0.5f, 0.5f, 0.0f},
    //                              {0.5f, -0.5f, 0.0f},
    //                              {-0.5f, -0.5f, 0.0f}, 
    //                              {-0.5f, 0.5f, 0.0f},
    //                              {0.5f, 0.5f, 0.0f},
    //                              {0.5f, -0.5f, 0.0f}};
                        
    //NOTE: y sort based on layer and y position of the quad
    //I normalize it to don't let layers explode and generate high numbers
    //NOTE: what happens if we render in negative space?? the normalization goes wrong, should we take the absolute values????
    if(sprite->ySort){
        position.z = sprite->layer + (1.0f - (position.y / (renderer->camera->position.y + renderer->camera->height))); 
    }else{
        position.z = sprite->layer;
    }

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, position);

    glm::vec3 modelCenter(0,0,0);
    if(sprite->pivot == SpriteComponent::PIVOT_CENTER){
        glm::vec3 modelCenter(0.5f * sprite->size.x, 0.5f * sprite->size.y, 0.0f);
    }
    model = glm::translate(model, modelCenter);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); //rotate x axis
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); //rotate y axis
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); //rotate z axis
    model = glm::translate(model, -modelCenter);

    //TODO: scale inside model to flip in center
    //the problem is that if i do this the collider is missaligned
    model = glm::scale(model, glm::vec3(scale.x, scale.y, 1.0f));
    model = glm::scale(model, glm::vec3(sprite->size, 1.0f));

    for(int i = 0; i < vertSize; i++){
        QuadVertex v = {};
        v.pos = model * vertexPosition[i];
        v.texCoord = textureCoords[i];
        //v.color = verterxColor[i];
        v.color = sprite->color;
        //vertices[i] = v;
        v.texIndex = textureIndex;
        renderer->quadVertices.push_back(v);
    }

    renderer->quadVertexCount += 6;
}

void renderDrawText(Font* font, OrtographicCamera camera, const char* text, float x, float y, float scale){
    glClear(GL_DEPTH_TEST);
    glDisable(GL_DEPTH_TEST);

    useShader(&renderer->textShader);
    setUniform(&renderer->textShader, "projection", camera.projection);
    setUniform(&renderer->textShader, "textColor", glm::vec3(1,1,1));

    glActiveTexture(GL_TEXTURE0);
    bindVertexArrayObject(renderer->textVao);

    // iterate through all characters
    //std::string::const_iterator c;
    //NOTE: not sure if it's safe!!!
    for(int i = 0; text[i] != '\0'; i++){
    //for (c = text.begin(); c != text.end(); c++){
        Character ch = font->characters[(unsigned char) text[i]];

        float xpos = x;
        float ypos = y;
        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        glm::vec4 uv = calculateUV(font->texture, {0,0}, {ch.Size.x, ch.Size.y},{ch.xOffset, 0}); //index is always 0 because the character size change and so we can't rely on index
        // update VBO for each character
        float vertices[6*4] = { 
            xpos,     ypos + h, uv.y, uv.z,//0.0f, 0.0f ,            
            xpos,     ypos,     uv.y, uv.x,//0.0f, 1.0f ,
            xpos + w, ypos,     uv.w, uv.x,//1.0f, 1.0f ,

            xpos,     ypos + h, uv.y, uv.z, //0.0f, 0.0f ,
            xpos + w, ypos,     uv.w, uv.x, //1.0f, 1.0f ,
            xpos + w, ypos + h, uv.w, uv.z};//1.0f, 0.0f };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, font->texture->id);

        bindVertexArrayObject(renderer->textVao);
        size_t vertSize = 6*4;
        bindVertexArrayBuffer(renderer->textVbo, vertices, vertSize);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2* sizeof(float)));
        glEnableVertexAttribArray(1);
        useShader(&renderer->textShader);
        setUniform(&renderer->textShader, "projection", camera.projection);
        setUniform(&renderer->textShader, "textColor", glm::vec3(1,1,1)); //TODO: take color as input
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glEnable(GL_DEPTH_TEST);
}

void renderDrawFilledRect(const glm::vec3 position, const glm::vec2 size, const glm::vec3 rotation, const glm::vec4 color){
    if(renderer->quadVertexCount >= MAX_VERTICES){
        renderFlush();
        renderStartBatch();
    }
    const size_t vertSize = 6;
    SimpleVertex vertices[vertSize];
    //constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

    //Bot left origin
    glm::vec4 vertexPosition[] = {{0.0f, 1.0f, 0.0f, 1.0f},
                                  {1.0f, 0.0f, 0.0f, 1.0f},
                                  {0.0f, 0.0f, 0.0f, 1.0f}, 
                                  {0.0f, 1.0f, 0.0f, 1.0f},
                                  {1.0f, 1.0f, 0.0f, 1.0f},
                                  {1.0f, 0.0f, 0.0f, 1.0f}};

    glm::vec2 textureCoords[] = { {0, 0}, {1, 0}, {1, 1}, {0, 1}, {0, 0}, {1, 0} };

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, position);

    glm::vec3 modelCenter(0,0,0);
    model = glm::translate(model, modelCenter);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); //rotate x axis
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); //rotate y axis
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); //rotate z axis
    model = glm::translate(model, -modelCenter);

    //TODO: scale inside model to flip in center
    //the problem is that if i do this the collider is missaligned
    model = glm::scale(model, glm::vec3(size.x, size.y, 1.0f));

    for(int i = 0; i < vertSize; i++){
        QuadVertex v = {};
        v.pos = model * vertexPosition[i];
        v.color = color;
        v.texCoord = textureCoords[i];
        v.texIndex = 0;
        renderer->quadVertices.push_back(v);
        //vertices[i] = v;
    }
    renderer->quadVertexCount += 6;
}