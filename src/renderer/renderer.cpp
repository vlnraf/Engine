#include "renderer.hpp"
#include "fontmanager.hpp"
#include "core/tracelog.hpp"
#include <glm/gtx/string_cast.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H  

Renderer* renderer;

void genVertexArrayObject(uint32_t* vao){
    glGenVertexArrays(1, vao);
}

void genVertexBuffer(uint32_t* vbo){
    glGenBuffers(1, vbo);
}

void genFrameBuffer(uint32_t* fbo){
    glGenFramebuffers(1, fbo);
}

void genRenderBuffer(uint32_t* rbo){
    glGenRenderbuffers(1, rbo);
}

void bindVertexArrayObject(uint32_t vao){
    glBindVertexArray(vao);
}

void bindVertexArrayBuffer(uint32_t vbo, const LineVertex* vertices, size_t vertCount){
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(LineVertex) * vertCount, vertices, GL_STATIC_DRAW);
}

void bindVertexArrayBuffer(uint32_t vbo, const QuadVertex* vertices, size_t vertCount){ //std::vector<float> vertices){
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVertex) * vertCount, vertices, GL_STATIC_DRAW);
}

void bindVertexArrayBuffer(uint32_t vbo, const SimpleVertex* vertices, size_t vertCount){ //std::vector<float> vertices){
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SimpleVertex) * vertCount, vertices, GL_STATIC_DRAW);
}

void bindFrameBuffer(uint32_t fbo){
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void bindRenderBuffer(uint32_t rbo){
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
}

void unbindFrameBuffer(uint32_t* fbo){
    glDeleteBuffers(1, fbo);
}

void unbindRenderBuffer(uint32_t* rbo){
    glDeleteBuffers(1, rbo);
}

void genTexture(uint32_t* texture, uint32_t width, uint32_t height){
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void attachFrameBuffer(uint32_t texture){
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
}

void attachRenderBuffer(uint32_t rbo, uint32_t width, uint32_t height){
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
}

void setShader(Renderer* renderer, const Shader shader){
    renderer->shader = shader;
}

void commandDrawSimpleVertex(const SimpleVertex* vertices, const size_t vertCount){ // SpriteComponent* sprite){ //std::vector<float> vertices){
    bindVertexArrayObject(renderer->simpleVao);
    bindVertexArrayBuffer(renderer->simpleVbo, vertices, vertCount);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), (void*)offsetof(SimpleVertex, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), (void*)offsetof(SimpleVertex, color));
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, vertCount);
}

void commandDrawQuad(const QuadVertex* vertices, const size_t vertCount){ // SpriteComponent* sprite){ //std::vector<float> vertices){
    bindVertexArrayObject(renderer->vao);
    bindVertexArrayBuffer(renderer->vbo, vertices, vertCount);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)offsetof(QuadVertex, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)offsetof(QuadVertex, texCoord));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)offsetof(QuadVertex, color));
    glEnableVertexAttribArray(2);

    glVertexAttribIPointer(3, 1, GL_UNSIGNED_BYTE, sizeof(QuadVertex), (void*)offsetof(QuadVertex, texIndex));
    glEnableVertexAttribArray(3);

    glDrawArrays(GL_TRIANGLES, 0, vertCount);
}

void commandDrawLine(const LineVertex* vertices, const size_t vertCount){
    bindVertexArrayObject(renderer->lineVao);
    bindVertexArrayBuffer(renderer->lineVbo, vertices, vertCount);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)offsetof(LineVertex, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)offsetof(LineVertex, color));
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_LINES, 0, vertCount);
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

void initRenderer(Arena* arena, const uint32_t width, const uint32_t height){
    renderer = arenaAllocStruct(arena, Renderer);
    renderer->frameArena = initArena(GB(1));
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
    genVertexArrayObject(&renderer->simpleVao);
    genVertexBuffer(&renderer->simpleVbo);
    //genFrameBuffer(&renderer->fbo);
    //genRenderBuffer(&renderer->rbo);
    LOGINFO("buffer binded");

    //TODO: change to arena implementation
    renderer->shader = createShader("shaders/quad-shader.vs", "shaders/quad-shader.fs");
    renderer->simpleShader = createShader("shaders/simple-shader.vs", "shaders/simple-shader.fs");
    renderer->lineShader = createShader("shaders/line-shader.vs", "shaders/line-shader.fs");
    LOGINFO("shader binded");

    renderer->screenCamera = createCamera({0,0,0}, renderer->width, renderer->height);

    LOGINFO("init renderer finished");
}

