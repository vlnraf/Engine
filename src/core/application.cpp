#include <stdint.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "application.hpp"
#include "tracelog.hpp"

void frameBufferSizeCallback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
    LOGINFO("Window resized %dx%d", width, height);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mod){
    Input* input = (Input*)glfwGetWindowUserPointer(window);
    if (!input) return; 

    if (key >= 0 && key < GLFW_KEY_LAST) { 
        if (action == GLFW_PRESS) {
            input->keys[key] = true;
        } else if (action == GLFW_RELEASE) {
            input->keys[key] = false;
        }
    }
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos){
    //LOGINFO("xpos %f, ypos %f", (float)xpos, (float)ypos);
}

void joystickCallback(int jid, int event){

    if(event == GLFW_CONNECTED){
        Gamepad* gamepad = new Gamepad();
        glfwSetJoystickUserPointer(jid, gamepad);
        gamepad->name = glfwGetJoystickName(jid);
        gamepad->jid = jid;
        LOGINFO("Gamepad id: %d name: %s connected!", jid, gamepad->name);
    }else{
        Gamepad* gamepad = (Gamepad*)glfwGetJoystickUserPointer(jid);
        if (!gamepad) return; 
        LOGINFO("Gamepad id: %d name: %s disconnected!", jid, gamepad->name);
        free(gamepad);
    }
}

FILETIME getFileTime(const char* fileName){
    FILETIME result = {0};
    WIN32_FIND_DATA findData;
    HANDLE dllFile = FindFirstFileA(fileName, &findData);
    if(dllFile != INVALID_HANDLE_VALUE){
        result = findData.ftLastWriteTime;
        FindClose(dllFile);
    }
    return result;
}

// TODO: fare in modo che il nome della dll non sia statico ma venga passato in input
Win32DLL win32LoadGameCode(){
    Win32DLL result = {};
    CopyFile("game.dll", "game_temp.dll", FALSE);
    result.gameCodeDLL = LoadLibraryA("game_temp.dll");

    result.lastWriteTimeOld = getFileTime("game.dll");

    if(result.gameCodeDLL){
        result.gameStart = (GameStart*)GetProcAddress(result.gameCodeDLL, "gameStart");
        result.gameRender = (GameRender*)GetProcAddress(result.gameCodeDLL, "gameRender");
        result.gameUpdate = (GameUpdate*)GetProcAddress(result.gameCodeDLL, "gameUpdate");
        //result.gameReload = (GameReload*)GetProcAddress(result.gameCodeDLL, "gameReload");
        result.gameStop = (GameStop*)GetProcAddress(result.gameCodeDLL, "gameStop");
        result.isValid = (result.gameRender != nullptr) && (result.gameUpdate != nullptr);
        LOGINFO("new DLL attached");
    }
    if(!result.isValid){
        result.gameStart = NULL;
        result.gameRender = NULL;
        LOGERROR("Unable to reload the new DLL");
    }
    return result;
}

void win32UnloadGameCode(Win32DLL* gameCode){
    if(gameCode->gameCodeDLL){
        FreeLibrary(gameCode->gameCodeDLL);
        gameCode->gameCodeDLL = 0;
    }
    gameCode->isValid = false;
    LOGINFO("DLL detached");
}

Win32DLL reloadGame(ApplicationState* app, Win32DLL gameCode){
    FILETIME lastWriteTime = getFileTime("game.dll");

    //NOTE: Am i doing something wrong or just windows has a trash API??
    //it reloads the file twice each time
    if(CompareFileTime(&lastWriteTime, &gameCode.lastWriteTimeOld) != 0){
        win32UnloadGameCode(&gameCode);
        gameCode = win32LoadGameCode();
        app->reload = true;
    }

    return gameCode;
}

void initWindow(ApplicationState* app, const char* name, const uint32_t width, const uint32_t height){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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

    app->window = window;
    app->width = width;
    app->height = height;

    app->engine = initEngine(width, height);

    glfwSetWindowUserPointer(app->window, app->engine->input);

    glfwGetFramebufferSize(app->window, &app->width, &app->height);
    glfwSetFramebufferSizeCallback(app->window, frameBufferSizeCallback);
    glfwSetKeyCallback(app->window, keyCallback);
    glfwSetCursorPosCallback(app->window, cursorPositionCallback);
    glfwSetJoystickCallback(joystickCallback);

    LOGINFO("Renderer successfully initialized");
}

void* updateAndRender(ApplicationState* app, void* gameState, Win32DLL gameCode){
    app->startFrame = glfwGetTime();
    app->dt = app->startFrame - app->lastFrame;
    app->lastFrame = app->startFrame;

    //fps and dt informations
    //LOGINFO("dt: %f - FPS: %.2f", app->dt, 1.0f / app->dt);

    //should i calculate it directly on the engine?
    updateDeltaTime(app->engine, app->dt, 1.0f/app->dt);

    glfwPollEvents();
    registerGamepadInput(app->engine->input);

    clearRenderer(0.2f, 0.3f, 0.3f, 1.0f);

    //int state = glfwGetKey(app->window, GLFW_KEY_F11);
    //if(state == GLFW_PRESS){
    //    gameState = gameCode.gameReload(gameState, &app->engine.renderer, "test");
    //}

    gameCode.gameUpdate(app->engine, gameState, app->dt);
    gameCode.gameRender(app->engine, gameState, app->dt);

    glfwSwapBuffers(app->window);
    app->endFrame = glfwGetTime();
    return gameState;
}

int main(){
    ApplicationState* app = new ApplicationState();
    initWindow(app, "ciao", 1280, 720);

    Win32DLL gameCode =  win32LoadGameCode();

    //void* gameState = (void*) gameCode.gameStart(&app->engine.renderer);
    //app->engine->gameState = gameCode.gameStart(app->engine, app->engine->gameState);
    gameCode.gameStart(app->engine);
    app->lastFrame = glfwGetTime();
    while(!glfwWindowShouldClose(app->window)){
        gameCode = reloadGame(app, gameCode);
        if(app->reload){
            //NOTE: Comment if you need to not reset the state of the game
            gameCode.gameStop(app->engine, app->engine->gameState);
            //app->engine->gameState = (void*) gameCode.gameStart(app->engine, app->engine->gameState);
            gameCode.gameStart(app->engine);
            app->reload = false;
        }
        app->engine->gameState = updateAndRender(app, app->engine->gameState, gameCode);
    }
    LOGINFO("Closing application");
    gameCode.gameStop(app->engine, app->engine->gameState);
    destroyEngine(app->engine);
    glfwTerminate();
    return 0;
}