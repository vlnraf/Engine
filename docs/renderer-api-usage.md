# Renderer API Usage Guide

## Render Resolution vs Window Size

The renderer supports **independent render and display resolutions**, giving you full control over how your game looks.

### Key Concepts

**Render Resolution**: The coordinate space your game draws in (cameras, sprites, UI)
**Window Size**: The actual pixel dimensions of the window
**Viewport**: Controls how render resolution maps to window

## Configuration API

### `setRenderResolution(width, height)`

Changes the render resolution (camera coordinate space) at runtime.

```cpp
// Set render resolution to pixel art size
setRenderResolution(640, 320);

// Now all rendering happens in 640x320 space
beginScene(NORMAL);
    renderDrawText2D(font, "HP: 100", {10, 10}, 1.0f);  // Position in 640x320 space
endScene();
```

**Use cases:**
- Pixel art games (render at low-res, display at high-res)
- Dynamic resolution scaling for performance
- Retro aesthetic

### `setViewport(x, y, width, height)`

Controls which part of the window receives rendered output and how it's scaled.

```cpp
// Full window viewport (default)
setViewport(0, 0, 1280, 720);

// Render to left half of window
setViewport(0, 0, 640, 720);

// Pillarbox for 4:3 content on 16:9 display
int barWidth = (windowWidth - gameWidth * scale) / 2;
setViewport(barWidth, 0, gameWidth * scale, windowHeight);
```

**Use cases:**
- Split-screen multiplayer
- Letterboxing/pillarboxing for aspect ratio
- Picture-in-picture

### `getScreenSize()` / `getRenderSize()`

Query current resolutions.

```cpp
glm::vec2 renderRes = getRenderSize();  // {640, 320}
glm::vec2 screenRes = getScreenSize();  // Same as render res

// Use for dynamic UI positioning
float centerX = renderRes.x / 2.0f;
renderDrawText2D(font, "PAUSED", {centerX, renderRes.y / 2}, 2.0f);
```

## Common Scenarios

### Scenario 1: Pixel Art Game (640x320 → 1280x720)

```cpp
// In game initialization
void gameStart(Arena* gameArena, EngineState* engine) {
    // Set render resolution to pixel art size
    setRenderResolution(640, 320);

    // Viewport scales to window (handled in engine init, but can override)
    // setViewport(0, 0, 1280, 720);  // Usually automatic
}

// In game rendering
void gameRender(Arena* gameArena, EngineState* engine, float dt) {
    // Game world (640x320 space)
    OrtographicCamera worldCam = createCamera(playerPos, 640, 320);
    beginScene(worldCam, NORMAL);
        renderDrawSprite(player, ...);  // Position in 640x320 world
    endScene();

    // UI (also 640x320 space - scales with game)
    beginScene(NORMAL);  // Uses screenCamera at 640x320
        renderDrawText2D(font, "HP: 100", anchorTopLeft(10, 10), 1.0f);
        renderDrawQuad2D(heartIcon, anchorTopLeft(5, 5), {1, 1}, {0, 0}, {0, 0}, {16, 16});
    endScene();
}
```

**Result**: Everything renders at 640x320, GPU scales to 1280x720. Pixel-perfect consistency.

### Scenario 2: HD Game with HD UI (1280x720)

```cpp
// In game initialization
void gameStart(Arena* gameArena, EngineState* engine) {
    // Render at window resolution (default, or explicitly set)
    setRenderResolution(1280, 720);
}

// In game rendering
void gameRender(Arena* gameArena, EngineState* engine, float dt) {
    // Game world
    OrtographicCamera worldCam = createCamera(playerPos, 1280, 720);
    beginScene(worldCam, NORMAL);
        renderDrawSprite(player, ...);
    endScene();

    // UI (crisp text at native resolution)
    beginScene(NORMAL);
        renderDrawText2D(font, "HP: 100", anchorTopLeft(10, 10), 1.0f);
    endScene();
}
```

**Result**: 1:1 pixel mapping, crisp rendering.

### Scenario 3: Mixed Resolution (Pixel Art Game, HD UI)

**Requires render targets** (not yet implemented), but the API would be:

```cpp
// Render game to 640x320 texture
RenderTarget* gameRT = createRenderTarget(640, 320);
beginRenderTarget(gameRT);
    beginScene(worldCam, NORMAL);
        renderDrawSprite(player, ...);
    endScene();
endRenderTarget();

// Composite at 1280x720 with HD UI
setRenderResolution(1280, 720);
beginScene(NORMAL);
    // Draw upscaled game texture
    Texture* gameTex = getRenderTargetTexture(gameRT);
    renderDrawQuad2D(gameTex, {0, 0}, {1280, 720}, ...);

    // HD UI on top
    renderDrawText2D(font, "HP: 100", anchorTopLeft(10, 10), 1.0f);  // Crisp!
endScene();
```

### Scenario 4: Dynamic Resolution Scaling

```cpp
// Adaptive performance: lower resolution when FPS drops
void gameUpdate(Arena* gameArena, EngineState* engine, float dt) {
    if (engine->fps < 30.0f) {
        setRenderResolution(640, 360);  // Drop to 360p
    } else if (engine->fps > 55.0f) {
        setRenderResolution(1280, 720);  // Back to 720p
    }
}
```

