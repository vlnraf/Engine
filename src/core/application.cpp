#include <stdint.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "application.hpp"
#include "input.hpp"
#include "tracelog.hpp"

#define srcGameName "game.dll"

static ApplicationState* app;


#if defined(_WIN32)
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
#else
void registerGamepadInput(Input* input){}
#endif

void frameBufferSizeCallback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
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
    //LOGINFO("xpos %f, ypos %f", (float)xpos, (float)ypos);
}

void joystickCallback(int jid, int event){

    if(event == GLFW_CONNECTED){
        //Gamepad* gamepad = new Gamepad();
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

#if defined(_WIN32)

FILETIME getFileTime(const char* fileName){
    FILETIME result = {};
    WIN32_FIND_DATA findData;
    HANDLE dllFile = FindFirstFileA(fileName, &findData);
    if(dllFile != INVALID_HANDLE_VALUE){
        result = findData.ftLastWriteTime;
        FindClose(dllFile);
    }
    return result;
}

// TODO: fare in modo che il nome della dll non sia statico ma venga passato in input
void win32LoadGameCode(Win32DLL* result, const char* dllName){
    //Win32DLL result = {};
    //TODO: don't use game_temp.dll directly but map it to a variable or constant
    CopyFile(dllName, "game_temp.dll", FALSE);
    result->gameCodeDLL = LoadLibraryA("game_temp.dll");
    result->lastWriteTimeOld = getFileTime(dllName);

    if(result->gameCodeDLL){
        result->gameStart = (GameStart*)GetProcAddress(result->gameCodeDLL, "gameStart");
        result->gameRender = (GameRender*)GetProcAddress(result->gameCodeDLL, "gameRender");
        result->gameUpdate = (GameUpdate*)GetProcAddress(result->gameCodeDLL, "gameUpdate");
        //result.gameReload = (GameReload*)GetProcAddress(result.gameCodeDLL, "gameReload");
        result->gameStop = (GameStop*)GetProcAddress(result->gameCodeDLL, "gameStop");
        result->isValid = (result->gameRender != nullptr) && (result->gameUpdate != nullptr);
        LOGINFO("new DLL attached");
    }
    if(!result->isValid){
        result->gameStart = NULL;
        result->gameRender = NULL;
        LOGERROR("Unable to reload the new DLL");
    }
    //return result;
}

void win32UnloadGameCode(Win32DLL* gameCode){
    if(gameCode->gameCodeDLL){
        FreeLibrary(gameCode->gameCodeDLL);
        gameCode->gameCodeDLL = 0;
    }
    gameCode->isValid = false;
    LOGINFO("DLL detached");
}

void reloadGame(ApplicationState* app, Win32DLL* gameCode, const char* dllName){
    FILETIME lastWriteTime = getFileTime(dllName);

    //NOTE: Am i doing something wrong or just windows has a trash API??
    //it reloads the file twice each time
    if(CompareFileTime(&lastWriteTime, &gameCode->lastWriteTimeOld) != 0){
        //NOTE: Because windows is trash and modify the timestemp when he writes the file and also when he finish to write it
        //      so if you don't wait a bit it will reload the dll two times
        Sleep(300);

        win32UnloadGameCode(gameCode);
        win32LoadGameCode(gameCode, dllName);
        app->reload = true;
    }
    //return gameCode;
}
#else
void getFileTime(const char* fileName){}
void win32LoadGameCode(Win32DLL* result, const char* dllName){
    result->gameStart  = gameStart;
    result->gameRender = gameRender;
    result->gameUpdate = gameUpdate;
    result->gameStop   = gameStop;
    result->isValid    = true;
}
void win32UnloadGameCode(Win32DLL* gameCode){}
void reloadGame(ApplicationState* app, Win32DLL* gameCode, const char* dllName){
    app->reload = false;
}

#include <emscripten.h>
void* updateAndRender(ApplicationState* app, void* gameState, Win32DLL gameCode);

void main_loop(void* arg) {
    Win32DLL* gameCode = (Win32DLL*) arg;
    //if (glfwWindowShouldClose(app->window)) {
    //    emscripten_cancel_main_loop(); // Optional: only if you want to stop
    //    return;
    //}
    reloadGame(app, gameCode, srcGameName);
    if(app->reload){
        app->reload = false;
    }
    app->engine->gameState = updateAndRender(app, app->engine->gameState, *gameCode);
}
#endif

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
    #ifndef __EMSCRIPTEN__
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	// Putting it in the centre
	glfwSetWindowPos(window, mode->width/7, mode->height/7);
    #endif

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

    glfwGetFramebufferSize(app->window, &app->width, &app->height);
    glfwSetFramebufferSizeCallback(app->window, frameBufferSizeCallback);
    glfwSetKeyCallback(app->window, keyCallback);
    glfwSetMouseButtonCallback(app->window, mouseCallback);
    glfwSetCursorPosCallback(app->window, cursorPositionCallback);
    glfwSetJoystickCallback(joystickCallback);

    LOGINFO("Application successfully initialized");
}

void updateAndRender(ApplicationState* app, void* gameState, Win32DLL gameCode){
    //NOTE: update last keyboard state?
    //memcpy(input->keysPrevFrame, input->keys, sizeof(input->keys)); //350 are the keys states watch input.hpp
    //memcpy(input->gamepad.buttonsPrevFrame, input->gamepad.buttons, sizeof(input->gamepad.buttons));
    updateInputState();

    app->startFrame = glfwGetTime();
    app->dt = app->startFrame - app->lastFrame;
    app->lastFrame = app->startFrame;

    //fps and dt informations
    //LOGINFO("dt: %f - FPS: %.2f", app->dt, 1.0f / app->dt);

    //should i calculate it directly on the engine?
    updateDeltaTime(app->engine, app->dt, 1.0f/app->dt);

    glfwPollEvents();
    registerGamepadInput(getInputState());

    //clearColor(0.2f, 0.3f, 0.3f, 1.0f);

    //int state = glfwGetKey(app->window, GLFW_KEY_F11);
    //if(state == GLFW_PRESS){
    //    gameState = gameCode.gameReload(gameState, &app->engine.renderer, "test");
    //}

    startFrame();
    systemUpdateColliderPosition(app->engine->ecs);
    systemUpdateTransformChildEntities(app->engine->ecs);
    systemCheckCollisions(app->engine->ecs, 0);
    gameCode.gameUpdate(app->engine, app->dt);
    endFrame();
    //gameCode.gameRender(app->engine, gameState, app->dt);

    //Audio update
    //NOTE: should it be done here or in the game loop?
    updateAudio();

    glfwSwapBuffers(app->window);
    app->endFrame = glfwGetTime();
    //return gameState;
}

int main(){
    PROFILER_SAVE("prof.json");
    Arena* appArena = initArena(); //NOTE: default memory is 4 MB
    app = arenaAllocStruct(appArena, ApplicationState);
    //app = new ApplicationState();
    initWindow(app, "Prototype 1", 1280, 720);

    app->engine = initEngine(app->width, app->height);
    if(!app->engine){
        LOGERROR("Engine not initilized");
        return 1;
    }


    Win32DLL gameCode = {};
    win32LoadGameCode(&gameCode, srcGameName);

    //void* gameState = (void*) gameCode.gameStart(&app->engine.renderer);
    //app->engine->gameState = gameCode.gameStart(app->engine, app->engine->gameState);
    app->engine->gameState = gameCode.gameStart(app->engine);
    #if defined(_WIN32)
    app->lastFrame = glfwGetTime();
    while(!glfwWindowShouldClose(app->window)){
        reloadGame(app, &gameCode, srcGameName);
        if(app->reload){
            //NOTE: Comment if you need to not reset the state of the game
            //gameCode.gameStop(app->engine, app->engine->gameState);
            //gameCode.gameStart(app->engine);
            app->engine->gameState = gameCode.gameStart(app->engine);
            app->reload = false;
        }
        updateAndRender(app, app->engine->gameState, gameCode);
    }
    #else
    LOGINFO("emscripten");
    emscripten_set_main_loop_arg(main_loop, &gameCode, 0, 1);
    #endif
    LOGINFO("Closing application");
    gameCode.gameStop(app->engine, app->engine->gameState);
    //NOTE: slow down when i close the game, the OS will free memory anyway
    //destroyEngine(app->engine);
    PROFILER_CLEANUP();
    glfwTerminate();
    clearArena(appArena);
    return 0;
}