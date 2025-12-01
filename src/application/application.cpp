#include <stdint.h>

//#include <glad/glad.h>
//#include <glm/glm.hpp>
//#include <GLFW/glfw3.h>

#include "../core/application.hpp"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

static ApplicationState s_app;

// Quit callback that core.dll will call
static void onQuitRequested(){
    s_app.quit = true;
}

int main(){
    s_app = initApplication(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Set up the quit callback for core.dll
    applicationSetQuitCallback(onQuitRequested);

    while(!applicationShouldClose(&s_app)){
        applicationRun(&s_app);
    }

    applicationShutDown(&s_app);
    return 0;
}