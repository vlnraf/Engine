#include "core/coreapi.hpp"
#include "renderer/renderer.hpp"
#include "renderer/texture.hpp"
#include "renderer/fontmanager.hpp"
#include "renderer/uirenderer.hpp"
#include "audioengine.hpp"
#include "tracelog.hpp"
#include "input.hpp"

struct EngineState{
    Renderer* renderer;
    Input* input;
    TextureManager* textureManager;
    FontManager* fontManager;
    Ecs* ecs;

    void* gameState;

    float dt;
    float fps;
};


CORE_API EngineState* initEngine(uint32_t width, uint32_t height);
CORE_API void updateDeltaTime(EngineState* engine, float dt, float fps);
CORE_API void destroyEngine(EngineState* engine);