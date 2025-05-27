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


    initInput();
    LOGINFO("Inputs successfully initialized");

    initTextureManager();
    initFontManager();

    initRenderer(width, height);
    LOGINFO("Renderer successfully initialized");

    //initUIRenderer(width, height);
    //uiRenderer->uiShader = createShader("shaders/ui-shader.vs", "shaders/ui-shader.fs");
    //uiRenderer->uiTextShader = createShader("shaders/text-shader.vs", "shaders/text-shader.fs");
    initUI({width, height});
    loadFont("ProggyClean");
    //uiRenderer->uiFont = getFont("ProggyClean");
    //LOGINFO("UIRenderer sucessfully initialized");

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

    engine->windowWidth = width;
    engine->windowHeight = height;

    LOGINFO("GameEngine initialized successfully");
    return engine;
}

void updateEngineWindowSize(EngineState* engine, int width, int height){
    engine->windowWidth = width;
    engine->windowHeight = height;
}

void updateDeltaTime(EngineState* engine, float dt, float fps){
    engine->dt = dt;
    engine->fps = fps;
}

void destroyEngine(EngineState* engine){
    destroyRenderer();
    //destroyUIRenderer();
    destroyUI();
    destroyAudioEngine();
    destroyTextureManager();
    destroyAnimationManager();
    destroyFontManager();
    destroyInput();
    delete engine;
}