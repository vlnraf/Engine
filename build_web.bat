@echo off
emcc -std=c++14 -O2 ^
  -s USE_GLFW=3 ^
  -s FULL_ES3=1 ^
  -s USE_FREETYPE=1 ^
  -s ALLOW_MEMORY_GROWTH=1 ^
  -s TOTAL_MEMORY=256MB ^
  -DPLATFORM_WEB ^
  --preload-file assets ^
  --preload-file shaders ^
  --preload-file map ^
  -I./src ^
  -I./external ^
  -I./external/glm ^
  src/application/application.cpp ^
  src/core/arena.cpp ^
  src/core/audioengineweb.cpp ^
  src/core/camera.cpp ^
  src/core/ecs.cpp ^
  src/core/engine.cpp ^
  src/core/input.cpp ^
  src/core/profiler.cpp ^
  src/core/serialization.cpp ^
  src/core/tilemap.cpp ^
  src/core/tracelog.cpp ^
  src/core/ui.cpp ^
  src/core/animationmanager.cpp ^
  src/core/colliders.cpp ^
  src/core/mystring.cpp ^
  src/renderer/fontmanager.cpp ^
  src/renderer/renderer.cpp ^
  src/renderer/shader.cpp ^
  src/renderer/texture.cpp ^
  src/game/lifetime.cpp ^
  src/game/mainmenu.cpp ^
  src/game/player.cpp ^
  src/game/projectile.cpp ^
  src/game/projectx.cpp ^
  src/game/telegraphattack.cpp ^
  src/game/vampireclone.cpp ^
  src/game/weapon.cpp ^
  src/platform/platformweb.cpp ^
  src/platform/applicationweb.cpp ^
  -o game.html

echo Build complete! Run: python -m http.server 8080
