ifeq ($(OS), Windows_NT)
detected_OS := Windows

#Compilation
CXX = clang++
CXXFLAGS = -target x86_64-windows -Wall -g -O0 -D_CRT_SECURE_NO_WARNINGS #-fno-fast-math # da provare a inserire nel caso si hanno dei problemi con i calcoli metematici 

LDFLAGS = -lgame -lshell32 -lopengl32 -lglfw3 -Xlinker /subsystem:console
LIBS = -L external/glfw
INCLUDE :=-I external/glfw/include -I external -I src/core -I src/renderer -I src
INCLUDE_GAME :=-I src/core -I src/game -I src/renderer -I external -I src

#Sources
GAME_SRC = \
	src/game/game.cpp 

APP_SRC = \
	src/core/application.cpp

CORE_SRC = \
	src/core/tracelog.cpp \
	src/core/tracelog.cpp \
	src/core/input.cpp \
	src/core/ecs.cpp 

RENDERING_SRC = \
	src/renderer/shader.cpp \
	src/renderer/renderer.cpp \
	src/renderer/texture.cpp 

UTILITIES_SRC = \
	src/glad.c \
	src/scene.cpp 


all: game.dll application.exe
game: game.dll

game.dll: ${GAME_SRC} ${RENDERING_SRC} ${UTILITIES_SRC} ${CORE_SRC}
	@echo "Building the game"
	$(CXX) $(CXXFLAGS) $(INCLUDE_GAME) -DGAME_EXPORT -o $@ $^ -shared -lopengl32
	@echo "Game builded successfull"
	

application.exe: ${CORE_SRC} ${RENDERING_SRC} ${UTILITIES_SRC} ${APP_SRC}
	@echo "Building the system"
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(LIBS) $^ -o $@ $(LDFLAGS) 
	@echo "System builded successfull"

	
clean:
	@echo "Cleaning workspace"
	del *.exe 
	del game.*
	del *.pdb
	del *.ilk
endif