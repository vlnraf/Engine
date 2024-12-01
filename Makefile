ifeq ($(OS), Windows_NT)
detected_OS := Windows

CXX = clang++
CXXFLAGS = -target x86_64-windows -Wall -g -O0 -D_CRT_SECURE_NO_WARNINGS #-fno-fast-math # da provare a inserire nel caso si hanno dei problemi con i calcoli metematici 

LDFLAGS = -lgame -lshell32 -lopengl32 -lglfw3 -Xlinker /subsystem:console
LIBS = -L external/glfw
INCLUDE :=-I external/glfw/include -I external

all: game.dll application.exe
game: game.dll

game.dll: src/game/game.cpp src/tracelog.cpp src/shader.cpp src/renderer/renderer.cpp src/input.cpp src/scene.cpp src/ecs.cpp src/texture.cpp src/glad.c 
	@echo "Building the game"
	$(CXX) $(CXXFLAGS) -I src/ -I external -DGAME_EXPORT -o $@ $^ -shared -lopengl32
	@echo "Game builded successfull"
	

application.exe: src/application.cpp src/tracelog.cpp src/shader.cpp src/renderer/renderer.cpp src/input.cpp src/scene.cpp src/ecs.cpp src/texture.cpp src/glad.c
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