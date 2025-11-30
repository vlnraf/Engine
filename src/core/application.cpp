#include <stdint.h>

//#include <glad/glad.h>
//#include <glm/glm.hpp>
//#include <GLFW/glfw3.h>

#include "application.hpp"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

struct ApplicationState;

//static ApplicationState* app;
//void logger(){
//    printf("CIAO");
//}

int main(){
    //Arena arena = initArena();
    //app = arenaAllocStruct(&arena, ApplicationState);
    ApplicationState app = initApplication(WINDOW_WIDTH, WINDOW_HEIGHT);
    while(!applicationShouldClose(&app)){
        applicationRun(&app);
    }
    applicationShutDown(&app);
    return 0;
}