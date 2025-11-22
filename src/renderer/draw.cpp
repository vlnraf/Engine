#include "draw.hpp"
#include "renderer.hpp"
#include "fontmanager.hpp"
#include "core/tracelog.hpp"

// Internal state for Raylib-style API
struct DrawState {
    bool inDrawing = false;
    bool inMode2D = false;
    OrtographicCamera savedCamera;
    float currentLayer = 0.0f;
    Font* defaultFont = nullptr;
};

static DrawState* drawState = nullptr;

// Initialize draw state (called once in initRenderer)
void initDrawAPI() {
    static DrawState state;
    drawState = &state;
    drawState->defaultFont = getFont("default");  // Assumes you have a default font
}

//------------------------------------------------------------------------------------
// Frame Management
//------------------------------------------------------------------------------------

void BeginDrawing() {
    if (!drawState) initDrawAPI();

    if (drawState->inDrawing) {
        LOGWARN("BeginDrawing called while already drawing!");
        return;
    }

    drawState->inDrawing = true;
    drawState->inMode2D = false;
    drawState->currentLayer = 0.0f;

    // Start with screen camera (default)
    beginScene(NORMAL);
}

void EndDrawing() {
    if (!drawState->inDrawing) {
        LOGWARN("EndDrawing called without BeginDrawing!");
        return;
    }

    if (drawState->inMode2D) {
        LOGWARN("EndDrawing called while still in Mode2D! Calling EndMode2D automatically.");
        EndMode2D();
    }

    endScene();
    drawState->inDrawing = false;
}

void ClearBackground(Color color) {
    clearColor(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
}

//------------------------------------------------------------------------------------
// Camera Management
//------------------------------------------------------------------------------------

void BeginMode2D(OrtographicCamera camera) {
    if (!drawState->inDrawing) {
        LOGERROR("BeginMode2D called outside BeginDrawing/EndDrawing!");
        return;
    }

    if (drawState->inMode2D) {
        LOGWARN("BeginMode2D called while already in Mode2D!");
        return;
    }

    // End current scene (screen space)
    endScene();

    // Start new scene with world camera
    beginMode2D(camera);

    drawState->inMode2D = true;
}

void EndMode2D() {
    if (!drawState->inMode2D) {
        LOGWARN("EndMode2D called without BeginMode2D!");
        return;
    }

    // End world camera scene
    endScene();

    // Resume screen space rendering
    beginScene(NORMAL);

    drawState->inMode2D = false;
}

//------------------------------------------------------------------------------------
// Basic Shapes
//------------------------------------------------------------------------------------

void DrawPixel(int x, int y, Color color) {
    DrawRectangle(x, y, 1, 1, color);
}

void DrawLine(int startX, int startY, int endX, int endY, Color color) {
    DrawLineV({(float)startX, (float)startY}, {(float)endX, (float)endY}, color);
}

void DrawLineV(glm::vec2 startPos, glm::vec2 endPos, Color color) {
    renderDrawLine(startPos, endPos, color.toVec4(), drawState->currentLayer);
}

void DrawRectangle(int x, int y, int width, int height, Color color) {
    DrawRectangleV({(float)x, (float)y}, {(float)width, (float)height}, color);
}

void DrawRectangleV(glm::vec2 position, glm::vec2 size, Color color) {
    renderDrawFilledRect(position, size, 0.0f, color.toVec4(), drawState->currentLayer);
}

void DrawRectangleLines(int x, int y, int width, int height, Color color) {
    renderDrawRect({(float)x, (float)y}, {(float)width, (float)height}, color.toVec4(), drawState->currentLayer);
}

//------------------------------------------------------------------------------------
// Texture Drawing
//------------------------------------------------------------------------------------

void DrawTexture(Texture* texture, int x, int y, Color tint) {
    DrawTextureEx(texture, {(float)x, (float)y}, 0.0f, 1.0f, tint);
}

void DrawTextureEx(Texture* texture, glm::vec2 position, float rotation, float scale, Color tint) {
    glm::vec2 size = {texture->width * scale, texture->height * scale};
    glm::vec4 source = {0, 0, (float)texture->width, (float)texture->height};
    glm::vec4 dest = {position.x, position.y, size.x, size.y};

    DrawTexturePro(texture, source, dest, {0, 0}, rotation, tint);
}

void DrawTexturePro(Texture* texture, glm::vec4 source, glm::vec4 dest, glm::vec2 origin, float rotation, Color tint) {
    // source: {x, y, width, height} in texture
    // dest: {x, y, width, height} on screen

    Rect sourceRect = {.pos = {source.x, source.y}, .size = {source.z, source.w}};
    glm::vec3 position = {dest.x, dest.y, drawState->currentLayer};
    glm::vec2 size = {dest.z, dest.w};
    glm::vec3 rot = {0, 0, rotation};

    renderDrawQuadPro(position, size, rot, sourceRect, origin, texture, tint.toVec4(), false);
}

//------------------------------------------------------------------------------------
// Text Drawing
//------------------------------------------------------------------------------------

void DrawText(const char* text, int x, int y, int fontSize, Color color) {
    if (!drawState->defaultFont) {
        LOGWARN("Default font not loaded! Cannot draw text.");
        return;
    }

    DrawTextEx(drawState->defaultFont, text, {(float)x, (float)y}, (float)fontSize, 1.0f, color);
}

void DrawTextEx(Font* font, const char* text, glm::vec2 position, float fontSize, float spacing, Color color) {
    // Note: Raylib uses top-left for text, we use bottom-left
    // Need to convert coordinate systems
    glm::vec2 screenSize = getRenderSize();
    glm::vec2 adjustedPos = {position.x, screenSize.y - position.y};

    renderDrawText2D(font, text, adjustedPos, fontSize / 10.0f);  // Scale adjustment
}

void DrawFPS(int x, int y) {
    // TODO: Get actual FPS from engine
    // For now, placeholder
    DrawText("FPS: 60", x, y, 20, GREEN);
}

//------------------------------------------------------------------------------------
// Screen/Window Management
//------------------------------------------------------------------------------------

int GetScreenWidth() {
    glm::vec2 size = getScreenSize();
    return (int)size.x;
}

int GetScreenHeight() {
    glm::vec2 size = getScreenSize();
    return (int)size.y;
}

void SetRenderResolution(int width, int height) {
    setRenderResolution(width, height);
}

//------------------------------------------------------------------------------------
// Utility
//------------------------------------------------------------------------------------

float GetCurrentLayer() {
    return drawState ? drawState->currentLayer : 0.0f;
}

void SetDrawLayer(float layer) {
    if (drawState) {
        drawState->currentLayer = layer;
    }
}