glm::vec4 calculateUV(const Texture* texture, glm::vec2 index, glm::vec2 size, glm::vec2 offset){
    float tileWidth = (float)size.x / texture->width;
    float tileHeight = (float)size.y / texture->height;
    glm::vec2 normalizedOffset = {offset.x / size.x, offset.y / size.y};

    glm::vec2 offIndex = index + normalizedOffset;

    float epsilon = 0.01f;
    float uOffset = epsilon / (float) texture->width;
    float vOffset = epsilon / (float) texture->height;

    float tileLeft = tileWidth * offIndex.x + uOffset;
    float tileRight = tileWidth * (offIndex.x + 1) - uOffset;
    float tileBottom = tileHeight * offIndex.y + vOffset;
    float tileTop = tileHeight * (offIndex.y + 1) - vOffset;

    return glm::vec4(tileTop, tileLeft, tileBottom, tileRight);
}

//TODO refactor;
glm::vec4 calculateSpriteUV(const Texture* texture, glm::vec2 index, glm::vec2 size, glm::vec2 tileSize){
    float tileWidth = (float)tileSize.x / texture->width;
    float tileHeight = (float)tileSize.y / texture->height;
    glm::vec2 nextIndex = {size.x / tileSize.x, size.y / tileSize.y};
    float epsilon = 0.01f;
    float uOffset = epsilon / (float) texture->width;
    float vOffset = epsilon / (float) texture->height;

    float tileLeft =    (tileWidth * index.x) + uOffset;
    float tileRight =   (tileWidth * (index.x + nextIndex.x)) - uOffset;
    float tileBottom =  (tileHeight * index.y) + vOffset;
    float tileTop =     (tileHeight * (index.y + nextIndex.y)) - vOffset;

    return glm::vec4(tileTop, tileLeft, tileBottom, tileRight);
}

glm::vec4 calculateSpriteUV(const Texture* texture, Rect sourceRect){
    float epsilon = 0.01f;
    float uOffset = epsilon / (float) texture->width;
    float vOffset = epsilon / (float) texture->height;

    float tileLeft =    sourceRect.pos.x / texture->width + uOffset;
    float tileRight =   (sourceRect.pos.x + sourceRect.size.x) / texture->width - uOffset;
    float tileBottom =  sourceRect.pos.y / texture->height + vOffset;
    float tileTop =     (sourceRect.pos.y + sourceRect.size.y) / texture->height - vOffset;

    return glm::vec4(tileTop, tileLeft, tileBottom, tileRight);
}

void renderStartBatch();
void renderFlush();

void beginScene(RenderMode mode){
    renderer->mode = mode;
    renderer->activeCamera = renderer->screenCamera;
    renderStartBatch();
}

void beginMode2D(OrtographicCamera camera){
    renderFlush();
    renderer->activeCamera = camera;
    renderStartBatch();
}

void beginTextureMode(RenderTexture* renderTexture){
    renderFlush();  // Flush any pending draws

    bindFrameBuffer(renderTexture->fbo);
    attachFrameBuffer(renderTexture->texture.id);
    bindRenderBuffer(renderTexture->rbo);
    attachRenderBuffer(renderTexture->fbo, renderTexture->texture.width, renderTexture->texture.height);

    // Check framebuffer is complete
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        LOGERROR("Framebuffer is not complete!");
    }

    // Set viewport for framebuffer
    glViewport(0, 0, renderTexture->texture.width, renderTexture->texture.height);

    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderStartBatch();
}

void endTextureMode(){
    renderFlush();  // Flush framebuffer draws
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, (uint32_t)renderer->screenCamera.width, (uint32_t)renderer->screenCamera.height);

    renderStartBatch();  // Start fresh batch for screen rendering
}

