#include <stdint.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "application.hpp"
#include "input.hpp"
#include "tracelog.hpp"

#define srcGameName "game.dll"

//static ApplicationState* app;


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

void frameBufferSizeCallback(GLFWwindow* window, int width, int height){
    ApplicationState* app = (ApplicationState*)glfwGetWindowUserPointer(window);
    if(!app) return;
    //glViewport(0, 0, width, height);
    updateEngineWindowSize(app->engine, width, height);
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

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos){
    Input* input = getInputState();
    if (!input) return;
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    input->mousePos = {xpos, ypos};
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

void initWindow(ApplicationState* app, const char* name, const uint32_t width, const uint32_t height){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, name, NULL, NULL);
    if(window == NULL){
        LOGERROR("Failed to create GLFW window");
        glfwTerminate();
    }
    LOGINFO("Window successfully initialized");
    // Defining a monitor
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	// Putting it in the centre
	glfwSetWindowPos(window, mode->width/7, mode->height/7);

    glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        LOGERROR("Failied to initialize GLAD");
        glfwTerminate();
    }
    LOGINFO("GLAD successfully initialized");

    glfwSwapInterval(0); //Disable vsync

    app->window = window;
    app->width = width;
    app->height = height;

    glfwSetWindowUserPointer(window, app);

    glfwGetFramebufferSize(app->window, &app->width, &app->height);
    glfwSetFramebufferSizeCallback(app->window, frameBufferSizeCallback);
    glfwSetKeyCallback(app->window, keyCallback);
    glfwSetMouseButtonCallback(app->window, mouseCallback);
    glfwSetCursorPosCallback(app->window, cursorPositionCallback);
    glfwSetJoystickCallback(joystickCallback);

    LOGINFO("Application successfully initialized");
}

void updateAndRender(ApplicationState* app){
    updateInputState();
    glfwPollEvents();

    if(isJustPressed(KEYS::F5)){
        app->engine->debugMode = !app->engine->debugMode;
    }

    app->startFrame = glfwGetTime();
    app->dt = app->startFrame - app->lastFrame;
    app->lastFrame = app->startFrame;

    //fps and dt informations
    //LOGINFO("dt: %f - FPS: %.2f", app->dt, 1.0f / app->dt);

    //should i calculate it directly on the engine?
    updateDeltaTime(app->engine, app->dt, 1.0f/app->dt);

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

    if(app->engine->debugMode){
        beginScene(RenderMode::NO_DEPTH);
            beginMode2D(app->engine->mainCamera);
                renderGrid();
                systemRenderColliders(app->engine->ecs);
            endMode2D();
        endScene();
    }
    ecsEndFrame(app->engine->ecs);

    glfwSwapBuffers(app->window);
    app->endFrame = glfwGetTime();
    //return gameState;
}

int main(){
    PROFILER_SAVE("prof.json");
    //Arena appArena = initArena(); //NOTE: default memory is 4 MB
    //app = arenaAllocStruct(&appArena, ApplicationState);
    ApplicationState app = {};
    initWindow(&app, "Prototype 1", 640, 320);

    app.engine = initEngine(app.width, app.height);
    if(!app.engine){
        LOGERROR("Engine not initilized");
        return 1;
    }

    platformLoadGame(srcGameName);

    platformGameStart(&app.engine->gameArena, app.engine);
    app.lastFrame = glfwGetTime();
    while(!glfwWindowShouldClose(app.window)){
        app.reload = platformReloadGame(srcGameName);
        if(app.reload){
            //NOTE: Comment if you need to not reset the state of the game
            //app->engine->gameState = platformGameStart(app->engine);
            platformGameStart(&app.engine->gameArena, app.engine);
            app.reload = false;
        }
        updateAndRender(&app);
    }
    LOGINFO("Closing application");
    platformGameStop(&app.engine->gameArena, app.engine);
    //NOTE: slow down when i close the game, the OS will free memory anyway
    //destroyEngine(app->engine);
    PROFILER_CLEANUP();
    glfwTerminate();
    //clearArena(&appArena);
    //destroyArena(&appArena);
    return 0;
}