#pragma once

#include "core/application.hpp"
#include "core/tracelog.hpp"

// This file provides an OPTIONAL main() entry point following Hazel's pattern.
//
// USAGE OPTION 1: Use the entrypoint (recommended for simple cases)
//   - Define EXIS_ENTRYPOINT before including this file
//   - Implement createApplication() to setup and return your ApplicationState
//   - main() is provided automatically
//   - Example:
//     #define EXIS_ENTRYPOINT
//     #include "core/entrypoint.hpp"
//
// USAGE OPTION 2: Write your own main() (for custom control)
//   - Don't define EXIS_ENTRYPOINT
//   - Don't include this file (or include without the define)
//   - Write your own main() in application.cpp with full custom logic
//   - You can still use core.dll in other projects this way
// ============================================================================

// Forward declaration - implement this in your application.cpp if using EXIS_ENTRYPOINT
extern ApplicationState createApplication();

extern CORE_API ApplicationState* app;

// Only define main() if explicitly requested via EXIS_ENTRYPOINT
#ifdef EXIS_ENTRYPOINT

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

int main() {
    // Allocate application on heap and assign to global pointer (managed by core.dll)
    app = new ApplicationState(createApplication());

    // Set window user pointer now that app is in its final location
    // This must be done after the ApplicationState is stored in the global pointer
    // to prevent dangling pointers in GLFW callbacks
    windowSetUserPointer(&app->window);
    windowSetCallbacks(&app->window);

    while (!applicationShouldClose()) {
        applicationRun();
    }

    // Cleanup
    applicationShutDown();
    delete app;
    app = nullptr;

    return 0;
}

#endif // EXIS_ENTRYPOINT
