#include "core.hpp"

// Opt-in to using the Hazel-style entrypoint pattern
// This gives us main() automatically by calling createApplication()
#define EXIS_ENTRYPOINT
#include "core/entrypoint.hpp"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

// Implement the createApplication() function required by the entrypoint
ApplicationState createApplication(){
    //Arena arena = initArena();
    //ApplicationState* app = arenaAllocStructZero(&arena, ApplicationState);
    ApplicationState app = initApplication("Prototype 1", WINDOW_WIDTH, WINDOW_HEIGHT);
    //app->window = t.window;
    //app->engine = t.engine;
    //app->lastFrame = t.lastFrame;
    return app;
}