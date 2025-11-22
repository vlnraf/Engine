# Renderer Architecture Design Notes

**Date:** 2025-11-18
**Discussion:** Architecture review and improvement priorities

## Current Architecture

### Two-Tier API Design

The renderer intentionally separates **low-level** and **high-level** APIs:

**Low-Level API (Engine Internal)**
- `commandDrawQuad()` - Direct GL commands
- `bindVertexArrayObject()` - Raw GL wrapper
- `genVertexBuffer()` - Buffer management
- Direct access to GL state

**High-Level API (Game/User Facing)**
- `renderDrawSprite()` - Easy sprite rendering
- `renderDrawText2D()` - Simple text rendering
- `renderDrawLine()` - Primitive drawing
- `renderDrawRect()` - Rectangle rendering

**Naming Convention:** The `render` prefix distinguishes the public rendering API from other engine systems.

### Architecture Layers

```
┌─────────────────────────────────┐
│   Game Layer (game.dll)         │  <- Uses HIGH-LEVEL API
│   renderDrawSprite()             │
│   renderDrawText2D()             │
└─────────────────────────────────┘
           ↓
┌─────────────────────────────────┐
│   Core Engine (core.dll)         │  <- Can use BOTH
│   Renderer System                │
│   - High-level: renderDraw*()    │
│   - Low-level: commandDraw*()    │
└─────────────────────────────────┘
           ↓
┌─────────────────────────────────┐
│   Platform/OpenGL                │
└─────────────────────────────────┘
```

### Current Strengths

✅ **Batched Rendering** - Automatic quad/line batching with flush on overflow
✅ **Multi-texture Support** - Up to 16 textures per batch (MAX_TEXTURES_BIND)
✅ **Arena Allocation** - Frame arena cleared each scene, no malloc/free
✅ **Sprite Features** - UV calculation, flipping (flipX/flipY), tile-based sprites
✅ **Y-Sorting** - Layer-based depth with Y-position normalization
✅ **Scene Management** - beginScene/endScene pattern
✅ **Global State** - Single renderer instance (similar to Raylib approach)

### Current Implementation Details

**File:** `src/renderer/renderer.cpp`, `src/renderer/renderer.hpp`

**Key Constants:**
- `MAX_QUADS: 100000` (600k vertices max)
- `MAX_LINES: 100000` (200k vertices max)
- `MAX_TEXTURES_BIND: 16` (texture slots per batch)

**Rendering Flow:**
1. `beginScene(camera, mode)` - Sets camera, starts batch, clears frame arena
2. `renderDrawSprite/Quad/Line()` - Adds vertices to batch
3. Auto-flush when `quadVertexCount >= MAX_VERTICES` or texture limit hit
4. `endScene()` - Final flush, submits all batched geometry

**Vertex Types:**
- `QuadVertex` - pos(vec4), color(vec4), texCoord(vec2), texIndex(uint8)
- `LineVertex` - pos(vec3), color(vec4)

## Known Limitations

### 1. Camera Rigidity (CRITICAL)

**Problem:** Only one camera per begin/end scene pair.

**Current Behavior:**
```cpp
beginScene(worldCamera, NORMAL);
  renderDrawSprite(player);
endScene();  // FLUSH #1

beginScene(uiCamera, NO_DEPTH);
  renderDrawText2D("HP: 100");
endScene();  // FLUSH #2

// Can't go back to worldCamera without another flush!
```

**Result:** Multiple draw calls when one batch could handle everything.

**Impact:**
- Performance: Extra draw calls
- Flexibility: Can't interleave cameras (world → UI → world → UI)
- Batching: Breaks optimal batch ordering

### 2. Layer System Mixed Concerns

**Current Y-Sort Implementation** (renderer.cpp:374-381):
```cpp
if(sprite->ySort){
    position.z = sprite->layer + (1.0f - ((position.y - camBottom) / (camTop - camBottom)));
}
```

**Problem:** Mixes explicit layers with automatic Y-sorting.

**Limitations:**
- Can't have UI always on top without high layer values
- Particles can't easily slot between world layers
- Background layer management is manual

### 3. Missing Engine Features

