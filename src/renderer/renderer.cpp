#include "renderer.hpp"
#include "fontmanager.hpp"
#include "core/tracelog.hpp"
#include <glm/gtx/string_cast.hpp>

#ifndef __EMSCRIPTEN__
#include <glad/glad.h>
#else
#include <GLES3/gl3.h>
#endif

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

// Low-level OpenGL resource destruction
void deleteVertexArrayObject(uint32_t vao){
    glDeleteVertexArrays(1, &vao);
}

void deleteVertexBuffer(uint32_t vbo){
    glDeleteBuffers(1, &vbo);
}

void deleteFrameBuffer(uint32_t fbo){
    glDeleteFramebuffers(1, &fbo);
}

void deleteRenderBuffer(uint32_t rbo){
    glDeleteRenderbuffers(1, &rbo);
}

void deleteTexture(uint32_t texture){
    glDeleteTextures(1, &texture);
}

void bindVertexArrayObject(uint32_t vao){
    glBindVertexArray(vao);
}

// Single implementation for all vertex types (they're all the same now!)
void bindVertexArrayBuffer(uint32_t vbo, const Vertex* vertices, size_t vertCount){
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertCount, vertices, GL_STATIC_DRAW);
}

void bindFrameBuffer(uint32_t fbo){
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void unbindFrameBuffer(){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void bindRenderBuffer(uint32_t rbo){
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
}

void unbindRenderBuffer(){
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void genRenderTexture(uint32_t* texture, uint32_t width, uint32_t height){
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void genTexture(Texture* texture, uint32_t format, unsigned char* data){
    glGenTextures(1, &texture->id);
    glBindTexture(GL_TEXTURE_2D, texture->id);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, format, texture->width, texture->height, 0, format, GL_UNSIGNED_BYTE, data);
}

void attachFrameBuffer(uint32_t texture){
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
}

void attachRenderBuffer(uint32_t rbo, uint32_t width, uint32_t height){
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
}

// Generic draw function - handles all geometry types with standard vertex layout
void commandDraw(uint32_t vao, uint32_t vbo, const Vertex* vertices, size_t vertCount, GLenum primitiveType){
    bindVertexArrayObject(vao);
    bindVertexArrayBuffer(vbo, vertices, vertCount);

    // Standard vertex layout (same for all geometry types)
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    #ifdef __EMSCRIPTEN__
    // WebGL doesn't support integer vertex attributes properly, use float and cast in shader
    glVertexAttribPointer(3, 1, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texIndex));
    #else
    glVertexAttribIPointer(3, 1, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)offsetof(Vertex, texIndex));
    #endif
    glEnableVertexAttribArray(3);

    glDrawArrays(primitiveType, 0, vertCount);
}

// Convenience wrappers for semantic clarity
void commandDrawQuad(const Vertex* vertices, const size_t vertCount){
    commandDraw(renderer->vao, renderer->vbo, vertices, vertCount, GL_TRIANGLES);
}

void commandDrawSimpleVertex(const Vertex* vertices, const size_t vertCount){
    commandDraw(renderer->simpleVao, renderer->simpleVbo, vertices, vertCount, GL_TRIANGLES);
}

void commandDrawLine(const Vertex* vertices, const size_t vertCount){
    commandDraw(renderer->lineVao, renderer->lineVbo, vertices, vertCount, GL_LINES);
}

void enableDepthTest(){
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

void disableDepthTest(){
    glDisable(GL_DEPTH_TEST);
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
    renderer->frameArena = initArena(MB(100));
    renderer->width = width;
    renderer->height = height;
    setViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
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
    renderer->shader = loadShader(arena, "shaders/quad-shader.vs", "shaders/quad-shader.fs");
    renderer->simpleShader = loadShader(arena, "shaders/simple-shader.vs", "shaders/simple-shader.fs");
    renderer->lineShader = loadShader(arena, "shaders/line-shader.vs", "shaders/line-shader.fs");
    LOGINFO("shader binded");
    renderer->activeShader = NULL;

    // Screen-space camera for UI: pixel-perfect (0,0) to (width, height)
    renderer->screenCamera = createCamera(0.0f, (float)renderer->width, 0.0f, (float)renderer->height);

    useShader(&renderer->shader);
    int samplers[MAX_TEXTURES_BIND];
    for(int i = 0; i < MAX_TEXTURES_BIND; i++){
        samplers[i] = i;
    }
    setUniform(&renderer->shader, "sprite", samplers, MAX_TEXTURES_BIND);

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
    setViewport(0, 0, renderTexture->texture.width, renderTexture->texture.height);

    //glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderStartBatch();
}

void beginShaderMode(Shader* shader){
    renderFlush();
    renderStartBatch();
    renderer->activeShader = shader;
    // Load shader on GPU so uniforms can be set
    //useShader(shader);
}

void beginDepthMode(){
    enableDepthTest();
}


void endShaderMode(){
    renderFlush();
    renderer->activeShader = NULL;
    //unBindShader();
    // Shader will be unloaded when next shader mode begins or when default shader is used
}

void endTextureMode(){
    renderFlush();  // Flush framebuffer draws
    unbindFrameBuffer();

    setViewport(0, 0, (uint32_t)renderer->screenCamera.width, (uint32_t)renderer->screenCamera.height);

    renderStartBatch();  // Start fresh batch for screen rendering
}

void endMode2D(){
    renderFlush();
    renderer->activeCamera = renderer->screenCamera;

    renderStartBatch();
}

void endDepthMode(){
    disableDepthTest();
}

void endScene(){
    renderFlush();
}

void renderStartBatch(){
    renderer->quadVertices = arenaAllocArrayZero(&renderer->frameArena, Vertex, MAX_VERTICES);
    renderer->lineVertices = arenaAllocArrayZero(&renderer->frameArena, Vertex, MAX_VERTICES_LINES);
    renderer->simpleVertex = arenaAllocArrayZero(&renderer->frameArena, Vertex, MAX_VERTICES);

    renderer->textures = arenaAllocArrayZero(&renderer->frameArena, const Texture*, MAX_TEXTURES_BIND);
    renderer->textures[0] = getTextureByName("default");
    renderer->textureCount = 1;
    renderer->quadVertexCount = 0;
    renderer->lineVertexCount = 0;
    renderer->simpleVertexCount = 0;
}

void executeCommandQueue(RenderCommand* commands){
    // Always set projection and view uniforms as they may change between frames
    useShader(commands->shader);
    setUniform(commands->shader, "projection", renderer->activeCamera.projection);
    setUniform(commands->shader, "view", renderer->activeCamera.view);


    switch(commands->type){
        case RenderCommandType::RENDER_QUAD: {
            for(size_t i = 0; i < renderer->textureCount; i++){
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, renderer->textures[i]->id);
            }
            commandDrawQuad(commands->vertexData, commands->vertexCount);
            break;
        }
        case RenderCommandType::RENDER_LINE: {
            commandDrawLine(commands->vertexData, commands->vertexCount);
            break;
        }
        case RenderCommandType::RENDER_TEXT: {
            commandDrawSimpleVertex(commands->vertexData, commands->vertexCount);
            break;
        }
    }
}

void renderFlush(){
    if(renderer->mode == RenderMode::NO_DEPTH){
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);  // Disable depth writes
    }
    RenderCommand commandQueue = {};
    // Render simple geometry first (filled rects, etc.)
    if(renderer->simpleVertexCount){
        commandQueue.type = RenderCommandType::RENDER_TEXT;
        if(renderer->activeShader){
            commandQueue.shader = renderer->activeShader;
        }else{
            commandQueue.shader = &renderer->simpleShader;
        }
        commandQueue.vertexData = renderer->simpleVertex;
        commandQueue.vertexCount = renderer->simpleVertexCount;
        executeCommandQueue(&commandQueue);
    }
    // Render quads second (includes text)
    if(renderer->quadVertexCount){
        commandQueue.type = RenderCommandType::RENDER_QUAD;
        if(renderer->activeShader){
            commandQueue.shader = renderer->activeShader;
        }else{
            commandQueue.shader = &renderer->shader;
        }
        commandQueue.vertexData = renderer->quadVertices;
        commandQueue.vertexCount = renderer->quadVertexCount;
        executeCommandQueue(&commandQueue);
    }
    if(renderer->lineVertexCount){
        commandQueue.type = RenderCommandType::RENDER_LINE;
        if(renderer->activeShader){
            commandQueue.shader = renderer->activeShader;
        }else{
            commandQueue.shader = &renderer->lineShader;
        }
        commandQueue.vertexData = renderer->lineVertices;
        commandQueue.vertexCount = renderer->lineVertexCount;
        executeCommandQueue(&commandQueue);
    }

    if(renderer->mode == RenderMode::NORMAL){
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
    }else{
        glDepthMask(GL_TRUE);  // Re-enable depth writes for next frame
    }

    renderer->textureCount = 1;
    renderer->quadVertexCount = 0;
    renderer->lineVertexCount = 0;
    renderer->simpleVertexCount = 0;
    clearArena(&renderer->frameArena);
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
    //Vertex vertices[vertSize];
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


    glm::vec3 pos = position;
    float layerZ = pos.z;  // user-defined
    float ySortZ = 0.0f;
    float zMin = layerZ;
    float zMax = layerZ + 1.0f;
    if (ySort) {
        // Use position.y + ySortOffset as the sort reference
        // ySortOffset allows specifying where the "foot" position is relative to the sprite center
        float sortY = position.y + ySortOffset;
        ySortZ = sortY * 0.001f;   // small scale factor
    }
    // Subtract ySortZ so higher Y positions (farther away) get lower Z values (render behind)
    pos.z = layerZ + ySortZ;
    pos.z = glm::clamp(pos.z, zMin, zMax);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    //glm::vec3 modelCenter(origin.x * size.x, origin.y * size.y, 0.0f);
    //model = glm::translate(model, modelCenter);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); //rotate x axis
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); //rotate y axis
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); //rotate z axis
    //model = glm::translate(model, -modelCenter);

    // Use size directly instead of scale
    model = glm::scale(model, glm::vec3(size.x, size.y, 1.0f));

    for(size_t i = 0; i < vertSize; i++){
        Vertex v = {};
        v.pos = model * vertexPosition[i];
        v.texCoord = textureCoords[i];
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
    glm::vec4 vertexPosition[] = {{p0.x, p0.y, layer, 1.0f},
                                  {p1.x, p1.y, layer, 1.0f}};

    const size_t vertSize = 2;

    Vertex vertices[vertSize];
    for(size_t i = 0; i < vertSize; i++){
        Vertex v = {};
        v.pos = vertexPosition[i];
        v.texCoord = {0.0f, 0.0f};  // Unused for lines
        v.color = verterxColor[i] * color;
        v.texIndex = 0;  // Unused for lines
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
            Vertex v = {};
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

    // UV coordinates for each vertex (0,0 at top-left, 1,1 at bottom-right)
    glm::vec2 uvCoords[] = {
                                {0.0f, 1.0f},  // Top-left
                                {1.0f, 0.0f},  // Bottom-right
                                {0.0f, 0.0f},  // Bottom-left
                                {0.0f, 1.0f},  // Top-left
                                {1.0f, 1.0f},  // Top-right
                                {1.0f, 0.0f}   // Bottom-right
                            };

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(position, layer));

    glm::vec3 modelCenter(0.5f * size.x, 0.5f * size.y, 0.0f);
    model = glm::translate(model, modelCenter);
    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f)); //rotate z axis
    model = glm::translate(model, -modelCenter);

    model = glm::scale(model, glm::vec3(size.x, size.y, 1.0f));

    for(size_t i = 0; i < vertSize; i++){
        Vertex v = {};
        v.pos = model * vertexPosition[i];
        v.texCoord = uvCoords[i];  // Provide UV coordinates for custom shaders
        v.color = color;
        v.texIndex = 0;
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
    // Clean up memory arenas
    clearArena(&renderer->frameArena);
    destroyArena(&renderer->frameArena);

    // Delete quad rendering resources
    deleteVertexArrayObject(renderer->vao);
    deleteVertexBuffer(renderer->vbo);

    // Delete line rendering resources
    deleteVertexArrayObject(renderer->lineVao);
    deleteVertexBuffer(renderer->lineVbo);

    // Delete simple/text rendering resources
    deleteVertexArrayObject(renderer->simpleVao);
    deleteVertexBuffer(renderer->simpleVbo);

    // Delete framebuffer resources
    deleteFrameBuffer(renderer->fbo);
    deleteRenderBuffer(renderer->rbo);

    // Delete shaders
    unloadShader(&renderer->shader);
    unloadShader(&renderer->simpleShader);
    unloadShader(&renderer->lineShader);

    // Note: Textures are managed by the TextureManager and should be destroyed separately
}

//------------------------------------------------------ Configuration API ------------------------------------------------------

void setRenderResolution(uint32_t width, uint32_t height){
    renderer->width = width;
    renderer->height = height;
    // Screen-space camera for UI: pixel-perfect (0,0) to (width, height)
    renderer->screenCamera = createCamera(0.0f, (float)width, 0.0f, (float)height);
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