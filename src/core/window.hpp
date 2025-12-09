#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "coreapi.hpp"

// Opaque window handle (platform-specific implementation)
typedef void* WindowHandle;

struct Window{
    WindowHandle handle;
    int width;
    int height;
};

// Window functions implemented in platform layer
CORE_API Window windowCreate(const char* name, int width, int height);
CORE_API void windowDestroy(Window* window);
CORE_API void windowRequestClose(Window* window);
CORE_API bool windowShouldClose(Window* window);
CORE_API void windowSwapBuffers(Window* window);
CORE_API void windowPollEvents();
CORE_API void windowResize(Window* window, int width, int height);
CORE_API void windowSetUserPointer(Window* window);
CORE_API void windowSetCallbacks(Window* window);

#endif