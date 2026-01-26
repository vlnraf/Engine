#pragma once

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

#define MAX_QUADS 10000
#define MAX_VERTICES MAX_QUADS * 6
#define MAX_LINES 10000
#define MAX_VERTICES_LINES MAX_LINES * 2

#define MAX_TEXTURES_BIND 16

// Standard vertex structure used by ALL geometry types
// All fields must be present to match shader attribute layout
struct Vertex{
    glm::vec4 pos;        // location = 0
    glm::vec2 texCoord;   // location = 1 (set to 0,0 if unused)
    glm::vec4 color;      // location = 2
    uint8_t texIndex;     // location = 3 (set to 0 if unused)
};

enum RenderCommandType{
    RENDER_QUAD,
    RENDER_LINE,
    RENDER_TEXT,
    RENDER_MAX,
};

struct RenderCommand{
    RenderCommandType type;
    Shader* shader;
    Vertex* vertexData;
    size_t vertexCount;
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

    Shader* activeShader;

    RenderMode mode = NORMAL;

    Vertex* quadVertices;
    Vertex* simpleVertex;
    Vertex* lineVertices;

    const Texture** textures;
    uint16_t textureCount = 1;

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
void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);  // Internal: OpenGL-specific state management
CORE_API glm::vec2 getScreenSize();
CORE_API glm::vec2 getRenderSize();


//void setYsort(Renderer* renderer, bool flag);

// Low-level OpenGL resource creation
void genVertexArrayObject(uint32_t* vao);
void genVertexBuffer(uint32_t* vbo);
void genFrameBuffer(uint32_t* fbo);
void genRenderBuffer(uint32_t* rbo);
void genTexture(Texture* texture, uint32_t format, unsigned char* data);
void genRenderTexture(uint32_t* texture, uint32_t width, uint32_t height);

// Low-level OpenGL resource destruction
void deleteVertexArrayObject(uint32_t vao);
void deleteVertexBuffer(uint32_t vbo);
void deleteFrameBuffer(uint32_t fbo);
void deleteRenderBuffer(uint32_t rbo);
void deleteTexture(uint32_t texture);

// Low-level OpenGL binding/unbinding
void bindVertexArrayObject(uint32_t vao);
void bindVertexArrayBuffer(uint32_t vbo, const float* vertices, size_t vertCount);
void bindVertexArrayBuffer(uint32_t vbo, const Vertex* vertices, size_t vertCount);  // Single function for all vertex types
void bindFrameBuffer(uint32_t fbo);
void unbindFrameBuffer();
void bindRenderBuffer(uint32_t rbo);
void unbindRenderBuffer();

// Low-level drawing commands
void commandDrawQuad(const Vertex* vertices, const size_t vertCount);
void commandDrawLine(const Vertex* vertices, const size_t vertCount);
void commandDrawSimpleVertex(const Vertex* vertices, const size_t vertCount);

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
CORE_API void beginShaderMode(Shader* shader);
CORE_API void endShaderMode();
CORE_API void beginDepthMode();
CORE_API void endDepthMode();

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
