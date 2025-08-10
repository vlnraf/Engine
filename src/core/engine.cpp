#include "engine.hpp"
#include "animationmanager.hpp"
#include "arena.hpp"

EngineState* initEngine(uint32_t width, uint32_t height){
    #ifdef _WIN32
    if (!gladLoadGL()) {
        LOGERROR("GLAD not loaded properly in DLL.");
        return nullptr;
    }
    #endif
    LOGINFO("GLAD successfully initialized");
    Arena* engineArena = initArena(); //NOTE: 4MB default
    EngineState* engine = arenaAllocStruct(engineArena, EngineState);
    engine->arena = engineArena;

    Arena* inputArena = initArena();
    engine->inputArena = inputArena;
    initInput(engine->inputArena);
    LOGINFO("Inputs successfully initialized");

    Arena* textureArena = initArena(GB(1));
    engine->textureManagerArena = textureArena;
    initTextureManager(engine->textureManagerArena);

    Arena* fontArena = initArena(MB(500));
    engine->fontManagerArena = fontArena;
    initFontManager(engine->fontManagerArena);

    initRenderer(engine->arena, width, height);
    LOGINFO("Renderer successfully initialized");

    //initUIRenderer(width, height);
    //uiRenderer->uiShader = createShader("shaders/ui-shader.vs", "shaders/ui-shader.fs");
    //uiRenderer->uiTextShader = createShader("shaders/text-shader.vs", "shaders/text-shader.fs");
    engine->uiState = initUI({width, height});
    //uiRenderer->uiFont = getFont("ProggyClean");
    //LOGINFO("UIRenderer sucessfully initialized");

    engine->ecs = initEcs(engine->arena);
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
    engine->uiState->screenSize = {width, height};
}

void updateDeltaTime(EngineState* engine, float dt, float fps){
    engine->dt = dt;
    engine->fps = fps;
}

void destroyEngine(EngineState* engine){
    //destroyUIRenderer();
    destroyUI();
    destroyAudioEngine();
    //destroyTextureManager();
    destroyAnimationManager();
    destroyEcs(engine->ecs);
    destroyRenderer();
    clearArena(engine->arena);
    //destroyFontManager();
    //destroyInput();
    //delete engine;
}