void endMode2D(){
    renderFlush();
    renderer->activeCamera = renderer->screenCamera;

    renderStartBatch();
}

void endScene(){
    renderFlush();
}

void renderStartBatch(){
    clearArena(&renderer->frameArena);
    renderer->quadVertices = arenaAllocArrayZero(&renderer->frameArena, QuadVertex, MAX_QUADS);
    renderer->lineVertices = arenaAllocArrayZero(&renderer->frameArena, LineVertex, MAX_LINES);
    renderer->simpleVertex = arenaAllocArrayZero(&renderer->frameArena, SimpleVertex, MAX_QUADS);

    renderer->textures = arenaAllocArrayZero(&renderer->frameArena, const Texture*, MAX_TEXTURES_BIND);
    renderer->textures[0] = getTextureByName("default");
    renderer->textureCount = 1;
    renderer->quadVertexCount = 0;
    renderer->lineVertexCount = 0;
    renderer->simpleVertexCount = 0;
}

void renderFlush(){
    if(renderer->mode == RenderMode::NO_DEPTH){
        glDisable(GL_DEPTH_TEST);
    }
    if(renderer->quadVertexCount){
        useShader(&renderer->shader);
        setUniform(&renderer->shader, "projection", renderer->activeCamera.projection);
        setUniform(&renderer->shader, "view", renderer->activeCamera.view);
        for(size_t i = 0; i < renderer->textureCount; i++){
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, renderer->textures[i]->id);
        }
        commandDrawQuad(renderer->quadVertices, renderer->quadVertexCount);
        renderer->drawCalls++;
    }
    if(renderer->simpleVertexCount){
        useShader(&renderer->simpleShader);
        setUniform(&renderer->simpleShader, "projection", renderer->activeCamera.projection);
        setUniform(&renderer->simpleShader, "view", renderer->activeCamera.view);
        commandDrawSimpleVertex(renderer->simpleVertex, renderer->simpleVertexCount);
        renderer->drawCalls++;
    }
    if(renderer->lineVertexCount){
        useShader(&renderer->lineShader);
        setUniform(&renderer->lineShader, "projection", renderer->activeCamera.projection);
        setUniform(&renderer->lineShader, "view", renderer->activeCamera.view);
        commandDrawLine(renderer->lineVertices, renderer->lineVertexCount);
        renderer->drawCalls++;
    }

    if(renderer->mode == RenderMode::NO_DEPTH){
        glEnable(GL_DEPTH_TEST);
    }
}

