#include <stdint.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "application.hpp"
#include "tracelog.hpp"

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mod){
    ApplicationState* appState = (ApplicationState*)glfwGetWindowUserPointer(window);
    if (!appState) return; // Safety check

    if (key >= 0 && key < GLFW_KEY_LAST) { // Ensure the key is within range
            if (action == GLFW_PRESS) {
                appState->input.keys[key] = true;
            } else if (action == GLFW_RELEASE) {
                appState->input.keys[key] = false;
            }
        }
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos){
    //LOGINFO("xpos %f, ypos %f", (float)xpos, (float)ypos);
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
    }
    if(!result.isValid){
        result.gameStart = NULL;
        result.gameRender = NULL;
    }
    return result;
}

void win32UnloadGameCode(Win32DLL* gameCode){
    if(gameCode->gameCodeDLL){
        FreeLibrary(gameCode->gameCodeDLL);
        gameCode->gameCodeDLL = 0;
    }
    gameCode->isValid = false;
}

void initWindow(ApplicationState* app, const char* name, const uint32_t width, const uint32_t height){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "LearnOpenGl-colors", NULL, NULL);
    if(window == NULL){
        LOGERROR("Failed to create GLFW window");
        glfwTerminate();
    }
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
    app->window = window;
    app->width = width;
    app->height = height;
    app->input = { false };

    glfwSetWindowUserPointer(app->window, app);

    glfwGetFramebufferSize(app->window, &app->width, &app->height);
    glfwSetKeyCallback(app->window, keyCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);

    app->renderer.shader = createShader("shaders/default-shader.vs", "shaders/default-shader.fs");
    initRenderer(&app->renderer, width, height);
}

void updateAndRender(ApplicationState* app, Scene* gameState, Win32DLL gameCode){
    FILETIME lastWriteTime = getFileTime("game.dll");

    if(CompareFileTime(&lastWriteTime, &gameCode.lastWriteTimeOld) > 0){
        win32UnloadGameCode(&gameCode);
        gameCode = win32LoadGameCode();
    }

    glfwPollEvents();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gameCode.gameUpdate(gameState, &app->input);
    gameCode.gameRender(gameState, &app->renderer);

    //Rendering code da spostare probabilmente altrove
    //Renderizza tutti gli oggetti presenti nella scena
    // per ora e' statico, ma lo generalizziamo subito
    //glm::mat4 projection = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f, -1.0f, 1.0f);
    //for(int i = 0 ; i < gameState->ecs->entities; i ++){
    //    glm::mat4 transform = glm::mat4(1.0f);
    //    transform = glm::translate(transform, gameState->ecs->components.transforms[i].position);
    //    transform = glm::scale(transform, glm::vec3(50.0f, 50.0f, 0.0f));
    //    setShader(&app->renderer, gameState->ecs->components.sprite[i].shader);
    //    setUniform(&app->renderer.shader, "projection", projection);
    //    setUniform(&app->renderer.shader, "transform", transform);
    //    LOGINFO("%f - %f",  gameState->ecs->components.transforms[i].position.x,
    //                        gameState->ecs->components.transforms[i].position.x);
    //    renderDraw(&app->renderer, gameState->ecs->components.sprite[i].vertices,
    //                               gameState->ecs->components.sprite[i].vertCount);
    //}

    glfwSwapBuffers(app->window);
}

int main(){
    ApplicationState app = {};
    initWindow(&app, "ciao", 1280, 720);

    Win32DLL gameCode =  win32LoadGameCode();
    
    Scene* gameState = (Scene*) gameCode.gameStart("ciao sono l'inizializzazione del gioco!!!", &app.renderer);
    while(!glfwWindowShouldClose(app.window)){
        updateAndRender(&app, gameState, gameCode);
    }

    glfwTerminate();
    return 0;
}