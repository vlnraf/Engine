#include "platform.hpp"
#include "core.hpp"
#include <stdio.h>

bool platform_load_game(const char* dllName){ 
    (void)dllName;
    LOGINFO("Not implemented on this platform");
    return false;
}
void platform_unload_game(void){
    LOGINFO("Not implemented on this platform");
}
bool platform_reload_game_if_needed(ApplicationState* app, const char* dllName){
    (void)app; (void)dllName;
    return false;
}