//TODO: used in tilemap renderer, but it's deprecated
void renderDrawQuadPro(glm::vec3 position, const glm::vec2 size, const glm::vec3 rotation, const Rect sourceRect, const glm::vec2 origin, const Texture* texture,
                    glm::vec4 color, bool ySort, float ySortOffset){

    OrtographicCamera cam = renderer->activeCamera;
    uint8_t textureIndex = 0;

    if(renderer->quadVertexCount >= MAX_VERTICES){
        renderFlush();
        renderStartBatch();
    }

    for(size_t i = 1; i < renderer->textureCount; i++){
        if(renderer->textures[i]->id == texture->id){
            textureIndex = i;
            break;
        }
    }
    if(textureIndex == 0){
        if(renderer->textureCount >= MAX_TEXTURES_BIND){
            renderFlush();
            renderStartBatch();
        }
        renderer->textures[renderer->textureCount] = texture;
        textureIndex = renderer->textureCount;
        renderer->textureCount++;
    }

    // returned a vec4 so i use x,y,z,w to map
    // TODO: make more redable
    glm::vec4 uv = calculateSpriteUV(texture, sourceRect);

    const size_t vertSize = 6;
    //QuadVertex vertices[vertSize];
    //constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
    //glm::vec2 textureCoords[] = { { uv.y, uv.x }, { uv.w, uv.z }, {uv.y, uv.z}, {uv.y, uv.x}, { uv.w, uv.x }, { uv.w, uv.z } };
    glm::vec2 textureCoords[] = { { uv.y, uv.z }, { uv.w, uv.x }, {uv.y, uv.x}, {uv.y, uv.z}, { uv.w, uv.z }, { uv.w, uv.x } };
    //glm::vec4 verterxColor[] = { {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };
    //glm::vec4 vertexPosition[] = {{0.0f, 1.0f, 0.0f, 1.0f},
    //                              {1.0f, 0.0f, 0.0f, 1.0f},
    //                              {0.0f, 0.0f, 0.0f, 1.0f},
    //                              {0.0f, 1.0f, 0.0f, 1.0f},
    //                              {1.0f, 1.0f, 0.0f, 1.0f},
    //                              {1.0f, 0.0f, 0.0f, 1.0f}};
    //glm::vec4 vertexPosition[] = {{-0.5f,  0.5f,  0.0f, 1.0f},
    //                              {0.5f,  -0.5f,  0.0f, 1.0f},
    //                              {-0.5f, -0.5f,  0.0f, 1.0f},
    //                              {-0.5f,  0.5f,  0.0f, 1.0f},
    //                              {0.5f,   0.5f,  0.0f, 1.0f},
    //                              {0.5f,  -0.5f,  0.0f, 1.0f}};

    glm::vec4 vertexPosition[] = {
                                    {-origin.x,        1.0f - origin.y, 0.0f, 1.0f},
                                    {1.0f - origin.x, -origin.y,        0.0f, 1.0f},
                                    {-origin.x,       -origin.y,        0.0f, 1.0f},
                                    {-origin.x,        1.0f - origin.y, 0.0f, 1.0f},
                                    {1.0f - origin.x,  1.0f - origin.y, 0.0f, 1.0f},
                                    {1.0f - origin.x, -origin.y,        0.0f, 1.0f}
                                };

    float layerZ = position.z;  // user-defined
    float ySortZ = 0.0f;
    if (ySort) {
        // Use position.y + ySortOffset as the sort reference
        // ySortOffset allows specifying where the "foot" position is relative to the sprite center
        float sortY = position.y + ySortOffset;
        ySortZ = sortY * 0.001f;   // small scale factor
    }
    // Subtract ySortZ so higher Y positions (farther away) get lower Z values (render behind)
    position.z = layerZ - ySortZ;

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, position);

    glm::vec3 modelCenter(origin.x * size.x, origin.y * size.y, 0.0f);
    model = glm::translate(model, modelCenter);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); //rotate x axis
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); //rotate y axis
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); //rotate z axis
    model = glm::translate(model, -modelCenter);

    // Use size directly instead of scale
    model = glm::scale(model, glm::vec3(size.x, size.y, 1.0f));

    for(size_t i = 0; i < vertSize; i++){
        QuadVertex v = {};
        v.pos = model * vertexPosition[i];
        v.texCoord = textureCoords[i];
        //v.color = verterxColor[i];
        v.color = color;
        v.texIndex = textureIndex;
        renderer->quadVertices[renderer->quadVertexCount] = v;
        renderer->quadVertexCount += 1;
    }
}

// Simple variant: draw whole texture with color tint and single float rotation
void renderDrawQuad(glm::vec3 position, const glm::vec2 size, float rotation, const Texture* texture, glm::vec4 color, bool ySort){
    Rect sourceRect = {.pos = {0,0}, .size = {(float)texture->width, (float)texture->height}};
    renderDrawQuadPro(position, size, {0, 0, rotation}, sourceRect, {0,0}, texture, color, ySort);
}

// Extended variant: atlas region with color tint and optional ySort
void renderDrawQuadEx(glm::vec3 position, const glm::vec2 size, const glm::vec3 rotation, const Texture* texture, const Rect sourceRect, glm::vec4 color, bool ySort){
    renderDrawQuadPro(position, size, rotation, sourceRect, {0,0}, texture, color, ySort);
}

