#include "renderer/renderer.hpp"
#include "tracelog.hpp"
#include "input.hpp"

struct EngineState{
    Renderer renderer;
    Input input;

    float dt;
    float fps;
};

EngineState initEngine(uint32_t width, uint32_t height);
void updateDeltaTime(EngineState* engine, float dt, float fps);