## Anchor Helpers

UI positioning helpers for bottom-left origin coordinate system.

### `anchorTopLeft(x, y)`

Position relative to top-left corner.

```cpp
// 10 pixels from top-left
renderDrawText2D(font, "FPS: 60", anchorTopLeft(10, 10), 1.0f);
```

### `anchorTopRight(x, y)`

Position relative to top-right corner.

```cpp
// 10 pixels from top-right
renderDrawText2D(font, "Score: 100", anchorTopRight(10, 10), 1.0f);
```

### `anchorBottomLeft(x, y)`

Position relative to bottom-left corner (same as raw coordinates, but explicit).

```cpp
// 10 pixels from bottom-left
renderDrawText2D(font, "Lives: 3", anchorBottomLeft(10, 10), 1.0f);
// Equivalent to: renderDrawText2D(font, "Lives: 3", {10, 10}, 1.0f);
```

### `anchorBottomRight(x, y)`

Position relative to bottom-right corner.

```cpp
// 10 pixels from bottom-right
renderDrawText2D(font, "v1.0", anchorBottomRight(10, 10), 1.0f);
```

### `anchorCenter(x, y)`

Position relative to screen center.

```cpp
// Centered text
renderDrawText2D(font, "PAUSED", anchorCenter(0, 0), 2.0f);

// Slightly above center
renderDrawText2D(font, "Press Start", anchorCenter(0, 50), 1.0f);
```

## Coordinate System

All rendering uses **bottom-left origin** with **Y+ pointing UP**:

```
(0, height) -------- (width, height)   <- Top
    |                       |
    |       Screen          |
    |                       |
(0, 0) ---------- (width, 0)            <- Bottom
^
Bottom-left origin
```

**This matches:**
- OpenGL convention
- Unity UI system
- Mathematical convention (Cartesian coordinates)

**Why bottom-left?**
- Consistent with world space (no mental flip)
- Simpler rendering code (no sprite flipping)
- Natural for physics (Y+ is up)

**For UI**, use anchor helpers to avoid mental math:
- `anchorTopLeft(10, 10)` → `{10, height - 10}`
- `anchorCenter(0, 0)` → `{width/2, height/2}`

## Best Practices

### 1. Set Render Resolution Early

```cpp
void gameStart(Arena* gameArena, EngineState* engine) {
    // First thing: configure rendering
    setRenderResolution(640, 320);

    // Then load assets, create cameras, etc.
}
```

### 2. Use Anchors for All UI

```cpp
// Good: Intent is clear
renderDrawText2D(font, "HP", anchorTopLeft(10, 10), 1.0f);

// Bad: Magic number, unclear intent
renderDrawText2D(font, "HP", {10, 310}, 1.0f);  // Why 310?
```

### 3. Query Resolution for Dynamic UI

```cpp
glm::vec2 size = getRenderSize();
float buttonX = size.x / 2;
float buttonY = size.y / 2;

renderButton(buttonX, buttonY);  // Always centered
```

### 4. Keep Game Resolution Constant

```cpp
// Good: Design around fixed game resolution
const float GAME_WIDTH = 640;
const float GAME_HEIGHT = 320;
setRenderResolution(GAME_WIDTH, GAME_HEIGHT);

// Bad: Tie game logic to window size
setRenderResolution(windowWidth, windowHeight);  // Breaks on resize!
```

## Migration Guide

### From Old API (Window-Size Rendering)

**Before:**
```cpp
// UI was at window size (1280x720)
beginScene(NORMAL);
    renderDrawText2D(font, "HP", {10, 710}, 1.0f);  // Near top
endScene();
```

**After:**
```cpp
// Set render resolution to match game
setRenderResolution(640, 320);

// Use anchors for positioning
beginScene(NORMAL);
    renderDrawText2D(font, "HP", anchorTopLeft(10, 10), 1.0f);
endScene();
```

## API Reference Summary

```cpp
// Configuration
void setRenderResolution(uint32_t width, uint32_t height);
void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
glm::vec2 getRenderSize();
glm::vec2 getScreenSize();

// Anchors
glm::vec2 anchorTopLeft(float x, float y);
glm::vec2 anchorTopRight(float x, float y);
glm::vec2 anchorBottomLeft(float x, float y);
glm::vec2 anchorBottomRight(float x, float y);
glm::vec2 anchorCenter(float x, float y);

// Scene Management
void beginScene(RenderMode mode = NORMAL);
void beginScene(OrtographicCamera camera, RenderMode mode);
void endScene();

// Drawing
void renderDrawSprite(glm::vec3 pos, glm::vec3 scale, glm::vec3 rot, SpriteComponent* sprite);
void renderDrawText2D(Font* font, const char* text, glm::vec2 pos, float scale);
void renderDrawQuad2D(Texture* tex, glm::vec2 pos, glm::vec2 scale, ...);
void renderDrawLine(glm::vec2 p0, glm::vec2 p1, glm::vec4 color, float layer);
void renderDrawRect(glm::vec2 offset, glm::vec2 size, glm::vec4 color, float layer);
void renderDrawFilledRect(glm::vec2 pos, glm::vec2 size, glm::vec2 rot, glm::vec4 color);
```
