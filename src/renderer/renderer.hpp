#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "core/arena.hpp"
#include "core/coreapi.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "fontmanager.hpp"
#include "core/camera.hpp"
#include "core/ecs.hpp"

#define MAX_QUADS 100000
#define MAX_VERTICES MAX_QUADS * 6
#define MAX_LINES 100000
#define MAX_VERTICES_LINES MAX_LINES * 2

#define MAX_TEXTURES_BIND 16


struct UIVertex{
    glm::vec3 pos;
    glm::vec4 color;
};

struct QuadVertex{
    glm::vec4 pos;
    glm::vec4 color;
    glm::vec2 texCoord;
    uint8_t texIndex;
};

struct SimpleVertex{
    glm::vec3 pos;
    glm::vec4 color;
};

struct LineVertex{
    glm::vec3 pos;
    glm::vec4 color;
};

enum RenderMode{
    NORMAL,
    NO_DEPTH
};

struct Renderer{
    Arena frameArena;
    uint32_t vao, vbo, ebo;
    uint32_t lineVao, lineVbo, lineEbo;
    Shader shader;
    Shader lineShader;

    RenderMode mode = NORMAL;

    QuadVertex* quadVertices;
    LineVertex* lineVertices;

    Texture* textures;
    uint8_t textureIndex = 1;

    Font* defaultFont;

    //OrtographicCamera camera;
    OrtographicCamera screenCamera;
    OrtographicCamera activeCamera;
    OrtographicCamera camera[10];
    uint32_t cameraCount = 0;

    uint32_t drawCalls = 0;
    uint32_t quadVertexCount = 0;
    uint32_t lineVertexCount = 0;

    uint32_t width, height;
};

void initRenderer(Arena* arena, const uint32_t width, const uint32_t height);
CORE_API void setRenderResolution(uint32_t width, uint32_t height);
CORE_API void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
CORE_API glm::vec2 getScreenSize();
CORE_API glm::vec2 getRenderSize();

//void setYsort(Renderer* renderer, bool flag);
void genVertexArrayObject(uint32_t* vao);
void genVertexBuffer(uint32_t* vbo);
void bindVertexArrayObject(uint32_t vao);
void bindVertexArrayBuffer(uint32_t vbo, const float* vertices, size_t vertCount);
void bindVertexArrayBuffer(uint32_t vbo, const QuadVertex* vertices, size_t vertCount);
void bindVertexArrayBuffer(uint32_t vbo, const LineVertex* vertices, size_t vertCount);
void bindVertexArrayBuffer(uint32_t vbo, const UIVertex* vertices, size_t vertCount);
void setShader(Renderer* renderer, const Shader shader);
void commandDrawQuad(const QuadVertex* vertices, const size_t vertCount);
void commandDrawLine(const LineVertex* vertices, const size_t vertCount);
void commandDrawQuad(const SimpleVertex* vertices, const size_t vertCount);
void commandDrawQuad(const UIVertex* vertices, const size_t vertCount);

glm::vec4 calculateUV(const Texture* texture, glm::vec2 index, glm::vec2 size, glm::vec2 offset);


//------------------HIGH LEVEL RENDERER-----------------------------
//void renderDrawQuad(Renderer* renderer, OrtographicCamera camera, glm::vec3 position, const glm::vec3 scale, const glm::vec3 rotation, const Texture* texture);
CORE_API void clearColor(float r, float g, float b, float a);

CORE_API Texture beginTextureMode(uint32_t width, uint32_t height);
CORE_API void endTextureMode();

CORE_API void renderDrawQuad(glm::vec3 position, const glm::vec3 scale, const glm::vec3 rotation, const Texture* texture, glm::vec2 index, glm::vec2 spriteSize, bool ySort);
CORE_API void renderDrawQuadPro(glm::vec3 position, const glm::vec3 scale, const glm::vec3 rotation, const glm::vec2 origin, const Texture* texture, glm::vec4 color, glm::vec2 index, glm::vec2 spriteSize, bool ySort);
CORE_API void renderDrawSprite(glm::vec3 position, const glm::vec3 scale, const glm::vec3 rotation, const SpriteComponent* sprite);
CORE_API void renderDrawLine(const glm::vec2 p0, const glm::vec2 p1, const glm::vec4 color, const float layer);
//CORE_API void renderDrawText(Font* font, OrtographicCamera camera, const char* text, float x, float y, float scale);
CORE_API void renderDrawText3D(Font* font, const char* text, glm::vec3 pos, float scale);
CORE_API void renderDrawRect(const glm::vec2 offset, const glm::vec2 size, const glm::vec4 color, const float layer);
//void renderDrawFilledRect(Renderer* renderer, const glm::vec2 position, const glm::vec2 size);
CORE_API void beginScene(RenderMode mode = NORMAL);
//CORE_API void beginScene(OrtographicCamera camera, RenderMode mode);
CORE_API void beginMode2D(OrtographicCamera camera);
CORE_API void endMode2D();
CORE_API void endScene();
CORE_API void destroyRenderer();
//CORE_API void beginUIRender(glm::vec2 pos, glm::vec2 size);
//CORE_API void endUIRender();


CORE_API void renderDrawText2D(Font* font, const char* text, glm::vec2 pos, float scale);
CORE_API void renderDrawFilledRect(const glm::vec2 position, const glm::vec2 size, const glm::vec2 rotation, const glm::vec4 color);
CORE_API void renderDrawFilledRectPro(const glm::vec2 position, const glm::vec2 size, const glm::vec2 rotation, const glm::vec2 origin, const glm::vec4 color);
CORE_API void renderDrawQuad2D(const Texture* texture, glm::vec2 position, const glm::vec2 scale, const glm::vec2 rotation, glm::vec2 index, glm::vec2 textureSize);

// UI Anchor helpers (for bottom-left origin coordinate system)
CORE_API glm::vec2 anchorTopLeft(float x, float y);
CORE_API glm::vec2 anchorTopRight(float x, float y);
CORE_API glm::vec2 anchorBottomLeft(float x, float y);
CORE_API glm::vec2 anchorBottomRight(float x, float y);
CORE_API glm::vec2 anchorCenter(float x, float y);