#ifndef __EMSCRIPTEN__
#include <glad/glad.h>
#else
#include <GLES3/gl3.h>
#endif

#include "engine.hpp"
#include "animationmanager.hpp"
#include "arena.hpp"
#include "colliders.hpp"

EngineState* initEngine(uint32_t width, uint32_t height){
    // Load OpenGL function pointers for this DLL
    // This must be called after the OpenGL context is created in the application layer
    #ifndef __EMSCRIPTEN__
    if (!gladLoadGL()) {
        LOGERROR("GLAD not loaded properly in core.dll");
        return nullptr;
    }
    #endif
    Arena engineArena = initArena(MB(100)); //NOTE: 4MB default
    EngineState* engine = arenaAllocStruct(&engineArena, EngineState);
    engine->arena = engineArena;
    //engine->debugMode = false;
    engine->gameArena = initArena(MB(100));

    initInput(&engine->arena);
    LOGINFO("Inputs successfully initialized");

    initTextureManager(&engine->arena);

    initFontManager(&engine->arena);

    initRenderer(&engine->arena, width, height);
    //initRenderer(&engine->arena, engine->mainCamera.width, engine->mainCamera.height);
    LOGINFO("Renderer successfully initialized");

    initUI(&engine->arena);
    engine->ecs = initEcs(&engine->arena);
    LOGINFO("ECS sucessfully initialized");
    
    if(!initAudioEngine()){
        LOGERROR("AudioEngine not initilized");
        return nullptr;
    }
    LOGINFO("Audio Engine sucessfully initialized");

    initAnimationManager();

    initCollisionManager(&engine->arena);
    importBaseModule(engine->ecs);
    importCollisionModule(engine->ecs);

    //engine->dt = 0.0f;
    //engine->fps = 0.0f;

    //engine->windowWidth = width;
    //engine->windowHeight = height;

    LOGINFO("GameEngine initialized successfully");
    return engine;
}

void updateEngineWindowSize(EngineState* engine, int width, int height){
    //engine->windowWidth = width;
    //engine->windowHeight = height;
    //setRenderResolution(width, height);  // Update renderer screen camera
    //setViewport(0, 0, width, height);    // Update OpenGL viewport

    // Update main camera dimensions while preserving position
    //glm::vec3 oldPosition = engine->mainCamera.position;
    //engine->mainCamera = createCamera(oldPosition, (float)width, (float)height);

    //UIsetScreenSize(width, height);
    //engine->uiState->screenSize = {engine->mainCamera.width, engine->mainCamera.height};
}

void destroyEngine(EngineState* engine){
    //destroyUIRenderer();
    destroyUI();
    destroyAudioEngine();
    //destroyTextureManager();
    destroyAnimationManager();
    destroyEcs(engine->ecs);
    destroyRenderer();
    clearArena(&engine->arena);
    //destroyFontManager();
    //destroyInput();
    //delete engine;
}