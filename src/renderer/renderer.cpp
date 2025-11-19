#include "renderer.hpp"
#include "fontmanager.hpp"
#include "core/tracelog.hpp"
#include <glm/gtx/string_cast.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H  

Renderer* renderer;

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
    LOGINFO("buffer binded");

    //TODO: change to arena implementation
    renderer->shader = createShader("shaders/quad-shader.vs", "shaders/quad-shader.fs");
    renderer->lineShader = createShader("shaders/line-shader.vs", "shaders/line-shader.fs");
    LOGINFO("shader binded");


    //renderer->defaultFont = getFont("Minecraft");
    renderer->screenCamera = createCamera({0,0,0}, width, height);
    renderer->activeCamera = renderer->screenCamera;
    //renderer->camera = renderer->screenCamera;
    renderer->cameraCount = 0;

    LOGINFO("init renderer finished");
}

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

void generateTexture(uint32_t* texture, uint32_t width, uint32_t height){
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

void bindVertexArrayBuffer(uint32_t vbo, const QuadVertex* vertices, size_t vertCount){ //std::vector<float> vertices){
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVertex) * vertCount, vertices, GL_STATIC_DRAW);
}

void setShader(Renderer* renderer, const Shader shader){
    renderer->shader = shader;
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

void renderStartBatch();
void renderFlush();

void beginScene(RenderMode mode){
    renderer->mode = mode;
    //renderer->camera = renderer->screenCamera;
    renderer->activeCamera = renderer->screenCamera;
    renderStartBatch();
}

void beginMode2D(OrtographicCamera camera){
    renderFlush();
    //renderer->camera = camera;
    renderer->camera[renderer->cameraCount++] = camera;
    renderer->activeCamera = camera;
    renderStartBatch();
}

Texture beginTextureMode(uint32_t width, uint32_t height){
    renderFlush();  // Flush any pending draws

    // Save current camera state

    uint32_t fbo;
    genFrameBuffer(&fbo);
    bindFrameBuffer(fbo);
    uint32_t texture;
    generateTexture(&texture, width, height);
    attachFrameBuffer(texture);
    uint32_t rbo;
    genRenderBuffer(&rbo);
    bindRenderBuffer(rbo);
    attachRenderBuffer(rbo, width, height);

    // Check framebuffer is complete
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        LOGERROR("Framebuffer is not complete!");
    }

    // Set viewport for framebuffer
    glViewport(0, 0, width, height);

    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Create a flipped camera for framebuffer rendering
    // This makes the framebuffer render right-side up
    //OrtographicCamera fbCamera = createCamera({0,0,0}, (float)width, (float)height);
    //fbCamera.projection = glm::ortho(0.0f, (float)width, (float)height, 0.0f, -100.0f, 100.0f); // Flipped Y
    //renderer->activeCamera = fbCamera;

    renderStartBatch();

    Texture t = {};
    t.id = texture;
    t.width = (int)width;
    t.height = (int)height;
    t.nrChannels = 4;
    t.size = {(float)width, (float)height};
    return t;
}

void endTextureMode(){
    renderFlush();  // Flush framebuffer draws
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Restore previous camera and viewport
    glViewport(0, 0, (uint32_t)renderer->width, (uint32_t)renderer->height);

    renderStartBatch();  // Start fresh batch for screen rendering
}

void endMode2D(){
    renderFlush();
    renderer->cameraCount--;

    // Clear depth buffer when returning to screen space
    // This ensures UI renders on top of world geometry
    if(renderer->cameraCount == 0){
        glClear(GL_DEPTH_BUFFER_BIT);
        renderer->activeCamera = renderer->screenCamera;
    }else{
        renderer->activeCamera = renderer->camera[renderer->cameraCount-1];
    }

    renderStartBatch();
}

//void beginScene(OrtographicCamera camera, RenderMode mode){
//    renderer->mode = mode;
//    renderer->camera = camera;
//    renderStartBatch();
//}

void endScene(){
    renderFlush();
}

