# Raylib-Style Drawing API

## Overview

The `draw.hpp` API provides a **Raylib-compatible immediate-mode drawing interface** for beginners and those familiar with Raylib. It's a wrapper around the core renderer that simplifies common drawing tasks.

## Quick Start

```cpp
#include "renderer/draw.hpp"

void gameRender(Arena* gameArena, EngineState* engine, float dt) {
    BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw text
        DrawText("Hello World!", 10, 10, 20, BLACK);

        // Draw shapes
        DrawRectangle(100, 100, 200, 100, RED);
        DrawLine(0, 0, 800, 600, BLUE);

        // Draw texture
        Texture* playerTex = getTexture("player");
        DrawTexture(playerTex, 400, 300, WHITE);
    EndDrawing();
}
```

## Key Concepts

### Frame Management

**Every frame must begin with `BeginDrawing()` and end with `EndDrawing()`:**

```cpp
BeginDrawing();
    // All drawing code here
EndDrawing();
```

**Note:** Unlike Raylib, you don't need `BeginDrawing/EndDrawing` around the entire game loop—just around your rendering code.

### Coordinate System

**Top-Left Origin (Y+ Down)** - Like Raylib, traditional UI coordinates:

```
(0, 0) ---------- (width, 0)    <- Top
  |                   |
  |      Screen       |
  |                   |
(0, height) ---- (width, height) <- Bottom
```

**This is different from the core renderer** (which uses bottom-left origin). The `draw.hpp` API automatically converts coordinates.

### Camera Modes

By default, drawing happens in **screen space**. Use `BeginMode2D/EndMode2D` for world-space camera:

```cpp
BeginDrawing();
    // Screen space (UI)
    DrawText("Score: 100", 10, 10, 20, BLACK);

    BeginMode2D(worldCamera);
        // World space (game)
        DrawTexture(playerTex, player.x, player.y, WHITE);
    EndMode2D();

    // Back to screen space
    DrawFPS(10, 30);
EndDrawing();
```

## API Reference

### Frame Management

#### `BeginDrawing()`
Begin drawing frame. Call once per frame before any drawing.

#### `EndDrawing()`
End drawing frame and flush all batches.

#### `ClearBackground(Color color)`
Clear screen with color.

```cpp
ClearBackground(RAYWHITE);
ClearBackground(Color{30, 30, 30, 255});  // Dark gray
```

### Camera

#### `BeginMode2D(OrtographicCamera camera)`
Start world-space camera mode. All subsequent draws use camera transform.

```cpp
OrtographicCamera cam = createCamera(playerPos, 640, 320);
BeginMode2D(cam);
    // World-space drawing
EndMode2D();
```

#### `EndMode2D()`
End camera mode, return to screen space.

### Shapes

#### `DrawPixel(int x, int y, Color color)`
Draw single pixel.

```cpp
DrawPixel(100, 100, RED);
```

#### `DrawLine(int x1, int y1, int x2, int y2, Color color)`
Draw line between two points.

```cpp
DrawLine(0, 0, 800, 600, BLUE);
```

#### `DrawLineV(glm::vec2 start, glm::vec2 end, Color color)`
Draw line (vector version).

```cpp
DrawLineV({0, 0}, {800, 600}, BLUE);
```

#### `DrawRectangle(int x, int y, int w, int h, Color color)`
Draw filled rectangle.

```cpp
DrawRectangle(100, 100, 200, 150, RED);
```

#### `DrawRectangleV(glm::vec2 pos, glm::vec2 size, Color color)`
Draw filled rectangle (vector version).

```cpp
DrawRectangleV({100, 100}, {200, 150}, RED);
```

#### `DrawRectangleLines(int x, int y, int w, int h, Color color)`
Draw rectangle outline.

```cpp
DrawRectangleLines(100, 100, 200, 150, GREEN);
```

### Textures

#### `DrawTexture(Texture* tex, int x, int y, Color tint)`
Draw texture at position.

```cpp
Texture* tex = getTexture("player");
DrawTexture(tex, 100, 100, WHITE);
```

