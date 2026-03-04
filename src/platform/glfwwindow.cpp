#include <GLFW/glfw3.h>

#include "../core.hpp"
#include "../core/window.hpp"
#include "../core/tracelog.hpp"

void frameBufferSizeCallback(GLFWwindow* window, int width, int height){
    //ApplicationState* app = (ApplicationState*)glfwGetWindowUserPointer(window);
    //if(!app) return;

    // Update renderer resolution and recreate screen camera
    // Viewport is automatically managed by the render flow (beginTextureMode/endTextureMode)
    setRenderResolution(width, height);

    // Update game camera to maintain aspect ratio
    OrtographicCamera* gameCamera = getActiveCamera();
    if(gameCamera){
        updateCameraAspectRatio(gameCamera, (float)width, (float)height);
    }

    Window* windowData = (Window*)glfwGetWindowUserPointer(window);
    glfwSetWindowSize((GLFWwindow*)windowData->handle, width, height);
    windowData->width = width;
    windowData->height = height;
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
    //ApplicationState* app = (ApplicationState*)glfwGetWindowUserPointer(window);
    Window* windowData = (Window*)glfwGetWindowUserPointer(window);
    Input* input = getInputState();
    if (!input) return;
    //int width, height;
    //glfwGetWindowSize(window, &width, &height);
    input->mousePos = {xpos, windowData->height - ypos};
    //LOGINFO("Mouse pos %.0fx%.0f", input->mousePos.x, input->mousePos.y);
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

Window windowCreate(const char* name, int width, int height){
    Window result = {0};
    result.width = width;
    result.height = height;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, name, NULL, NULL);
    if(window == NULL){
        LOGERROR("Failed to create GLFW window");
        glfwTerminate();
        return result;
    }
    result.handle = window;
    LOGINFO("Window successfully initialized");

    // Defining a monitor
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	// Putting it in the centre
	glfwSetWindowPos(window, mode->width/7, mode->height/7);

    glfwMakeContextCurrent(window);
    LOGINFO("GLAD successfully initialized");

    glfwSwapInterval(0); //Disable vsync

    //glfwSetWindowUserPointer(window, &result);

    //glfwGetFramebufferSize(window, &result.width, &result.height);

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

void windowResize(Window* window, int width, int height){
    window->width = width;
    window->height = height;
    glfwSetWindowSize((GLFWwindow*)window->handle, width, height);
}

void windowPollEvents(){
    glfwPollEvents();
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