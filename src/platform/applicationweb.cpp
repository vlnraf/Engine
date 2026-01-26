#include <GLFW/glfw3.h>
#include <emscripten.h>
//#include <glm/glm.hpp>
//
//#include "application.hpp"
//#include "input.hpp"
//#include "tracelog.hpp"

#include "../core.hpp"
#include "../core/application.hpp"
#include "platform/platform.hpp"

CORE_API ApplicationState* app = nullptr;

void registerGamepadInput(Input* input){
    // Gamepad not supported on web - glfwGetGamepadState not available in Emscripten
    // Input will work with keyboard/mouse only
}

void frameBufferSizeCallback(GLFWwindow* glfwWindow, int width, int height){
    Window* window = (Window*)glfwGetWindowUserPointer(glfwWindow);
    if(!window) return;

    window->width = width;
    window->height = height;

    setRenderResolution(width, height);
    OrtographicCamera* gameCamera = getActiveCamera();
    if(gameCamera){
        updateCameraAspectRatio(gameCamera, (float)width, (float)height);
    }
    LOGINFO("Window resized %dx%d", width, height);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mod){
    Input* input = getInputState();
    if (!input) return;

    if (key >= 0 && key < GLFW_KEY_LAST) {
        if (action == GLFW_PRESS) {
            input->keys[key] = true;
        } else if (action == GLFW_RELEASE) {
            input->keys[key] = false;
        }
    }
}

void mouseCallback(GLFWwindow* window, int button, int action, int mods){
    Input* input = getInputState();
    if (!input) return;

    if(action == GLFW_PRESS){
        input->mouseButtons[button] = true;
    }else if(action == GLFW_RELEASE){
        input->mouseButtons[button] = false;
    }
}

void cursorPositionCallback(GLFWwindow* glfwWindow, double xpos, double ypos){
    Window* window = (Window*)glfwGetWindowUserPointer(glfwWindow);
    Input* input = getInputState();
    if (!input || !window) return;
    input->mousePos = {xpos, (float)window->height - ypos};
    LOGINFO("Mouse pos %.0fx%.0f", input->mousePos.x, input->mousePos.y);
}

void joystickCallback(int jid, int event){
    if(event == GLFW_CONNECTED){
        Input* input = getInputState();
        glfwSetJoystickUserPointer(jid, &input->gamepad);
        input->gamepad.name = glfwGetJoystickName(jid);
        input->gamepad.jid = jid;
        LOGINFO("Gamepad id: %d name: %s connected!", jid, input->gamepad.name);
    }else{
        Gamepad* gamepad = (Gamepad*)glfwGetJoystickUserPointer(jid);
        if (!gamepad) return;
        LOGINFO("Gamepad id: %d name: %s disconnected!", jid, gamepad->name);
        free(gamepad);
    }
}

// ============================================================================
// Window API implementation for Web
// ============================================================================

Window windowCreate(const char* name, int width, int height){
    Window result = {0};
    result.width = width;
    result.height = height;

    glfwInit();

    // WebGL 2.0 context (OpenGL ES 3.0)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

    GLFWwindow* window = glfwCreateWindow(width, height, name, NULL, NULL);
    if(window == NULL){
        LOGERROR("Failed to create GLFW window");
        glfwTerminate();
        return result;
    }
    result.handle = window;
    LOGINFO("Window successfully initialized");

    //To focus canvas
    EM_ASM({
        Module.canvas.tabIndex = 1;
        Module.canvas.focus();
    });

    glfwMakeContextCurrent(window);
    // Emscripten provides OpenGL ES functions directly - no glad needed
    LOGINFO("WebGL context initialized");

    glfwSwapInterval(0); // Disable vsync

    return result;
}

void windowDestroy(Window* window){
    if(!window || !window->handle) return;
    glfwDestroyWindow((GLFWwindow*)window->handle);
    window->handle = nullptr;
}

void windowRequestClose(Window* window){
    if(!window || !window->handle) return;
    glfwSetWindowShouldClose((GLFWwindow*)window->handle, true);
}

bool windowShouldClose(Window* window){
    if(!window || !window->handle) return true;
    return glfwWindowShouldClose((GLFWwindow*)window->handle);
}

void windowSwapBuffers(Window* window){
    if(!window || !window->handle) return;
    glfwSwapBuffers((GLFWwindow*)window->handle);
}

void windowPollEvents(){
    glfwPollEvents();
}

void updateAndRender(){
    app->startFrame = glfwGetTime();

    // Poll events first to process any pending input
    windowPollEvents();

    registerGamepadInput(getInputState());

    collisionStartFrame();
    updateCollisions(app->engine->ecs);
    platformGameUpdate(&app->engine->gameArena, app->engine, app->dt);
    systemUpdateTransformChildEntities(app->engine->ecs);
    systemUpdateColliderPosition(app->engine->ecs);
    collisionEndFrame();

    // Audio update (stubbed for web)
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

    // Update input state at END of frame so callbacks that fire between frames
    // will be properly detected in the next frame (prev=0, curr=1)
    updateInputState(app->dt);
}

// Emscripten main loop callback
void mainLoop(void* app){
    //app = (ApplicationState*) app;
    updateAndRender();
}

bool applicationShouldClose(){
    return windowShouldClose(&app->window) || app->quit;
}

ApplicationState initApplication(const char* name, int width, int height){
    ApplicationState app = {0};
    app.window = windowCreate(name, width, height);
    app.engine = initEngine(app.window.width, app.window.height);
    if(!app.engine){
        LOGERROR("Engine not initialized");
        return app;
    }

    platformGameStart(&app.engine->gameArena, app.engine);
    app.lastFrame = glfwGetTime();
    return app;
}

void applicationRun(){
    // Emscripten main loop - non-blocking
    emscripten_set_main_loop_arg(mainLoop, (void*)app, 0, 1);
}

void applicationShutDown(){
    LOGINFO("Closing application");
    emscripten_cancel_main_loop();
    platformGameStop(&app->engine->gameArena, app->engine);
    platformUnloadGame();  // Unload game DLL before destroying engine
    destroyEngine(app->engine);  // Clean up audio, renderer, and other resources
    glfwTerminate();
}

void applicationRequestQuit(){
    app->quit = true;
}

void applicationSetResolution(int width, int height){
    //No option in web build
}

void windowSetUserPointer(Window* window){
    if(!window || !window->handle) return;
    glfwSetWindowUserPointer((GLFWwindow*) window->handle, window);
}

void windowSetCallbacks(Window* window){
    if(!window || !window->handle) return;
    GLFWwindow* w = (GLFWwindow*) window->handle;
    glfwSetFramebufferSizeCallback(w, frameBufferSizeCallback);
    glfwSetKeyCallback(w, keyCallback);
    glfwSetMouseButtonCallback(w, mouseCallback);
    glfwSetCursorPosCallback(w, cursorPositionCallback);
    glfwSetJoystickCallback(joystickCallback);
}