#### `DrawTextureEx(Texture* tex, vec2 pos, float rotation, float scale, Color tint)`
Draw texture with extended parameters.

```cpp
DrawTextureEx(playerTex, {100, 100}, 45.0f, 2.0f, WHITE);  // Rotated 45°, 2x scale
```

#### `DrawTexturePro(Texture* tex, vec4 source, vec4 dest, vec2 origin, float rotation, Color tint)`
Draw texture with full control (source rect, dest rect, origin, rotation).

```cpp
// Draw portion of spritesheet
glm::vec4 source = {0, 0, 32, 32};      // 32x32 sprite from (0,0)
glm::vec4 dest = {100, 100, 64, 64};    // Draw at 64x64 size
DrawTexturePro(tex, source, dest, {16, 16}, 0.0f, WHITE);
```

### Text

#### `DrawText(const char* text, int x, int y, int fontSize, Color color)`
Draw text using default font.

```cpp
DrawText("Hello World!", 10, 10, 20, BLACK);
DrawText("Score: 1000", 10, 40, 16, BLUE);
```

#### `DrawTextEx(Font* font, const char* text, vec2 pos, float size, float spacing, Color color)`
Draw text with custom font.

```cpp
Font* customFont = getFont("Arial");
DrawTextEx(customFont, "Custom Font", {100, 100}, 24.0f, 1.0f, RED);
```

#### `DrawFPS(int x, int y)`
Draw current FPS.

```cpp
DrawFPS(10, 10);
```

### Screen Info

#### `GetScreenWidth()` / `GetScreenHeight()`
Get current render resolution.

```cpp
int w = GetScreenWidth();
int h = GetScreenHeight();
DrawText("Center", w/2, h/2, 20, BLACK);
```

#### `SetRenderResolution(int width, int height)`
Change render resolution (for pixel art).

```cpp
SetRenderResolution(640, 320);  // Pixel art resolution
```

### Utility

#### `SetDrawLayer(float layer)`
Set Z-layer for subsequent draws.

```cpp
SetDrawLayer(-10.0f);  // Background layer
DrawRectangle(0, 0, 800, 600, SKYBLUE);

SetDrawLayer(0.0f);    // Default layer
DrawTexture(playerTex, 100, 100, WHITE);

SetDrawLayer(10.0f);   // Foreground layer
DrawText("UI", 10, 10, 20, BLACK);
```

#### `GetCurrentLayer()`
Query current layer.

## Colors

Predefined Raylib-compatible colors:

```cpp
LIGHTGRAY, GRAY, DARKGRAY
WHITE, BLACK, BLANK
RED, MAROON, PINK
ORANGE, GOLD, YELLOW
GREEN, LIME, DARKGREEN
BLUE, DARKBLUE, SKYBLUE
PURPLE, VIOLET, DARKPURPLE
BEIGE, BROWN, DARKBROWN
MAGENTA, RAYWHITE
```

Custom colors:

```cpp
Color myColor = {200, 100, 50, 255};  // RGBA
DrawRectangle(0, 0, 100, 100, myColor);
```

## Complete Examples

### Example 1: Simple Game with UI

```cpp
#include "renderer/draw.hpp"

void gameRender(Arena* gameArena, EngineState* engine, float dt) {
    BeginDrawing();
        ClearBackground(RAYWHITE);

        // Game world
        OrtographicCamera cam = createCamera(playerPos, 640, 320);
        BeginMode2D(cam);
            // Draw world
            DrawTexture(backgroundTex, 0, 0, WHITE);
            DrawTexture(playerTex, player.x, player.y, WHITE);
            DrawTexture(enemyTex, enemy.x, enemy.y, WHITE);
        EndMode2D();

        // UI overlay
        DrawText("HP: 100", 10, 10, 20, RED);
        DrawText("Score: 9999", 10, 40, 20, BLUE);
        DrawFPS(10, 70);

        // Debug
        if (debugMode) {
            DrawRectangleLines(player.x, player.y, 32, 32, GREEN);
        }
    EndDrawing();
}
```

### Example 2: Pixel Art Game