void renderStartBatch(){
    clearArena(&renderer->frameArena);
    renderer->quadVertices = arenaAllocArrayZero(&renderer->frameArena, QuadVertex, MAX_QUADS);
    renderer->lineVertices = arenaAllocArrayZero(&renderer->frameArena, LineVertex, MAX_LINES);

    renderer->textures = arenaAllocArrayZero(&renderer->frameArena, Texture, MAX_TEXTURES_BIND);
    renderer->textures[0] = *getTexture("default");
    renderer->textureIndex = 1;
    renderer->quadVertexCount = 0;
    renderer->lineVertexCount = 0;
}

void renderFlush(){
    if(renderer->mode == RenderMode::NO_DEPTH){
        glDisable(GL_DEPTH_TEST);
    }
    if(renderer->quadVertexCount){
        useShader(&renderer->shader);
        setUniform(&renderer->shader, "projection", renderer->activeCamera.projection);
        setUniform(&renderer->shader, "view", renderer->activeCamera.view);
        for(size_t i = 0; i < renderer->textureIndex; i++){
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, renderer->textures[i].id);
        }
        commandDrawQuad(renderer->quadVertices, renderer->quadVertexCount);
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
void renderDrawQuadPro(glm::vec3 position, const glm::vec3 scale, const glm::vec3 rotation, const glm::vec2 origin, const Texture* texture,
                    glm::vec4 color, glm::vec2 index, glm::vec2 spriteSize, bool ySort){
    
    OrtographicCamera cam = renderer->activeCamera;
    uint8_t textureIndex = 0;

    if(renderer->quadVertexCount >= MAX_VERTICES){
        renderFlush();
        renderStartBatch();
    }

    for(size_t i = 1; i < renderer->textureIndex; i++){
        if(renderer->textures[i].id == texture->id){
            textureIndex = i;
            break;
        }
    }
    if(textureIndex == 0){
        if(renderer->textureIndex >= MAX_TEXTURES_BIND){
            renderFlush();
            renderStartBatch();
        }
        renderer->textures[renderer->textureIndex] = *texture;
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

    if(ySort){
        position.z = position.z + (1.0f - (position.y / (cam.position.y + cam.height))); 
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

void renderDrawQuad(glm::vec3 position, const glm::vec3 scale, const glm::vec3 rotation, const Texture* texture,
                    glm::vec2 index, glm::vec2 spriteSize, bool ySort){
    renderDrawQuadPro(position, scale, rotation, {0,0}, texture, {1,1,1,1}, index, spriteSize, ySort);
}

void renderDrawSprite(glm::vec3 position, const glm::vec3 scale, const glm::vec3 rotation, const SpriteComponent* sprite){
    OrtographicCamera cam = renderer->activeCamera;
    if(renderer->quadVertexCount >= MAX_VERTICES){
        renderFlush();
        renderStartBatch();
    }

    uint8_t textureIndex = 0;

    for(size_t i = 1; i < renderer->textureIndex; i++){
        if(renderer->textures[i].id == sprite->texture->id){
            textureIndex = i;
            break;
        }
    }
    if(textureIndex == 0){
        if(renderer->textureIndex >= MAX_TEXTURES_BIND){
            renderFlush();
            renderStartBatch();
        }
        renderer->textures[renderer->textureIndex] = *sprite->texture;
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

    //Bot left origin
    //glm::vec4 vertexPosition[] = {{0.0f, 1.0f, 0.0f, 1.0f},
    //                              {1.0f, 0.0f, 0.0f, 1.0f},
    //                              {0.0f, 0.0f, 0.0f, 1.0f}, 
    //                              {0.0f, 1.0f, 0.0f, 1.0f},
    //                              {1.0f, 1.0f, 0.0f, 1.0f},
    //                              {1.0f, 0.0f, 0.0f, 1.0f}};

    //Center origin
    glm::vec4 vertexPosition[] = {{-0.5f,  0.5f,  0.0f, 1.0f},
                                  {0.5f,  -0.5f,  0.0f, 1.0f},
                                  {-0.5f, -0.5f,  0.0f, 1.0f}, 
                                  {-0.5f,  0.5f,  0.0f, 1.0f},
                                  {0.5f,   0.5f,  0.0f, 1.0f},
                                  {0.5f,  -0.5f,  0.0f, 1.0f}};
                        
    //NOTE: y sort based on layer and y position of the quad
    //I normalize it to don't let layers explode and generate high numbers
    //NOTE: what happens if we render in negative space?? the normalization goes wrong, should we take the absolute values????
    if(sprite->ySort){
        float camBottom = cam.position.y;
        float camTop = camBottom + cam.height;

        position.z = sprite->layer + (1.0f - ((position.y - camBottom) / (camTop - camBottom))); 
    }else{
        position.z = sprite->layer;
    }

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, position);

    glm::vec3 modelCenter(0,0,0);
    if(sprite->pivot == SpriteComponent::PIVOT_CENTER){
        modelCenter = glm::vec3(0.5f * sprite->size.x, 0.5f * sprite->size.y, 0.0f);
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

    for(size_t i = 0; i < vertSize; i++){
        QuadVertex v = {};
        v.pos = model * vertexPosition[i];
        v.texCoord = textureCoords[i];
        v.color = sprite->color;
        v.texIndex = textureIndex;
        renderer->quadVertices[renderer->quadVertexCount] = v;
        renderer->quadVertexCount += 1;
    }

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
void renderDrawText3D(Font* font, const char* text, glm::vec3 pos, float scale){
    if(renderer->quadVertexCount >= MAX_VERTICES){
        renderFlush();
        renderStartBatch();
    }

    uint8_t textureIndex = 0;
    Texture* texture = getTexture(font->textureIdx);

    for(size_t i = 1; i < renderer->textureIndex; i++){
        if(renderer->textures[i].id == texture->id){
            textureIndex = i;
            break;
        }
    }

    if(textureIndex == 0){
        if(renderer->textureIndex >= MAX_TEXTURES_BIND){
            renderFlush();
            renderStartBatch();
        }
        renderer->textures[renderer->textureIndex] = *texture;
        textureIndex = renderer->textureIndex;
        renderer->textureIndex++;
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
        glm::vec4 uv = calculateUV(texture, {0,0}, {ch.Size.x, ch.Size.y},{ch.xOffset, 0}); //index is always 0 because the character size change and so we can't rely on index
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
            v.color = glm::vec4(1,1,1,1);
            v.texIndex = textureIndex;
            renderer->quadVertices[renderer->quadVertexCount] = v;
            renderer->quadVertexCount += 1;
        }
        pos.x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
}


//------------------------------------------------------ UI methods ------------------------------------------------------
void renderDrawFilledRect(const glm::vec2 position, const glm::vec2 size, const glm::vec2 rotation, const glm::vec4 color){
    if(renderer->quadVertexCount >= MAX_VERTICES){
        renderFlush();
        renderStartBatch();
    }
    Texture* texture = getTexture("default");
    renderDrawQuadPro({position, 0}, {size, 1}, {rotation, 0}, {0,0}, texture, color, {0,0}, {texture->width, texture->height}, false);
}

void renderDrawFilledRectPro(const glm::vec2 position, const glm::vec2 size, const glm::vec2 rotation, const glm::vec2 origin, const glm::vec4 color){
    if(renderer->quadVertexCount >= MAX_VERTICES){
        renderFlush();
        renderStartBatch();
    }
    Texture* texture = getTexture("default");
    renderDrawQuadPro({position, 0}, {size, 1}, {rotation, 0}, origin, texture, color, {0,0}, {texture->width, texture->height}, false);
}

void renderDrawText2D(Font* font, const char* text, glm::vec2 pos, float scale){
    renderDrawText3D(font, text, {pos, 0.0f}, scale);
}

void renderDrawQuad2D(const Texture* texture, glm::vec2 position, const glm::vec2 scale, const glm::vec2 rotation,glm::vec2 index, glm::vec2 textureSize){
    renderDrawQuad({position, 0}, {scale, 1}, {rotation, 0}, texture, index, textureSize, false);
}

void destroyRenderer(){
    clearArena(&renderer->frameArena);
    destroyArena(&renderer->frameArena);
}

//------------------------------------------------------ Configuration API ------------------------------------------------------

void setRenderResolution(uint32_t width, uint32_t height){
    renderer->width = width;
    renderer->height = height;
    //renderer->screenCamera = createCamera({0,0,0}, width, height);
    // Note: viewport is separate - allows rendering at one resolution, displaying at another
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