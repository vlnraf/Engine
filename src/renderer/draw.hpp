#pragma once

// Raylib-style immediate mode drawing API
// Simple, beginner-friendly wrapper around the core renderer

#include "core/coreapi.hpp"
#include "core/camera.hpp"
#include "renderer.hpp"
#include "texture.hpp"
#include "fontmanager.hpp"
#include <glm/glm.hpp>

// Color helper struct (Raylib-compatible)
struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;

    glm::vec4 toVec4() const {
        return {r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f};
    }
};

// Common colors (Raylib-compatible)
#define LIGHTGRAY  Color{ 200, 200, 200, 255 }
#define GRAY       Color{ 130, 130, 130, 255 }
#define DARKGRAY   Color{ 80, 80, 80, 255 }
#define YELLOW     Color{ 253, 249, 0, 255 }
#define GOLD       Color{ 255, 203, 0, 255 }
#define ORANGE     Color{ 255, 161, 0, 255 }
#define PINK       Color{ 255, 109, 194, 255 }
#define RED        Color{ 230, 41, 55, 255 }
#define MAROON     Color{ 190, 33, 55, 255 }
#define GREEN      Color{ 0, 228, 48, 255 }
#define LIME       Color{ 0, 158, 47, 255 }
#define DARKGREEN  Color{ 0, 117, 44, 255 }
#define SKYBLUE    Color{ 102, 191, 255, 255 }
#define BLUE       Color{ 0, 121, 241, 255 }
#define DARKBLUE   Color{ 0, 82, 172, 255 }
#define PURPLE     Color{ 200, 122, 255, 255 }
#define VIOLET     Color{ 135, 60, 190, 255 }
#define DARKPURPLE Color{ 112, 31, 126, 255 }
#define BEIGE      Color{ 211, 176, 131, 255 }
#define BROWN      Color{ 127, 106, 79, 255 }
#define DARKBROWN  Color{ 76, 63, 47, 255 }
#define WHITE      Color{ 255, 255, 255, 255 }
#define BLACK      Color{ 0, 0, 0, 255 }
#define BLANK      Color{ 0, 0, 0, 0 }
#define MAGENTA    Color{ 255, 0, 255, 255 }
#define RAYWHITE   Color{ 245, 245, 245, 255 }

//------------------------------------------------------------------------------------
// Frame Management
//------------------------------------------------------------------------------------

// Begin drawing frame (call once per frame)
CORE_API void BeginDrawing();

// End drawing frame and flush all batches
CORE_API void EndDrawing();

// Clear background with color
CORE_API void ClearBackground(Color color);

//------------------------------------------------------------------------------------
// Camera Management (2D)
//------------------------------------------------------------------------------------

// Begin 2D camera mode (transforms all subsequent draws)
CORE_API void BeginMode2D(OrtographicCamera camera);

// End 2D camera mode (return to screen space)
CORE_API void EndMode2D();

//------------------------------------------------------------------------------------
// Basic Shapes Drawing
//------------------------------------------------------------------------------------

// Draw a pixel
CORE_API void DrawPixel(int x, int y, Color color);

// Draw a line
CORE_API void DrawLine(int startX, int startY, int endX, int endY, Color color);

// Draw a line (using gl lines)
CORE_API void DrawLineV(glm::vec2 startPos, glm::vec2 endPos, Color color);

// Draw a rectangle
CORE_API void DrawRectangle(int x, int y, int width, int height, Color color);

// Draw a rectangle (Vector version)
CORE_API void DrawRectangleV(glm::vec2 position, glm::vec2 size, Color color);

// Draw rectangle outline
CORE_API void DrawRectangleLines(int x, int y, int width, int height, Color color);

//------------------------------------------------------------------------------------
// Texture Drawing
//------------------------------------------------------------------------------------

// Draw texture at position
CORE_API void DrawTexture(Texture* texture, int x, int y, Color tint);

// Draw texture with extended parameters
CORE_API void DrawTextureEx(Texture* texture, glm::vec2 position, float rotation, float scale, Color tint);

// Draw texture from source rect to dest rect
CORE_API void DrawTexturePro(Texture* texture, glm::vec4 source, glm::vec4 dest, glm::vec2 origin, float rotation, Color tint);

//------------------------------------------------------------------------------------
// Text Drawing
//------------------------------------------------------------------------------------

// Draw text (using default font)
CORE_API void DrawText(const char* text, int x, int y, int fontSize, Color color);

// Draw text with custom font
CORE_API void DrawTextEx(Font* font, const char* text, glm::vec2 position, float fontSize, float spacing, Color color);

// Draw current FPS
CORE_API void DrawFPS(int x, int y);

//------------------------------------------------------------------------------------
// Screen/Window Management
//------------------------------------------------------------------------------------

// Get screen width
CORE_API int GetScreenWidth();

// Get screen height
CORE_API int GetScreenHeight();

// Set render resolution (for pixel art games)
CORE_API void SetRenderResolution(int width, int height);

//------------------------------------------------------------------------------------
// Utility
//------------------------------------------------------------------------------------

// Get current render layer for debug
CORE_API float GetCurrentLayer();

// Set default layer for subsequent draws
CORE_API void SetDrawLayer(float layer);
