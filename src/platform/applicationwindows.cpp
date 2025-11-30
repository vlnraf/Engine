#include "../core/application.hpp"

#define srcGameName "game.dll"

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

    // Update renderer resolution and recreate screen camera
    // Viewport is automatically managed by the render flow (beginTextureMode/endTextureMode)
    setRenderResolution(width, height);

    // Update game camera to maintain aspect ratio
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

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos){
    ApplicationState* app = (ApplicationState*)glfwGetWindowUserPointer(window);
    Input* input = getInputState();
    if (!input) return;
    //int width, height;
    //glfwGetWindowSize(window, &width, &height);
    input->mousePos = {xpos, app->height - ypos};
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
    //if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
    //    LOGERROR("Failied to initialize GLAD");
    //    glfwTerminate();
    //}
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
    app->startFrame = glfwGetTime();

    glfwPollEvents();

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

    glfwSwapBuffers(app->window);
    app->endFrame = glfwGetTime();

    // Calculate delta time from complete frame (end to end) for accurate FPS
    app->dt = app->endFrame - app->lastFrame;
    app->lastFrame = app->endFrame;

    updateInputState(app->dt);
    //return gameState;
}

bool applicationShouldClose(ApplicationState* app){
    return glfwWindowShouldClose(app->window);
}

ApplicationState initApplication(int width, int height){
    ApplicationState app = {0};
    initWindow(&app, "Prototype 1", width, height);
    app.engine = initEngine(app.width, app.height);
    if(!app.engine){
        LOGERROR("Engine not initilized");
        return app;
    }
    platformLoadGame(srcGameName);

    platformGameStart(&app.engine->gameArena, app.engine);
    app.lastFrame = glfwGetTime();
    return app;
}

void applicationRun(ApplicationState* app){
    app->reload = platformReloadGame(srcGameName);
    if(app->reload){
        //NOTE: Comment if you need to not reset the state of the game
        //app->engine->gameState = platformGameStart(app->engine);
        platformGameStart(&app->engine->gameArena, app->engine);
        app->reload = false;
    }
    updateAndRender(app);
}

void applicationShutDown(ApplicationState* app){
    LOGINFO("Closing application");
    platformGameStop(&app->engine->gameArena, app->engine);
    platformUnloadGame();  // Unload game DLL before destroying engine
    destroyEngine(app->engine);  // Clean up audio, renderer, and other resources
    glfwTerminate();
}