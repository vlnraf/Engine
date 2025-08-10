#include "core.hpp"
#include "mainmenu.hpp"
#include "projectx.hpp"

void advanceMenu(MenuState* state, int advance){
    state->focusItem += advance;
    if(state->focusItem < 0) state->focusItem += MenuItems::MENU_TOTAL;
    state->focusItem = state->focusItem % MenuItems::MENU_TOTAL;
}

void enterMenu(MenuState* state, EngineState* engine){
    if(state->focusItem == MenuItems::MENU_PLAY){
        gameState->gameLevels = GameLevels::FIRST_LEVEL;
        loadLevel(GameLevels::FIRST_LEVEL);
    }
}

void drawButton(MenuState* state, const char* text, Font* font, float fontScale, glm::vec4 rect, MenuItems index){
    glm::vec2 canvasSize = {gameState->camera.width, gameState->camera.height};
    glm::vec2 buttonSize = {rect.z, rect.w};
    glm::vec2 pos = {rect.x - (buttonSize.x / 2), rect.y};
    glm::vec4 color = {0,0,0,0.5f};
    glm::vec4 focusColor = {1,0,0,0.5f};
    glm::vec4 currentColor = color;
    if(state->focusItem == index){
        currentColor = focusColor;
    }
    renderDrawFilledRect(worldToScreen(gameState->camera, glm::vec3(pos, 0)), buttonSize, {0, 0}, currentColor);
    uint32_t fontHeight= round((font->characterSize * fontScale));
    uint32_t fontWidth = calculateTextWidth(font, text, fontScale);
    glm::vec2 fontPos = worldToScreen(gameState->camera, glm::vec3(pos, 0)) + (buttonSize / 2.0f);
    fontPos.x -= (fontWidth / 2);
    fontPos.y -= round((fontHeight / 2));
    renderDrawText2D(font, text, fontPos, fontScale);
}

void handleMenuInput(EngineState* engine){
    MenuState* state = &gameState->menuState;
    if(isJustPressed(KEYS::Down)){
        advanceMenu(state, 1);
    }else if(isJustPressed(KEYS::Up)){
        advanceMenu(state, -1);
    }else if(isJustPressed(KEYS::Enter)){
        enterMenu(state, engine);
    }
}

void drawMenu(){
    glm::vec2 canvasSize = {gameState->camera.width, gameState->camera.height};
    int yo = gameState->camera.height / 2;
    int xo = gameState->camera.width / 2;
    int padding = 10;
    Font* font = getFont("Minecraft");
    float fontScale = 0.3f;
    glm::vec2 buttonSize = {200, 30};


    Texture* controllerTexture = getTexture("Xone");
    renderDrawQuad2D(controllerTexture, worldToScreen({xo - (controllerTexture->size.x / 2), yo - controllerTexture->size.y}, controllerTexture->size, canvasSize), {1,1}, {0,0}, {0,0}, controllerTexture->size);

    drawButton(&gameState->menuState, "Play", font, fontScale, {xo, yo, buttonSize.x, buttonSize.y}, MenuItems::MENU_PLAY);
    yo += buttonSize.y + padding;
    drawButton(&gameState->menuState, "Load save", font, fontScale, {xo, yo, buttonSize.x, buttonSize.y}, MenuItems::MENU_LOAD);
    yo += buttonSize.y + padding;
    drawButton(&gameState->menuState, "Options", font, fontScale, {xo, yo, buttonSize.x, buttonSize.y}, MenuItems::MENU_OPTIONS);
    yo += buttonSize.y + padding;
    drawButton(&gameState->menuState, "Quit", font, fontScale, {xo, yo, buttonSize.x, buttonSize.y}, MenuItems::MENU_QUIT);
}
