#include "platform/platform.hpp"
#include "core.hpp"
#include <windows.h>
#include <string.h>

struct Win32DLL{
    HMODULE gameCodeDLL;

    FILETIME lastWriteTimeOld;
    bool isValid;
};

static Win32DLL gameCode = {};

GameStart*  platformGameStart  = NULL;
GameRender* platformGameRender = NULL;
GameUpdate* platformGameUpdate = NULL;
GameStop*   platformGameStop   = NULL;

FILETIME getFileTime(const char* fileName){
    FILETIME result = {};
    WIN32_FIND_DATA findData;
    HANDLE dllFile = FindFirstFileA(fileName, &findData);
    if(dllFile != INVALID_HANDLE_VALUE){
        result = findData.ftLastWriteTime;
        FindClose(dllFile);
    }
    return result;
}

void platformUnloadGame(){
    if(gameCode.gameCodeDLL){
        FreeLibrary(gameCode.gameCodeDLL);
        gameCode.gameCodeDLL = 0;
    }
    gameCode.isValid = false;
    LOGINFO("DLL detached");
}

void platformLoadGame(const char* dllName){
    //Win32DLL result = {};
    //TODO: don't use game_temp.dll directly but map it to a variable or constant
    CopyFile(dllName, "game_temp.dll", FALSE);
    gameCode.gameCodeDLL = LoadLibraryA("game_temp.dll");
    gameCode.lastWriteTimeOld = getFileTime(dllName);

    if(gameCode.gameCodeDLL){
        platformGameStart = (GameStart*)GetProcAddress(gameCode.gameCodeDLL, "gameStart");
        platformGameRender = (GameRender*)GetProcAddress(gameCode.gameCodeDLL, "gameRender");
        platformGameUpdate = (GameUpdate*)GetProcAddress(gameCode.gameCodeDLL, "gameUpdate");
        //gameCode.gameReload = (GameReload*)GetProcAddress(gameCode.gameCodeDLL, "gameReload");
        platformGameStop = (GameStop*)GetProcAddress(gameCode.gameCodeDLL, "gameStop");
        gameCode.isValid = (platformGameRender != nullptr) && (platformGameUpdate != nullptr);
        LOGINFO("new DLL attached");
    }
    if(!gameCode.isValid){
        platformGameStart = NULL;
        platformGameRender = NULL;
        platformGameUpdate = NULL;
        platformGameStop = NULL;
        LOGERROR("Unable to reload the new DLL");
    }
    //return result;
}

bool platformReloadGame(ApplicationState* app, const char* dllName){
    FILETIME lastWriteTime = getFileTime(dllName);

    //NOTE: Am i doing something wrong or just windows has a trash API??
    //it reloads the file twice each time
    if(CompareFileTime(&lastWriteTime, &gameCode.lastWriteTimeOld) != 0){
        //NOTE: Because windows is trash and modify the timestemp when he writes the file and also when he finish to write it
        //      so if you don't wait a bit it will reload the dll two times
        Sleep(300);

        platformUnloadGame();
        platformLoadGame(dllName);
        return true;
    }
    return false;
}