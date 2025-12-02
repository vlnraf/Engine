#ifndef __EMSCRIPTEN__
#include <glad/glad.h>
#else
#include <GLES3/gl3.h>
#endif

#include <GLFW/glfw3.h>

#include "../core.hpp"
#include "../core/application.hpp"
#include "platform/platform.hpp"
#include "../core/entrypoint.hpp"
//#include "../core/window.hpp"

#define srcGameName "game.dll"

ApplicationState* app;

//TODO: just move this function in input and record my inputs not the GLFW ones
void registerGamepadInput(Input* input){
    Gamepad& gamepad = input->gamepad;
    if(glfwJoystickPresent(gamepad.jid) && glfwJoystickIsGamepad(gamepad.jid)){
        GLFWgamepadstate state;
        if(glfwGetGamepadState(gamepad.jid, &state)){
            for(int button = 0; button < GLFW_GAMEPAD_BUTTON_LAST; button++){
                bool isPressed = state.buttons[button] == GLFW_PRESS;
                if(isPressed){
                    gamepad.buttons[button] = true;
                }else{
                    gamepad.buttons[button] = false;
                }
            }
        }
        gamepad.leftX = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
        gamepad.leftY = -state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
        gamepad.rightX = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
        gamepad.rightY = -state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
        (state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] == true) ? gamepad.trigger[GAMEPAD_AXIS_LEFT_TRIGGER] = true : gamepad.trigger[GAMEPAD_AXIS_LEFT_TRIGGER] = false;
        (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] == true) ? gamepad.trigger[GAMEPAD_AXIS_RIGHT_TRIGGER] = true : gamepad.trigger[GAMEPAD_AXIS_RIGHT_TRIGGER] = false;
    }
}

void updateAndRender(){
    app->startFrame = glfwGetTime();

    windowPollEvents();

    if(isJustPressed(KEYS::F5)){
        app->debugMode = !app->debugMode;
    }


    //fps and dt informations
    //LOGINFO("dt: %f - FPS: %.2f", app->dt, 1.0f / app->dt);

    //should i calculate it directly on the engine?
    //updateDeltaTime(app->engine, app->dt, 1.0f/app->dt);

    registerGamepadInput(getInputState());

    collisionStartFrame();
    //systemUpdateTransformChildEntities(app->engine->ecs);
    //systemUpdateColliderPosition(app->engine->ecs);
    updateCollisions(app->engine->ecs);
    platformGameUpdate(&app->engine->gameArena, app->engine, app->dt);
    systemUpdateTransformChildEntities(app->engine->ecs);
    systemUpdateColliderPosition(app->engine->ecs);
    collisionEndFrame();
    

    //Audio update
    updateAudio();

    if(app->debugMode){
        beginScene(RenderMode::NO_DEPTH);
            beginMode2D(*getActiveCamera());
                renderGrid();
                systemRenderColliders(app->engine->ecs);
            endMode2D();
        endScene();
    }
    ecsEndFrame(app->engine->ecs);

    windowSwapBuffers(&app->window);
    app->endFrame = glfwGetTime();

    // Calculate delta time from complete frame (end to end) for accurate FPS
    app->dt = app->endFrame - app->lastFrame;
    app->lastFrame = app->endFrame;

    updateInputState(app->dt);
    //return gameState;
}

bool applicationShouldClose(){
    return windowShouldClose(&app->window) || app->quit;
}

ApplicationState initApplication(const char* name, int width, int height){
    ApplicationState app = {0};
    app.window = windowCreate(name, width, height);

    LOGINFO("Application successfully initialized");
    app.engine = initEngine(app.window.width, app.window.height);
    if(!app.engine){
        LOGERROR("Engine not initilized");
        return app;
    }
    platformLoadGame(srcGameName);

    platformGameStart(&app.engine->gameArena, app.engine);
    app.lastFrame = glfwGetTime();
    app.quit = false;
    return app;
}

void applicationRun(){
    app->reload = platformReloadGame(srcGameName);
    if(app->reload){
        //NOTE: Comment if you need to not reset the state of the game
        //app->engine->gameState = platformGameStart(app->engine);
        platformGameStart(&app->engine->gameArena, app->engine);
        app->reload = false;
    }
    updateAndRender();
}

void applicationShutDown(){
    LOGINFO("Closing application");
    platformGameStop(&app->engine->gameArena, app->engine);
    platformUnloadGame();  // Unload game DLL before destroying engine
    destroyEngine(app->engine);  // Clean up audio, renderer, and other resources
    glfwTerminate();
}

void applicationRequestQuit(){
    app->quit = true;
}