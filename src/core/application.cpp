#include <stdint.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
        result.isValid = result.gameRender;
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
    app->input = initInput();

    LOGWARN("GLFW callbacks slow");
    glfwSetWindowUserPointer(app->window, &app->input);
    LOGWARN("GLFW callbacks set userPointer slow");

    glfwGetFramebufferSize(app->window, &app->width, &app->height);
    glfwSetFramebufferSizeCallback(app->window, frameBufferSizeCallback);
    glfwSetKeyCallback(app->window, keyCallback);
    glfwSetCursorPosCallback(app->window, cursorPositionCallback);
    glfwSetJoystickCallback(joystickCallback);
    LOGWARN("GLFW callbacks slow");

    app->renderer = initRenderer(width, height);
    app->renderer->shader = createShader("shaders/default-shader.vs", "shaders/default-shader.fs");
    LOGINFO("Renderer successfully initialized");
}

Win32DLL updateAndRender(ApplicationState* app, void* gameState, Win32DLL gameCode){
    app->startFrame = glfwGetTime();
    app->dt = app->startFrame - app->lastFrame;
    app->lastFrame = app->startFrame;

    //fps and dt informations
    LOGINFO("dt: %f - FPS: %.2f", app->dt, 1.0f / app->dt);

    FILETIME lastWriteTime = getFileTime("game.dll");

    if(CompareFileTime(&lastWriteTime, &gameCode.lastWriteTimeOld) > 0){
        win32UnloadGameCode(&gameCode);
        gameCode = win32LoadGameCode();
    }

    glfwPollEvents();
    registerGamepadInput(&app->input);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gameCode.gameUpdate(gameState, &app->input, app->dt);
    gameCode.gameRender(gameState, app->renderer);

    glfwSwapBuffers(app->window);
    app->endFrame = glfwGetTime();
    return gameCode;
}

int main(){
    ApplicationState* app = new ApplicationState();
    initWindow(app, "ciao", 1280, 720);

    Win32DLL gameCode =  win32LoadGameCode();

    void* gameState = (void*) gameCode.gameStart(app->renderer);
    app->lastFrame = glfwGetTime();
    while(!glfwWindowShouldClose(app->window)){
        gameCode = updateAndRender(app, gameState, gameCode);
    }
    LOGINFO("Closing application");
    glfwTerminate();
    return 0;
}