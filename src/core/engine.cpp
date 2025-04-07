#include "engine.hpp"
#include "animationmanager.hpp"

EngineState* initEngine(uint32_t width, uint32_t height){
    if (!gladLoadGL()) {
        LOGERROR("GLAD not loaded properly in DLL.");
        return nullptr;
    }
    LOGINFO("GLAD successfully initialized");
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

    initTextureManager();
    engine->fontManager = initFontManager();

    initUIRenderer(width, height);
    uiRenderer->uiShader = createShader("shaders/ui-shader.vs", "shaders/ui-shader.fs");
    uiRenderer->uiTextShader = createShader("shaders/text-shader.vs", "shaders/text-shader.fs");
    loadFont(engine->fontManager, "ProggyClean");
    uiRenderer->uiFont = getFont(engine->fontManager, "ProggyClean");
    LOGINFO("UIRenderer sucessfully initialized");

    engine->ecs = initEcs();
    LOGINFO("ECS sucessfully initialized");
    
    if(!initAudioEngine()){
        LOGERROR("AudioEngine not initilized");
        return nullptr;
    }
    LOGINFO("Audio Engine sucessfully initialized");

    initAnimationManager();

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
    destroyUIRenderer();
    destroyAudioEngine();
    destroyTextureManager();
    destroyAnimationManager();
    destroyInput(engine->input);
    destroyFontManager(engine->fontManager);
    delete engine;
}