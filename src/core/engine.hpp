#include "core/coreapi.hpp"
#include "renderer/renderer.hpp"
#include "renderer/texture.hpp"
#include "renderer/fontmanager.hpp"
#include "audioengine.hpp"
#include "tracelog.hpp"
#include "input.hpp"
#include "ui.hpp"
#include "arena.hpp"

struct EngineState{
    Arena* arena;
    //Arena* textureManagerArena;
    //Arena* inputArena;
    //Arena* fontManagerArena;
    //Arena* renderArena;
    //Arena* ecsArena;
    Ecs* ecs;
    UIState* uiState;

    int windowWidth;
    int windowHeight;

    void* gameState;

    float dt;
    float fps;
};


CORE_API EngineState* initEngine(uint32_t width, uint32_t height);
CORE_API void updateDeltaTime(EngineState* engine, float dt, float fps);
CORE_API void updateEngineWindowSize(EngineState* engine, int width, int height);
CORE_API void destroyEngine(EngineState* engine);