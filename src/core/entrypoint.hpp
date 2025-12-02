#pragma once

#include "core/application.hpp"
#include "core/tracelog.hpp"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

// ============================================================================
// HAZEL-STYLE ENTRYPOINT PATTERN
// ============================================================================
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

extern ApplicationState* app;

// Only define main() if explicitly requested via EXIS_ENTRYPOINT
#ifdef EXIS_ENTRYPOINT

int main() {
    // Call user-defined createApplication() to setup the application
    ApplicationState appInstance = createApplication();
    //app = &appInstance;

    // Main loop
    while (!applicationShouldClose()) {
        applicationRun();
    }

    // Cleanup
    applicationShutDown();

    return 0;
}

#endif // EXIS_ENTRYPOINT