void renderDrawLine(const glm::vec2 p0, const glm::vec2 p1, const glm::vec4 color, const float layer){
    //float normLayer = layer + (1.0f - (1.0f / camera.height));

    glm::vec4 verterxColor[] = { {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };
    glm::vec3 vertexPosition[] = {{p0.x, p0.y, layer},
                                  {p1.x ,p1.y, layer}};

    const size_t vertSize = 2;

    LineVertex vertices[vertSize];
    for(size_t i = 0; i < vertSize; i++){
        LineVertex v = {};
        v.pos = vertexPosition[i];
        v.color = verterxColor[i] * color;
        vertices[i] = v;
        renderer->lineVertices[renderer->lineVertexCount] = v;
        renderer->lineVertexCount += 1;
    }
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

//World text rendering (to be refactored)
void renderDrawText3D(Font* font, const char* text, glm::vec3 pos, float scale, glm::vec4 color){
    if(renderer->quadVertexCount >= MAX_VERTICES){
        renderFlush();
        renderStartBatch();
    }

    uint8_t textureIndex = 0;
    Texture* texture = getTextureByHandle(font->textureHandle);

    for(size_t i = 1; i < renderer->textureCount; i++){
        if(renderer->textures[i]->id == texture->id){
            textureIndex = i;
            break;
        }
    }

    if(textureIndex == 0){
        if(renderer->textureCount >= MAX_TEXTURES_BIND){
            renderFlush();
            renderStartBatch();
        }
        renderer->textures[renderer->textureCount] = texture;
        textureIndex = renderer->textureCount;
        renderer->textureCount++;
    }

    float initPosx = pos.x;
    //float initPosy = pos.y;

    for(int i = 0; text[i] != '\0'; i++){
        float xpos = pos.x + font->characters[(unsigned char) text[i]].Bearing.x * scale;
        float ypos = pos.y + (font->characters[(unsigned char) text[i]].Bearing.y - font->characters[(unsigned char) text[i]].Size.y) * scale;
        if(text[i] == '\n'){
            //NOTE: can be a problem for 3d text and 2d text??
            int padding = 10 * scale;
            pos.y -= (font->characters[(unsigned char) text[i]].Size.y * scale);
            pos.y -= padding;
            pos.x = initPosx;
            continue;
        }
        Character ch = font->characters[(unsigned char) text[i]];

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        // Create Rect for character atlas region (pixel coordinates)
        Rect charRect = {
            .pos = {(float)ch.xOffset, 0.0f},
            .size = {(float)ch.Size.x, (float)ch.Size.y}
        };
        glm::vec4 uv = calculateSpriteUV(texture, charRect);
        // update VBO for each character
        size_t vertSize = 6;
        glm::vec4 vertexPosition[] = { 
            {xpos,     ypos + h, pos.z, 1.0f},//, uv.y, uv.z},//0.0f, 0.0f ,
            {xpos,     ypos,     pos.z, 1.0f},//  uv.y, uv.x},//0.0f, 1.0f ,
            {xpos + w, ypos,     pos.z, 1.0f},//  uv.w, uv.x},//1.0f, 1.0f ,

            {xpos,     ypos + h, pos.z, 1.0f},//, uv.y, uv.z}, //0.0f, 0.0f ,
            {xpos + w, ypos,     pos.z, 1.0f},//  uv.w, uv.x}, //1.0f, 1.0f ,
            {xpos + w, ypos + h, pos.z, 1.0f}};//, uv.w, uv.z}};//1.0f, 0.0f };

        glm::vec2 textureCoords[] = {
            {uv.y, uv.z},
            {uv.y, uv.x},
            {uv.w, uv.x},
            {uv.y, uv.z},
            {uv.w, uv.x},
            {uv.w, uv.z}
        };

        for(size_t i = 0; i < vertSize; i++){
            QuadVertex v = {};
            v.pos = vertexPosition[i];
            v.texCoord = textureCoords[i];
            v.color = color;
            v.texIndex = textureIndex;
            renderer->quadVertices[renderer->quadVertexCount] = v;
            renderer->quadVertexCount += 1;
        }
        pos.x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
}


//------------------------------------------------------ UI methods ------------------------------------------------------
void renderDrawFilledRect(const glm::vec2 position, const glm::vec2 size, float rotation, const glm::vec4 color, const float layer){
    renderDrawFilledRectPro(position, size, rotation, {0,0}, color, layer);
}

void renderDrawFilledRectPro(const glm::vec2 position, const glm::vec2 size, float rotation, const glm::vec2 origin, const glm::vec4 color, const float layer){
    const size_t vertSize = 6;

    glm::vec4 vertexPosition[] = {
                                    {-origin.x,        1.0f - origin.y, 0.0f, 1.0f},
                                    {1.0f - origin.x, -origin.y,        0.0f, 1.0f},
                                    {-origin.x,       -origin.y,        0.0f, 1.0f},
                                    {-origin.x,        1.0f - origin.y, 0.0f, 1.0f},
                                    {1.0f - origin.x,  1.0f - origin.y, 0.0f, 1.0f},
                                    {1.0f - origin.x, -origin.y,        0.0f, 1.0f}
                                };

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(position, layer));

    glm::vec3 modelCenter(0.5f * size.x, 0.5f * size.y, 0.0f);
    model = glm::translate(model, modelCenter);
    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f)); //rotate z axis
    model = glm::translate(model, -modelCenter);

    model = glm::scale(model, glm::vec3(size.x, size.y, 1.0f));

    for(size_t i = 0; i < vertSize; i++){
        SimpleVertex v = {};
        v.pos = model * vertexPosition[i];
        v.color = color;
        renderer->simpleVertex[renderer->simpleVertexCount++] = v;
    }
}

