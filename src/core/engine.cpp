#include "engine.hpp"

EngineState initEngine(uint32_t width, uint32_t height){
    EngineState engine = {};
    engine.renderer = initRenderer(width, height);
    LOGINFO("Renderer successfully initialized");
    engine.renderer.shader = createShader("shaders/default-shader.vs", "shaders/default-shader.fs");

    engine.input = initInput();
    LOGINFO("Inputs successfully initialized");

    engine.dt = 0.0f;
    engine.fps = 0.0f;

    LOGINFO("GameEngine initialized successfully");
    return engine;
}

void updateDeltaTime(EngineState* engine, float dt, float fps){
    engine->dt = dt;
    engine->fps = fps;
}