```cpp
void gameStart(Arena* gameArena, EngineState* engine) {
    // Set pixel art resolution
    SetRenderResolution(320, 180);  // 16:9 at low res
}

void gameRender(Arena* gameArena, EngineState* engine, float dt) {
    BeginDrawing();
        ClearBackground(Color{30, 20, 40, 255});  // Dark purple

        // Everything renders at 320x180
        DrawText("Pixel Art Game", 10, 10, 8, WHITE);

        OrtographicCamera cam = createCamera(playerPos, 320, 180);
        BeginMode2D(cam);
            DrawTexture(playerTex, player.x, player.y, WHITE);
        EndMode2D();
    EndDrawing();

    // GPU scales 320x180 -> window size
}
```

### Example 3: Multiple Cameras

```cpp
void gameRender(Arena* gameArena, EngineState* engine, float dt) {
    BeginDrawing();
        ClearBackground(BLACK);

        // Main game view
        BeginMode2D(gameCamera);
            DrawTexture(worldTex, 0, 0, WHITE);
            DrawTexture(playerTex, player.x, player.y, WHITE);
        EndMode2D();

        // UI
        DrawText("Lives: 3", 10, 10, 20, WHITE);

        // Minimap (different camera)
        BeginMode2D(minimapCamera);
            SetDrawLayer(10.0f);  // Above main game
            DrawRectangle(500, 10, 200, 150, DARKGRAY);
            DrawTexture(playerTex, player.x, player.y, RED);
        EndMode2D();
    EndDrawing();
}
```

## Differences from Raylib

### What's the Same
✅ Function names and signatures
✅ Color system
✅ BeginDrawing/EndDrawing pattern
✅ BeginMode2D/EndMode2D for cameras
✅ Top-left origin coordinate system

### What's Different
⚠️ Must call `BeginDrawing/EndDrawing` per render function (not per game loop)
⚠️ Camera creation uses engine's `createCamera()` function
⚠️ Textures managed through engine's texture system (`getTexture()`)
⚠️ Fonts managed through engine's font system (`getFont()`)
⚠️ Some advanced Raylib functions not implemented yet

## Migration from Core Renderer API

**From Core API:**
```cpp
beginScene(NORMAL);
    renderDrawText2D(font, "HP: 100", anchorTopLeft(10, 10), 1.0f);
    renderDrawFilledRect({100, 100}, {200, 150}, {0, 0}, {1, 0, 0, 1});
endScene();
```

**To Raylib-Style API:**
```cpp
BeginDrawing();
    DrawText("HP: 100", 10, 10, 20, BLACK);
    DrawRectangle(100, 100, 200, 150, RED);
EndDrawing();
```

**Simpler, more intuitive!**

## When to Use Which API

### Use Raylib-Style API (`draw.hpp`) When:
- 👍 Learning game development
- 👍 Prototyping quickly
- 👍 Simple 2D games
- 👍 Familiar with Raylib
- 👍 Want minimal code

### Use Core Renderer API (`renderer.hpp`) When:
- 🔧 Need fine control over batching
- 🔧 Building engine features
- 🔧 Advanced rendering techniques
- 🔧 Performance-critical code
- 🔧 Complex scene composition

**You can mix both!** Use Raylib-style for game code, core API for engine internals.

## Implementation Status

✅ **Implemented:**
- Frame management (BeginDrawing/EndDrawing)
- Camera modes (BeginMode2D/EndMode2D)
- Basic shapes (line, rectangle, pixel)
- Texture drawing (DrawTexture, DrawTextureEx, DrawTexturePro)
- Text drawing (DrawText, DrawTextEx)
- Screen queries (GetScreenWidth/Height)
- Layer control (SetDrawLayer)

⏳ **TODO:**
- DrawFPS (needs engine FPS access)
- Circle drawing
- Triangle drawing
- Polygon drawing
- Gradient fills
- More text measurements
- Audio integration

## Performance Notes

The Raylib-style API is a **thin wrapper** around the core renderer. Performance is nearly identical:

- Batching still works (automatic)
- Same draw call count
- Minimal overhead (just coordinate conversion)
- No extra allocations

**Use it freely without performance concerns.**
