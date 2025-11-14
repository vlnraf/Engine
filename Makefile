ifeq ($(OS),Windows_NT)
	detected_OS := Windows
	PLATFORM_SRC = src/platform/platformwindows.cpp
	APP_NAME := application.exe

    CFLAGS += -DPLATFORM_WINDOWS
	PLATFORM_LIBS = -lshell32 -lopengl32 -lglfw3
	SHARED_EXT = dll
	REMOVE = del
    COPY = copy
    COPY_TO = .

else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
		APP_NAME := application
        PLATFORM_SRC = src/platform/platformlinux.c
        CFLAGS += -DPLATFORM_LINUX
		PLATFORM_LIBS = -ldl -lGL -lglfw
		SHARED_EXT = so
		REMOVE = rm -f
        COPY = cp
    	COPY_TO = .
    endif
endif

#Compilation
CXX = clang++ -std=c++14
CXXFLAGS = -m64 -W -Wall -Wno-missing-field-initializers -g -O0 -D_CRT_SECURE_NO_WARNINGS $(CFLAGS)# -march=native #-fno-fast-math # da provare a inserire nel caso si hanno dei problemi con i calcoli metematici 

# LDFLAGS = -lgame -lshell32 -lopengl32 -lglfw3 -Xlinker /subsystem:console
LIBS = -L external/libs/glfw -L external/libs/fmod -L external/libs/freetype
INCLUDE :=-I external/glfw/include -I external -I src -I external/fmod/core/inc 
INCLUDE_GAME :=-I src/game -I src -I external/ 

#Sources
GAME_SRC = \
	src/game/*.cpp \

APP_SRC = \
	$(PLATFORM_SRC) \
	src/core/application.cpp

CORE_SRC = \
	src/core/arena.cpp \
	src/core/engine.cpp \
	src/core/audioengine.cpp \
	src/core/tracelog.cpp \
	src/core/ecs.cpp \
	src/core/input.cpp \
	src/core/profiler.cpp \
	src/core/camera.cpp \
	src/core/serialization.cpp \
	src/core/animationmanager.cpp \
	src/core/colliders.cpp \
	src/core/tilemap.cpp \
	src/core/ui.cpp \

RENDERING_SRC = \
	src/renderer/shader.cpp \
	src/renderer/renderer.cpp \
	src/renderer/texture.cpp \
	src/renderer/fontmanager.cpp \

UTILITIES_SRC = \
	src/glad.c \


	


all: core.$(SHARED_EXT) game.$(SHARED_EXT) $(APP_NAME)
#game: game.dll
#core: core.dll
#kit: kit.dll

############################
# Compile glad.c as C
############################
src/glad.o: src/glad.c
	$(CXX) $(CXXFLAGS) -I external -c $< -o $@

ifeq ($(OS),Windows_NT)
copy_libs:
	@echo "Copying required libraries..."
	cmd /c copy /Y "external\\libs\\glfw\\glfw3.dll" .
	cmd /c copy /Y "external\\libs\\fmod\\fmodL.dll" .
else
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
copy_libs:
	@echo "Copying required libraries..."
	cp external/libs/glfw/libglfw.so .
	cp external/libs/fmod/libfmodL.so .
endif
endif

#NOTE: -lfmodL_vc is the debug version which print every error, just swap to -lfmod_vc for the realease build!!!
core.$(SHARED_EXT): ${CORE_SRC} ${RENDERING_SRC} src/glad.o
	@echo "Cleaning old core.dll"
	$(REMOVE) *.o
	$(REMOVE) core.$(SHARED_EXT)  
	@echo "Building the core library"
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(LIBS) -lfreetype -lfmodL_vc -DCORE_EXPORT -o $@ $^ -shared

game.$(SHARED_EXT): ${GAME_SRC} 
	$(REMOVE) game.pdb
	@echo "Building the game"
	$(CXX) $(CXXFLAGS) $(INCLUDE_GAME) $(LIBS) -lfreetype -DGAME_EXPORT -o $@ -lcore $^ -shared 
	@echo "Game builded successfull"
	

$(APP_NAME): ${APP_SRC} src/glad.o
	@echo "Building the system"
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(LIBS) -lfreetype $^ -o $@ $(PLATFORM_LIBS) -lcore
	$(MAKE) copy_libs
	@echo "System built successfully"
	@echo "System builded successfull"
	
clean:
	$(REMOVE) application application.exe
	$(REMOVE) *.dll *.so *.o *.pdb *.ilk 
	$(REMOVE) core.* game.* kit.*