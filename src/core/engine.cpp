#include "engine.hpp"

EngineState* initEngine(uint32_t width, uint32_t height){
    //EngineState engine = {};
    EngineState* engine = new EngineState();
    engine->renderer = initRenderer(width, height);
    LOGINFO("Renderer successfully initialized");
    engine->renderer->shader = createShader("shaders/quad-shader.vs", "shaders/quad-shader.fs");
    engine->renderer->lineShader = createShader("shaders/line-shader.vs", "shaders/line-shader.fs");
    engine->renderer->textShader = createShader("shaders/text-shader.vs", "shaders/text-shader.fs");
    engine->renderer->simpleShader = createShader("shaders/simple-shader.vs", "shaders/simple-shader.fs");

    engine->input = initInput();
    LOGINFO("Inputs successfully initialized");

    engine->textureManager = initTextureManager();
    engine->fontManager = initFontManager();

    engine->dt = 0.0f;
    engine->fps = 0.0f;

    LOGINFO("GameEngine initialized successfully");
    return engine;
}

void updateDeltaTime(EngineState* engine, float dt, float fps){
    engine->dt = dt;
    engine->fps = fps;
}

void destroyEngine(EngineState* engine){
    destroyRenderer(engine->renderer);
    destroyInput(engine->input);
    destroyTextureManager(engine->textureManager);
    destroyFontManager(engine->fontManager);
    delete engine;
}