**Not Implemented:**
- Render targets/FBOs (can't render to texture)
- Scissor/clipping regions (UI needs this)
- Render statistics (only tracks `drawCalls`)
- Shader parameter passing (can't do runtime effects)
- Custom blend modes (stuck with alpha blending)
- Stencil operations (no masking)

**Code Cleanup Needed:**
- Lines 24-31, 37-40, 46-50: Commented-out VAO/shader code
- Decision needed: Remove or implement?

## Improvement Priorities

### HIGH PRIORITY (Functionality)

#### 1. Camera Switching Implementation ✅ COMPLETED

**Initial Proposal:** Camera stack to avoid flushes when switching cameras.

**Final Decision (2025-11-20):** Camera stack is NOT needed. Flush-on-switch is correct and unavoidable.

**Reasoning:**
- Different cameras require different view/projection uniforms (set per-draw-call)
- Cannot batch vertices with different transformations in a single draw call
- All engines flush per-camera (Unity, Unreal, Raylib follow the same pattern)
- Camera stack would add complexity without reducing draw calls
- The only way to avoid flushes would be a deferred render graph (overkill for this engine)

**Implemented API:**
```cpp
beginScene(NO_DEPTH);              // Screen-space camera active
  renderUI();                      // Batched UI rendering

  beginMode2D(worldCamera);        // FLUSH - switch to world camera
    renderWorld();                 // Batched world rendering
  endMode2D();                     // FLUSH - back to screen camera

  renderMoreUI();                  // More batched UI
endScene();                        // Final flush
```

**Implementation:** `beginMode2D()` / `endMode2D()` (renderer.cpp:215-262)
- `beginMode2D()` flushes current batch, switches camera, starts new batch
- `endMode2D()` flushes world batch, restores screen camera, starts new batch

**Result:** Clean, simple API. Each camera gets optimal batching within its own render pass.

**Future Optimization:** Texture-based sorting within each camera's batch (not camera-stack related)

#### 2. Render Target Support (FBOs) ✅ COMPLETED

**Status:** Implemented (2025-11-20)

**Implemented API:**
```cpp
RenderTexture rt = loadRenderTexture(width, height);
beginTextureMode(&rt);     // Switch to FBO, flush pending draws
  renderDrawSprite(...);   // Renders to texture
endTextureMode();          // Restore default framebuffer

// Use the result
renderDrawQuad2D(&rt.texture, position, scale, ...);
```

**Implementation:** `beginTextureMode()` / `endTextureMode()` (renderer.cpp:221-255)
- Creates FBO with color attachment and depth/stencil RBO
- Flushes before switching framebuffer targets
- Adjusts viewport for render texture dimensions
- Restores screen framebuffer and viewport on end

**Use Cases:**
- Post-processing effects
- Minimaps/render-to-texture
- Off-screen rendering
- Pixel-perfect upscaling (render low-res, display high-res)

**Note:** Currently creates FBO per-call. Could be optimized to cache FBOs if needed.

#### 3. Improved Layer System

**Goal:** Separate Y-sorting from explicit layer control.

**Proposed Approach:**
```cpp
// Option A: Explicit layer parameter
renderDrawSprite(pos, scale, rot, sprite, layer);

// Option B: Layer state management
setRenderLayer(LAYER_BACKGROUND);  // 0-100
renderDrawSprite(...);
setRenderLayer(LAYER_WORLD);       // 100-200
renderDrawSprite(...);
setRenderLayer(LAYER_UI);          // 200-300
renderDrawSprite(...);

// Option C: Layer constants
#define LAYER_BACKGROUND -100.0f
#define LAYER_WORLD      0.0f
#define LAYER_PARTICLES  5.0f
#define LAYER_UI         1000.0f
```

**Y-Sort Refinement:**
- Only apply within a layer range
- UI should NEVER Y-sort
- Particles might need different sorting

#### 4. Render Statistics (Engine Profiling)

**Current:** Only tracks `drawCalls` (renderer.cpp:295, 302, 309)

**Proposed:**
```cpp
struct RenderStats {
    uint32_t drawCalls;
    uint32_t verticesSubmitted;
    uint32_t batchFlushes;
    uint32_t textureBinds;
    uint32_t spritesDrawn;
    uint32_t linesDrawn;
    uint32_t cameraSwitches;
};

CORE_API RenderStats getRenderStats();
CORE_API void resetRenderStats();
```

**Use Cases:**
- Performance profiling
- Optimization validation
- Debug overlays showing renderer load

### MEDIUM PRIORITY (API Improvements)

#### 5. Separate Geometry Building (Low-Level API)

**Current:** Vertex building and batching are mixed (renderer.cpp:383-411)

**Proposed Low-Level API:**
```cpp
// Build geometry separately
QuadGeometry buildSpriteQuad(const SpriteComponent* sprite, Transform transform);
LineGeometry buildLine(vec2 p0, vec2 p1);

// Submit to batch
void submitQuad(const QuadGeometry* quad, uint8_t textureIndex);
void submitLine(const LineGeometry* line);

// High-level wrapper (unchanged for users)
void renderDrawSprite(...) {
    QuadGeometry quad = buildSpriteQuad(sprite, transform);
    submitQuad(&quad, getTextureIndex(sprite->texture));
}
```

**Benefits:**
- Engine can modify geometry before submission
- Enables instancing (build once, submit many)
- Better for custom effects
- Cleaner separation of concerns

#### 6. Scissor/Clipping Regions

**Need:** UI rendering requires clipping (scrollable areas, panels)

**Proposed API:**
```cpp
CORE_API void beginScissor(int x, int y, int width, int height);
CORE_API void endScissor();

// Usage:
beginScissor(panelX, panelY, panelW, panelH);
  renderDrawText2D(...);  // Clipped to panel
endScissor();
```

**Implementation:** Manage GL scissor state, potentially flush on state change.

#### 7. Custom Shader Support

**Need:** Runtime shader switching for effects (grayscale, bloom, custom materials)

**Status:** Not yet implemented

**Proposed API:**
```cpp
CORE_API void beginShader(Shader* customShader);  // FLUSH - switch shader
CORE_API void endShader();                        // FLUSH - back to default

// Usage:
beginShader(&grayscaleShader);
  renderDrawSprite(...);  // Rendered with custom shader
endShader();
```

**Implementation Notes:**
- Like camera switching, shader changes require flushing (different GL program)
- Would need shader parameter passing API (uniforms)
- Should support texture slots just like default shader

### LOW PRIORITY (Polish)

#### 8. Code Cleanup

**Actions:**
- Remove or implement commented VAO/shader code (lines 24-31, 37-40, 46-50)
- Move TODOs to tracking system
- Document deprecated functions (renderDrawQuadPro usage)

#### 9. OpenGL Resource Cleanup (CRITICAL)

**Status:** Missing from `destroyRenderer()` (renderer.cpp:688-691)

**Problem:** VAOs, VBOs, and shaders are never cleaned up, causing resource leaks on shutdown.

**Required Implementation:**
```cpp
void destroyRenderer(){
    // Delete OpenGL resources
    glDeleteVertexArrays(1, &renderer->vao);
    glDeleteBuffers(1, &renderer->vbo);
    glDeleteVertexArrays(1, &renderer->lineVao);
    glDeleteBuffers(1, &renderer->lineVbo);
    glDeleteVertexArrays(1, &renderer->simpleVao);
    glDeleteBuffers(1, &renderer->simpleVbo);

    deleteShader(&renderer->shader);
    deleteShader(&renderer->simpleShader);
    deleteShader(&renderer->lineShader);

    // Arena cleanup
    clearArena(&renderer->frameArena);
    destroyArena(&renderer->frameArena);
}
```

**Impact:** Similar to the FMOD audio bug - missing cleanup can cause slow shutdown or resource leaks.

#### 10. API Documentation

**Add Comments:**
- Clarify which functions are low-level vs high-level
- Document expected usage patterns
- Add examples for complex features

#### 11. Shader Parameter Passing

**Need:** Runtime effects (color matrices, custom uniforms)

**Proposed:**
```cpp
CORE_API void setShaderParam(const char* name, float value);
CORE_API void setShaderParam(const char* name, vec4 value);

// Usage for grayscale effect:
setShaderParam("grayscale", 1.0f);
renderDrawSprite(...);  // Rendered in grayscale
```

## Design Decisions

### Why Global State Is Fine

**Question Raised:** Is global `Renderer* renderer` bad practice?

**Answer:** No. Raylib uses global state (`static rlglData RLGL`). For a single-window engine, this is pragmatic.

**Alternatives Considered:**
- Singleton pattern: Just global with extra steps, no benefit
- Explicit context passing: More verbose API, no real gain for single renderer

**Decision:** Keep global state. Focus on API ergonomics instead.

### Why Two-Tier API Matters

**Low-Level API** allows engine code to:
- Build custom rendering pipelines
- Optimize specific cases
- Access GL state directly when needed

**High-Level API** gives game developers:
- Simple, Raylib-like interface
- No OpenGL knowledge required
- Sensible defaults

**Both tiers can coexist** - advanced users can drop down, beginners stay high-level.

### Transform Calculation Location

**Current:** CPU-side transform in `renderDrawSprite()` (lines 383-400)

**Question:** Should GPU handle transforms?

**Analysis:**
- **CPU transforms:** Simpler batching, easier debugging, works with current architecture
- **GPU transforms:** Better for hierarchical transforms, less CPU work

**Current Decision:** Keep CPU transforms for now. The ECS already handles parent/child relationships, so this works. Could optimize later with instancing.

## Implementation Notes

### Texture Batching Logic

**How it works** (renderDrawSprite, lines 315-331):
1. Check if texture already bound in `renderer->textures[]`
2. If found, reuse existing index
3. If not found and slots available, bind to next slot
4. If no slots available, flush and start new batch

**Optimization Opportunity:** Sort sprites by texture before submitting to reduce flushes.

### Y-Sort Z-Calculation

**Formula** (line 378):
```cpp
position.z = sprite->layer + (1.0f - ((position.y - camBottom) / (camTop - camBottom)));
```

**Explanation:**
- `sprite->layer` = base layer (integer)
- Fraction part = normalized Y position (0.0 to 1.0)
- Top of screen (camTop) → z += 0.0
- Bottom of screen (camBottom) → z += 1.0

**Edge Case:** Negative world space not handled (normalization assumes camBottom < position.y < camTop)

### Frame Arena Pattern

**Memory Flow:**
```cpp
beginScene() → renderStartBatch() → clearArena(&renderer->frameArena)
// All per-frame allocations happen here
endScene() → renderFlush() // Frame arena still valid
// Next frame: clearArena() resets everything
```

**Benefits:**
- No fragmentation
- Fast allocation
- Automatic cleanup

## Questions for Future Consideration

1. **Multi-window support:** Would we ever need multiple renderers?
2. **Threading:** Could batching happen on a separate thread?
3. **Shader hot-reload:** We hot-reload game.dll, why not shaders?
4. **Material system:** Beyond textures, do we need material properties?
5. **Instancing:** Worth implementing for repeated sprites?
6. **Render graph:** Move toward modern frame graph architecture?

## References

- Raylib renderer: Similar immediate-mode batching approach
- Unity's SpriteRenderer: Two-tier API inspiration
- Our ECS: Parent/child already handled, renderer doesn't need hierarchy
- CLAUDE.md: Project architecture overview

## Recent API Additions (2025-11-18)

### Resolution Control API

Added flexible resolution management to support various rendering scenarios:

**New Functions:**
- `setRenderResolution(width, height)` - Change render coordinate space
- `setViewport(x, y, width, height)` - Control display scaling
- `getRenderSize()` / `getScreenSize()` - Query current resolutions

**Use Cases:**
- Pixel art games (render 640x320, display 1280x720)
- Dynamic resolution scaling (performance optimization)
- HD games with native resolution rendering

**See:** `docs/renderer-api-usage.md` for detailed examples

### Anchor Helper API

Added UI positioning helpers for bottom-left origin coordinate system:

**New Functions:**
- `anchorTopLeft(x, y)` - Position from top-left corner
- `anchorTopRight(x, y)` - Position from top-right corner
- `anchorBottomLeft(x, y)` - Position from bottom-left (explicit)
- `anchorBottomRight(x, y)` - Position from bottom-right corner
- `anchorCenter(x, y)` - Position from screen center

**Example:**
```cpp
// Clean UI positioning
renderDrawText2D(font, "HP: 100", anchorTopLeft(10, 10), 1.0f);
renderDrawText2D(font, "PAUSED", anchorCenter(0, 0), 2.0f);
```

**Benefits:**
- No manual `height - y` calculations
- Clear intent in code
- Works with any render resolution

## Changelog

- **2025-11-20:** Marked camera switching as complete with flush-on-switch design decision
- **2025-11-20:** Marked render target support (FBOs) as implemented
- **2025-11-20:** Added critical note about missing OpenGL cleanup in destroyRenderer()
- **2025-11-20:** Proposed custom shader support API for future implementation
- **2025-11-18:** Added resolution control and anchor helper APIs
- **2025-11-18:** Initial architecture review and improvement priorities documented