void renderDrawText2D(Font* font, const char* text, glm::vec2 pos, float scale, glm::vec4 color){
    renderDrawText3D(font, text, {pos, 0.0f}, scale, color);
}

// Simple variant: draw whole texture with color tint
void renderDrawQuad2D(glm::vec2 position, const glm::vec2 size, float rotation, const Texture* texture, glm::vec4 color){
    Rect sourceRect = {.pos = {0,0}, .size = {(float)texture->width, (float)texture->height}};
    renderDrawQuadPro({position, 0}, size, {0, 0, rotation}, sourceRect, {0,0}, texture, color, false);
}

// Extended variant: atlas region with color tint
void renderDrawQuadEx2D(glm::vec2 position, const glm::vec2 size, float rotation, const Texture* texture, const Rect sourceRect, glm::vec4 color){
    renderDrawQuadPro({position, 0}, size, {0, 0, rotation}, sourceRect, {0,0}, texture, color, false);
}

// Pro variant: full control with origin
void renderDrawQuadPro2D(glm::vec2 position, const glm::vec2 size, float rotation, const Rect sourceRect, const glm::vec2 origin, const Texture* texture, glm::vec4 color){
    renderDrawQuadPro({position, 0}, size, {0, 0, rotation}, sourceRect, origin, texture, color, false);
}

void destroyRenderer(){
    clearArena(&renderer->frameArena);
    destroyArena(&renderer->frameArena);

    glDeleteVertexArrays(1, &renderer->vao);
    glDeleteBuffers(1, &renderer->vbo);
    glDeleteVertexArrays(1, &renderer->lineVao);
    glDeleteBuffers(1, &renderer->lineVbo);
    glDeleteVertexArrays(1, &renderer->simpleVao);
    glDeleteBuffers(1, &renderer->simpleVbo);
    glDeleteFramebuffers(1, &renderer->fbo);
    glDeleteRenderbuffers(1, &renderer->rbo);
}

//------------------------------------------------------ Configuration API ------------------------------------------------------

void setRenderResolution(uint32_t width, uint32_t height){
    renderer->width = width;
    renderer->height = height;
    renderer->screenCamera = createCamera({0,0,0}, width, height);
}

void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height){
    glViewport(x, y, width, height);
}

glm::vec2 getScreenSize(){
    return {renderer->width, renderer->height};
}

glm::vec2 getRenderSize(){
    return {renderer->width, renderer->height};
}

//------------------------------------------------------ Anchor Helpers ------------------------------------------------------

glm::vec2 anchorTopLeft(float x, float y){
    return {x, renderer->height - y};
}

glm::vec2 anchorTopRight(float x, float y){
    return {renderer->width - x, renderer->height - y};
}

glm::vec2 anchorBottomLeft(float x, float y){
    return {x, y};
}

glm::vec2 anchorBottomRight(float x, float y){
    return {renderer->width - x, y};
}

glm::vec2 anchorCenter(float x, float y){
    return {renderer->width / 2.0f + x, renderer->height / 2.0f + y};
}