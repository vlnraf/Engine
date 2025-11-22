#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "core/arena.hpp"
#include "core/coreapi.hpp"
#include "core/types.hpp"
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
    glm::vec4 pos;
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
    uint32_t simpleVao, simpleVbo, simpleEbo;
    uint32_t fbo, rbo; // Framebuffer and renderbuffer for render-to-texture
    Shader shader;
    Shader simpleShader;
    Shader lineShader;

    RenderMode mode = NORMAL;

    QuadVertex* quadVertices;
    SimpleVertex* simpleVertex;
    LineVertex* lineVertices;

    const Texture** textures;
    uint8_t textureCount = 1;

    Font* defaultFont;

    OrtographicCamera screenCamera;
    OrtographicCamera activeCamera;

    uint32_t drawCalls = 0;
    uint32_t quadVertexCount = 0;
    uint32_t lineVertexCount = 0;
    uint32_t simpleVertexCount = 0;

    int width, height;
};

void initRenderer(Arena* arena, const uint32_t width, const uint32_t height);
void destroyRenderer();
CORE_API void setRenderResolution(uint32_t width, uint32_t height);
CORE_API void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
CORE_API glm::vec2 getScreenSize();
CORE_API glm::vec2 getRenderSize();


//void setYsort(Renderer* renderer, bool flag);
void genVertexArrayObject(uint32_t* vao);
void genVertexBuffer(uint32_t* vbo);
void genFrameBuffer(uint32_t* fbo);
void genRenderBuffer(uint32_t* rbo);
void genTexture(uint32_t* texture, uint32_t width, uint32_t height);
void bindVertexArrayObject(uint32_t vao);
void bindVertexArrayBuffer(uint32_t vbo, const float* vertices, size_t vertCount);
void bindVertexArrayBuffer(uint32_t vbo, const QuadVertex* vertices, size_t vertCount);
void bindVertexArrayBuffer(uint32_t vbo, const LineVertex* vertices, size_t vertCount);
void bindVertexArrayBuffer(uint32_t vbo, const UIVertex* vertices, size_t vertCount);
void commandDrawQuad(const QuadVertex* vertices, const size_t vertCount);
void commandDrawLine(const LineVertex* vertices, const size_t vertCount);
void commandDrawQuad(const SimpleVertex* vertices, const size_t vertCount);
void commandDrawQuad(const UIVertex* vertices, const size_t vertCount);
void setShader(Renderer* renderer, const Shader shader);

glm::vec4 calculateUV(const Texture* texture, glm::vec2 index, glm::vec2 size, glm::vec2 offset);


//------------------HIGH LEVEL RENDERER-----------------------------
CORE_API void clearColor(float r, float g, float b, float a);

CORE_API void beginTextureMode(RenderTexture* texture);
CORE_API void endTextureMode();
CORE_API void beginScene(RenderMode mode = NORMAL);
CORE_API void beginMode2D(OrtographicCamera camera);
CORE_API void endMode2D();
CORE_API void endScene();

// 3D Quad Drawing
// Note: position.z is the base layer, ySort dynamically adjusts it based on Y position for depth sorting
CORE_API void renderDrawQuad(glm::vec3 position, const glm::vec2 size, float rotation, const Texture* texture, glm::vec4 color, bool ySort = false); // Simple: whole texture with tint
CORE_API void renderDrawQuadEx(glm::vec3 position, const glm::vec2 size, const glm::vec3 rotation, const Texture* texture, const Rect sourceRect, glm::vec4 color, bool ySort = false); // Extended: atlas region + color tint
CORE_API void renderDrawQuadPro(glm::vec3 position, const glm::vec2 size, const glm::vec3 rotation, const Rect sourceRect, const glm::vec2 origin, const Texture* texture, glm::vec4 color, bool ySort, float ySortOffset = 0.0f); // Pro: full control with origin and y-sort offset
CORE_API void renderDrawText3D(Font* font, const char* text, glm::vec3 pos, float scale, glm::vec4 color = {1,1,1,1});

// 2D/UI Drawing - Primitives
CORE_API void renderDrawLine(const glm::vec2 p0, const glm::vec2 p1, const glm::vec4 color, const float layer = 0.0f);
CORE_API void renderDrawRect(const glm::vec2 offset, const glm::vec2 size, const glm::vec4 color, const float layer = 0.0f);
CORE_API void renderDrawFilledRect(const glm::vec2 position, const glm::vec2 size, float rotation, const glm::vec4 color, const float layer = 0.0f);
CORE_API void renderDrawFilledRectPro(const glm::vec2 position, const glm::vec2 size, float rotation, const glm::vec2 origin, const glm::vec4 color, const float layer = 0.0f);
CORE_API void renderDrawQuad2D(glm::vec2 position, const glm::vec2 size, float rotation, const Texture* texture, glm::vec4 color = {1,1,1,1}); // Simple: whole texture with tint
CORE_API void renderDrawQuadEx2D(glm::vec2 position, const glm::vec2 size, float rotation, const Texture* texture, const Rect sourceRect, glm::vec4 color = {1,1,1,1}); // Extended: atlas region + color tint
CORE_API void renderDrawQuadPro2D(glm::vec2 position, const glm::vec2 size, float rotation, const Rect sourceRect, const glm::vec2 origin, const Texture* texture, glm::vec4 color = {1,1,1,1}); // Pro: full control with origin
CORE_API void renderDrawText2D(Font* font, const char* text, glm::vec2 pos, float scale, glm::vec4 color = {1,1,1,1});

// UI Anchor helpers (for bottom-left origin coordinate system)
CORE_API glm::vec2 anchorTopLeft(float x, float y);
CORE_API glm::vec2 anchorTopRight(float x, float y);
CORE_API glm::vec2 anchorBottomLeft(float x, float y);
CORE_API glm::vec2 anchorBottomRight(float x, float y);
CORE_API glm::vec2 anchorCenter(float x, float y);
