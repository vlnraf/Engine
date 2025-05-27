ifeq ($(OS), Windows_NT)
detected_OS := Windows

#Compilation
CXX = clang++ -std=c++14
CXXFLAGS = -target x86_64-windows -W -Wall -g -O0 -D_CRT_SECURE_NO_WARNINGS #-fno-fast-math # da provare a inserire nel caso si hanno dei problemi con i calcoli metematici 

LDFLAGS = -lgame -lshell32 -lopengl32 -lglfw3 -Xlinker /subsystem:console
LIBS = -L external/glfw -L external/fmod/core/lib/x64
INCLUDE :=-I external/glfw/include -I external -I src -I external/fmod/core/inc
INCLUDE_GAME :=-I src/game -I src -I external/ 

#Sources
GAME_SRC = \
	src/game/*.cpp \
	src/glad.c 

APP_SRC = \
	src/core/application.cpp \
	#src/core/input.cpp \

CORE_SRC = \
	src/core/engine.cpp \
	src/core/audioengine.cpp \
	src/core/tracelog.cpp \
	src/core/ecs.cpp \
	src/core/input.cpp \
	src/core/profiler.cpp \
	src/core/camera.cpp \
	src/core/serialization.cpp \
	src/core/tilemap.cpp \
	src/core/animationmanager.cpp \
	src/core/colliders.cpp \
	src/core/ui.cpp \

RENDERING_SRC = \
	src/renderer/shader.cpp \
	src/renderer/renderer.cpp \
	src/renderer/texture.cpp \
	src/renderer/fontmanager.cpp \

#GAME_KIT = \
#	src/gamekit/animationmanager.cpp \
#	src/gamekit/colliders.cpp \

UTILITIES_SRC = \
	src/glad.c \


all: core.dll game.dll application.exe #kit.dll
game: game.dll
core: core.dll
#kit: kit.dll

#NOTE: -lfmodL_vc is the debug version which print every error, just swap to -lfmod_vc for the realease build!!!
core.dll: ${CORE_SRC} ${RENDERING_SRC} ${UTILITIES_SRC}
	@echo "Cleaning old core.dll"
	del *.o
	del core.dll  
	@echo "Building the core library"
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(LIBS) -I external -I src -lfreetype -lfmodL_vc -DCORE_EXPORT -o $@ $^ -shared

#core.lib: ${CORE_SRC} ${RENDERING_SRC} ${UTILITIES_SRC}
#	@echo "Cleaning old core.lib"
#	del *.o
#	del core.lib  
#	@echo "Building the core library"
#	$(CXX) $(CXXFLAGS) -I external -I src -c $^
#	llvm-ar rcs $@ *.o

#kit.dll: core.lib ${GAME_KIT}
#	@echo "Building the GameKit"
#	$(CXX) $(CXXFLAGS) $(INCLUDE_GAME) -L ./ -lcore -lfreetype -DKIT_EXPORT -o $@ $^ -shared -lopengl32
#	@echo "GameKit builded successfull"

#NOTE: -lfreetype should be compiled with the core library, but right now i can't because it's static
# 		try to find a method or just build the core library as dll
#		it's the same for kit.dll and application.exe they should not link the library by themself

game.dll: ${GAME_SRC} 
	del game.pdb
	@echo "Building the game"
	$(CXX) $(CXXFLAGS) $(INCLUDE_GAME) -L ./ -lfreetype -DGAME_EXPORT -o $@ -lcore $^ -shared -lopengl32 
	@echo "Game builded successfull"
	

application.exe: ${APP_SRC} ${UTILITIES_SRC}
	@echo "Building the system"
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(LIBS) -L ./ -lfreetype $^ -o $@ $(LDFLAGS) -lcore
	@echo "System builded successfull"
	
clean:
	@echo "Cleaning workspace"
	del *.exe 
	del game_temp.dll
	del game.*
	del *.pdb
	del *.ilk
	del *.o 
	del core.*
	del kit.*
endif