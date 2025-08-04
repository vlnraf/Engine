#pragma once

enum MenuItems{
    MENU_PLAY,
    MENU_LOAD,
    MENU_OPTIONS,
    MENU_QUIT,
    MENU_TOTAL
};

struct MenuState{
    glm::vec2 canvasSize;
    int focusItem = 0;
};

void advanceMenu(MenuState* state, int advance);
void drawButton(MenuState* state, const char* text, Font* font, float fontScale, glm::vec4 rect, MenuItems index);
void handleMenuInput(EngineState* engine);
void drawMenu();