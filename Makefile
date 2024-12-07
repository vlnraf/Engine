ifeq ($(OS), Windows_NT)
detected_OS := Windows

#Compilation
CXX = clang++
CXXFLAGS = -target x86_64-windows -Wall -g -O0 -D_CRT_SECURE_NO_WARNINGS #-fno-fast-math # da provare a inserire nel caso si hanno dei problemi con i calcoli metematici 

LDFLAGS = -lgame -lshell32 -lopengl32 -lglfw3 -Xlinker /subsystem:console
LIBS = -L external/glfw
INCLUDE :=-I external/glfw/include -I external -I src
INCLUDE_GAME :=-I src/game -I src -I external/

#Sources
GAME_SRC = \
	src/game/game.cpp \
	src/scene.cpp \
	src/glad.c  #TODO: capire come togliere questa dipendenza

APP_SRC = \
	src/core/application.cpp \
	src/core/input.cpp \

CORE_SRC = \
	src/core/tracelog.cpp \
	src/core/ecs.cpp \
	src/core/camera.cpp \
	src/core/tilemap.cpp \

RENDERING_SRC = \
	src/renderer/shader.cpp \
	src/renderer/renderer.cpp \
	src/renderer/texture.cpp \

UTILITIES_SRC = \
	src/glad.c \


all: core.lib game.dll application.exe 
game: game.dll
core: core.lib

core.lib: ${CORE_SRC} ${RENDERING_SRC} ${UTILITIES_SRC}
	@echo "Cleaning old core.lib"
	del *.o
	del core.lib  
	@echo "Building the core library"
	$(CXX) $(CXXFLAGS) -I external -I src -c $^
	llvm-ar rcs $@ *.o

game.dll: core.lib ${GAME_SRC} 
	@echo "Building the game"
	$(CXX) $(CXXFLAGS) $(INCLUDE_GAME) -L ./ -lcore -DGAME_EXPORT -o $@ $^ -shared -lopengl32
	@echo "Game builded successfull"
	

application.exe: core.lib ${APP_SRC}
	@echo "Building the system"
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(LIBS) -L ./ -lcore $^ -o $@ $(LDFLAGS) 
	@echo "System builded successfull"
	
clean:
	@echo "Cleaning workspace"
	del *.exe 
	del game.*
	del *.pdb
	del *.ilk
	del *.o 
	del core.lib
endif