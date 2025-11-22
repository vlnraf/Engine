# Engine Documentation

A high-performance 2D game engine with hot-reloadable game code, ECS architecture, and arena-based memory management.

---

## Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [Hot Reload System](#hot-reload-system)
3. [Memory Management](#memory-management)
4. [ECS (Entity Component System)](#ecs-entity-component-system)
5. [Rendering System](#rendering-system)
6. [Input System](#input-system)
7. [Collision System](#collision-system)
8. [Animation System](#animation-system)
9. [Audio System](#audio-system)
10. [UI System](#ui-system)
11. [Tilemap System](#tilemap-system)
12. [Best Practices](#best-practices)

---

## Architecture Overview

### Project Structure

```
Engine/
├── src/
│   ├── core/           # Engine framework (ECS, arena, input, collision)
│   ├── renderer/       # OpenGL rendering, textures, fonts
│   ├── platform/       # Platform-specific code (Windows/Linux)
│   ├── game/           # Game implementation (DLL)
│   └── gamekit/        # Export macros
```

### Design Philosophy

This engine follows these core principles:

1. **Data-Oriented Design** - Cache-friendly memory layouts
2. **Arena Allocation** - Predictable memory management
3. **Hot Reload** - Edit code while the game runs
4. **ECS Architecture** - Flexible component-based entities
5. **Explicit > Implicit** - Clear, visible operations

---

## Hot Reload System

### How It Works

The engine uses **DLL hot-reloading** to allow you to edit game code while the engine runs:

1. Engine detects when `game.dll` changes
2. Unloads old DLL
3. Loads new DLL with updated code
4. Game state persists across reload

### The Game Arena

The engine provides a **persistent arena** for game memory:

```cpp
// platform.hpp
typedef void GameStart(Arena* gameArena);
typedef void GameUpdate(Arena* gameArena, EngineState* engine, float dt);
typedef void GameStop(Arena* gameArena);
```

**CRITICAL CONVENTION:** GameState must be the **first allocation** in `gameStart()`:

```cpp
GAME_API void gameStart(Arena* gameArena) {
    // FIRST allocation - GameState
    GameState* state = arenaPushStruct(gameArena, GameState);

    // Initialize
    state->currentLevel = MAIN_MENU;
    state->paused = false;

    // Other permanent allocations can follow
    MyData* data = arenaPushArray(gameArena, MyData, 100);
}
```

### Accessing GameState

In any game function, retrieve GameState from the arena:

```cpp
GAME_API void gameUpdate(Arena* gameArena, EngineState* engine, float dt) {
    GameState* state = (GameState*)gameArena->base;  // First allocation

    if (state->paused) return;
    // ...
}
```

Or use a helper macro:

```cpp
#define getGameState(arena) ((GameState*)(arena)->base)

GameState* state = getGameState(gameArena);
```

### What's Safe for Hot Reload

#### ✓ SAFE - Survives Reload

```cpp
struct GameState {
    // Plain data types
    int health = 100;
    float speed = 5.0f;
    bool active = true;

    // Enums
    GameLevel currentLevel;

    // Fixed-size arrays
    Card cards[6];
    char playerName[64];

    // Structs of plain data
    MenuState menuState;

    // Entity handles (IDs, not pointers)
    Entity player;
    Entity enemies[100];
};

// Arena allocations
Enemy* pool = arenaPushArray(gameArena, Enemy, 1000);
```

#### ❌ UNSAFE - Breaks on Reload

```cpp
struct GameState {
    // Heap allocations
    std::vector<Enemy> enemies;     // ❌ Internal heap pointer
    std::string playerName;          // ❌ Heap-allocated
    std::map<int, Item> inventory;   // ❌ Heap-allocated

    // Raw pointers (unless from arena)
    Enemy* enemyList = new Enemy[100];  // ❌ Heap pointer

    // Classes with constructors/destructors
    std::unique_ptr<Data> data;     // ❌ RAII won't work
};
```

### Using malloc/new (Advanced)

**You CAN use `malloc`/`new` if you accept the trade-off:**

```cpp
struct GameState {
    int health;
    // ...

    // You can do this, but...
    std::vector<TempData> tempBuffer;  // ⚠️ Will break on hot reload
};
```

**What happens:**
- ✓ Game works normally
- ✓ Can use familiar C++ containers
- ❌ Hot reload will crash or corrupt data
- ❌ Must restart application to reload code

**When to use malloc/new:**
- Rapid prototyping (don't care about hot reload)
- Final builds (hot reload disabled)
- Transient data cleared before reload

**When to use arenas:**
- Data that must survive hot reload
- Performance-critical paths
- Shipping builds

### Hot Reload Best Practices

```cpp
// ✓ Good: Arena-based, hot-reload safe
struct GameState {
    GameLevel currentLevel;
    bool paused;
    Entity player;
};

// ✓ Good: Entities in ECS (already persistent)
Entity player = createEntity(ecs);
addComponent(ecs, player, HealthComponent, {.hp = 100});

// ⚠️ Acceptable: Transient data (recreate on reload)
void drawMenu() {
    static MenuState menuState = {};  // Local static, reset on reload is fine
    // ...
}

// ❌ Bad: Heap pointers in GameState
struct GameState {
    Enemy* enemies = new Enemy[100];  // Breaks on reload!
};
```

---

## Memory Management

### Arena Allocators

The engine uses **arena allocators** (linear allocators) for predictable, fast memory management.

#### Key Concepts

- **Linear allocation** - O(1), just bump a pointer
- **No individual frees** - Clear entire arena at once
- **Cache-friendly** - Sequential memory layout
- **No fragmentation** - Memory is contiguous

#### Arena API

```cpp
// Create arena (default 4MB)
Arena arena = initArena();
Arena largeArena = initArena(GIGABYTES(1));

// Allocate memory
void* memory = arenaPush(&arena, sizeInBytes);

// Allocate typed
MyStruct* s = arenaPushStruct(&arena, MyStruct);
int* array = arenaPushArray(&arena, int, 100);

// Zero-initialized
MyStruct* s = arenaPushStructZero(&arena, MyStruct);

// Clear all allocations (keep capacity)
clearArena(&arena);

// Destroy arena
destroyArena(&arena);
```

#### Arena Types in Engine

| Arena | Lifetime | Purpose |
|-------|----------|---------|
| **Engine Arena** | Application lifetime | Engine state, ECS |
| **Game Arena** | Application lifetime | GameState, persistent game data |
| **Frame Arena** | Per frame | Collision grid, temporary buffers |
| **ECS Arena** | Application lifetime | Entity/component data |

#### Example Usage

```cpp
// Permanent allocation
Arena* gameArena = getGameArena();
EnemyPool* enemies = arenaPushArray(gameArena, Enemy, 1000);

// Transient per-frame allocation
Arena* frameArena = getFrameArena();
TempBuffer* buffer = arenaPushArray(frameArena, char, 4096);
// Automatically cleared next frame

// Manual arena for subsystem
Arena audioArena = initArena(MEGABYTES(10));
SoundBuffer* sounds = arenaPushArray(&audioArena, SoundBuffer, 50);
```

### Initialization Without Constructors

Arena allocations **don't call C++ constructors**. You must initialize manually:

```cpp
struct MyStruct {
    int value = 100;  // ❌ Won't be set with arena allocation!
};

// Allocate and initialize
MyStruct* s = arenaPushStructZero(&arena, MyStruct);  // Zero memory
s->value = 100;  // Explicit initialization

// Or use init function
void initMyStruct(MyStruct* s) {
    s->value = 100;
    s->active = true;
}

MyStruct* s = arenaPushStruct(&arena, MyStruct);
initMyStruct(s);
```

---

## ECS (Entity Component System)

### Overview

The engine uses a custom **sparse-set ECS** for O(1) component access and cache-friendly iteration.

### Core Concepts

- **Entity** - Just an ID (uint32_t)
- **Component** - Plain data struct
- **System** - Function that operates on entities with specific components

### Creating Entities

```cpp
// Create entity
Entity entity = createEntity(ecs);

// Add components
TransformComponent transform = {
    .position = {100, 200, 0},
    .scale = {1, 1, 0},
    .rotation = {0, 0, 0}
};
pushComponent(ecs, entity, TransformComponent, &transform);

SpriteComponent sprite = {
    .texture = getTexture("player"),
    .index = {0, 0},
    .size = {32, 32}
};
pushComponent(ecs, entity, SpriteComponent, &sprite);

// Remove entity (and all its components)
removeEntity(ecs, entity);
```

### Defining Components

```cpp
// In your header
struct HealthComponent {
    float hp;
    float maxHp;
};

// Declare component type
ECS_DECLARE_COMPONENT(HealthComponent);

// In your cpp (usually gameStart)
registerComponent(ecs, HealthComponent);
```

### Querying Entities

```cpp
// Get all entities with Transform AND Sprite components
EntityArray entities = view(ecs,
    ECS_TYPE(TransformComponent),
    ECS_TYPE(SpriteComponent)
);

for (size_t i = 0; i < entities.count; i++) {
    Entity e = entities.entities[i];

    TransformComponent* transform = getComponent(ecs, e, TransformComponent);
    SpriteComponent* sprite = getComponent(ecs, e, SpriteComponent);

    // Use components...
}
```

### Component Access

```cpp
// Get component (returns NULL if not found)
HealthComponent* health = getComponent(ecs, entity, HealthComponent);
if (health) {
    health->hp -= 10;
}

// Check if entity has component
if (hasComponent(ecs, entity, PlayerTag)) {
    // Do player-specific logic
}

// Remove component
removeComponent(ecs, entity, HealthComponent);
```

### Built-in Components

The engine provides standard components:

```cpp
TransformComponent  // position, scale, rotation
SpriteComponent     // texture, sprite index, size
VelocityComponent   // velocity vector
DirectionComponent  // direction vector
AnimationComponent  // animation state
Box2DCollider       // collision box
Parent              // parent entity
Child               // child entities (up to 10)
```

### Example System

```cpp
void moveSystem(Ecs* ecs, float dt) {
    EntityArray entities = view(ecs,
        ECS_TYPE(TransformComponent),
        ECS_TYPE(VelocityComponent),
        ECS_TYPE(DirectionComponent)
    );

    for (size_t i = 0; i < entities.count; i++) {
        Entity e = entities.entities[i];

        TransformComponent* transform = getComponent(ecs, e, TransformComponent);
        VelocityComponent* velocity = getComponent(ecs, e, VelocityComponent);
        DirectionComponent* direction = getComponent(ecs, e, DirectionComponent);

        transform->position += direction->dir * velocity->vel * dt;
    }
}
```

### Parent-Child Relationships

```cpp
// Create parent
Entity parent = createEntity(ecs);

// Create child
Entity child = createEntity(ecs);

// Link them
Parent parentComp = {.entity = parent};
pushComponent(ecs, child, Parent, &parentComp);

// Parent automatically gets Child component updated

// Destroy parent (children are destroyed too if implemented)
removeEntity(ecs, parent);
```

---

## Rendering System

### Overview

The engine uses **OpenGL 4.2** with batched rendering for performance.

- Batch up to **150,000 quads** per frame
- Automatic texture atlasing
- Y-sorting for 2D depth
- Custom shaders support

### Drawing Sprites

```cpp
// Basic sprite rendering happens automatically via ECS
Entity player = createEntity(ecs);

TransformComponent transform = {
    .position = {100, 200, 0},
    .scale = {2, 2, 0}  // 2x scale
};

SpriteComponent sprite = {
    .texture = getTexture("player"),
    .index = {0, 0},        // Sprite sheet index
    .size = {32, 32},       // Source size in texture
    .tileSize = {16, 16},   // Individual tile size
    .visible = true,
    .ySort = false,         // Y-sorting for depth
    .layer = 1.0f          // Render layer (higher = front)
};

pushComponent(ecs, player, TransformComponent, &transform);
pushComponent(ecs, player, SpriteComponent, &sprite);

// Entities with Transform + Sprite render automatically
```

### Manual Drawing

```cpp
// Draw filled rectangle
renderDrawFilledRect(
    glm::vec3(100, 100, 0),  // Position
    glm::vec2(50, 50),       // Size
    glm::vec3(0, 0, 0),      // Rotation
    glm::vec4(1, 0, 0, 1)    // Color (RGBA)
);

// Draw line
renderDrawLine(
    glm::vec3(0, 0, 0),      // Start
    glm::vec3(100, 100, 0),  // End
    glm::vec4(1, 1, 1, 1)    // Color
);

// Draw text
Font* font = getFont("Roboto-Regular");
renderDrawText2D(
    font,
    "Hello World",
    glm::vec2(100, 100),
    0.5f  // Scale
);
```

### Render Loop

```cpp
// Begin scene
beginScene(camera, RenderMode::NORMAL);

    // All rendering here
    systemRenderSprites(ecs);
    renderTileMap(&tileMap);
    renderDrawFilledRect(...);

endScene();

// UI rendering (separate pass)
beginUiFrame(glm::vec2(0, 0), glm::vec2(screenWidth, screenHeight));

    UiText("Score: 100", {10, 10}, 0.3f);
    if (UiButton("Play", {100, 100}, {80, 30})) {
        // Button clicked
    }

endUiFrame();
```

### Textures

```cpp
// Load texture (in gameStart)
loadTexture("player");       // Loads assets/textures/player.png
loadTexture("enemy");

// Get texture
Texture* tex = getTexture("player");

// Texture is automatically managed by engine
```

### Fonts

```cpp
// Load font (in gameStart)
loadFont("Roboto-Regular");  // Loads assets/fonts/Roboto-Regular.ttf

// Get font
Font* font = getFont("Roboto-Regular");

// Render text
renderDrawText2D(font, "Hello", {100, 100}, 0.5f);

// Calculate text width
float width = calculateTextWidth(font, "Hello", 0.5f);
```

### Camera

```cpp
// Create camera
OrtographicCamera camera = createCamera(
    glm::vec3(0, 0, 0),  // Position
    640,                 // Width
    320                  // Height
);

// Follow target
followTarget(&camera, targetPosition);

// Manual control
camera.position.x += 10;
updateCamera(&camera);
```

### Y-Sorting

For 2D depth illusion (closer to bottom = rendered on top):

```cpp
SpriteComponent sprite = {
    .texture = getTexture("tree"),
    .ySort = true,  // Enable Y-sorting
    .layer = 1.0f
};
```

---

## Input System

### Keyboard

```cpp
#include "input.hpp"

// Check if key is currently pressed
if (isPressed(KEYS::W)) {
    player.y += speed * dt;
}

// Check if key was just pressed this frame
if (isJustPressed(KEYS::SPACE)) {
    playerJump();
}

// Check if key was pressed last frame
if (wasPressed(KEYS::ESCAPE)) {
    // ...
}

// Common keys
KEYS::W, KEYS::A, KEYS::S, KEYS::D
KEYS::SPACE, KEYS::ENTER, KEYS::ESCAPE
KEYS::LEFT, KEYS::RIGHT, KEYS::UP, KEYS::DOWN
```

### Mouse

```cpp
Input* input = getInputState();

// Mouse position
glm::vec2 mousePos = input->mousePos;

// Mouse buttons
if (input->mouseButtons[MOUSE_BUTTON_LEFT]) {
    // Left click
}

if (input->mouseButtons[MOUSE_BUTTON_RIGHT]) {
    // Right click
}
```

### Gamepad

```cpp
// Check button
if (isJustPressedGamepad(GAMEPAD_BUTTON_A)) {
    playerJump();
}

// Analog sticks
Input* input = getInputState();
float leftX = input->gamepad.leftX;   // -1 to 1
float leftY = input->gamepad.leftY;   // -1 to 1
float rightX = input->gamepad.rightX;
float rightY = input->gamepad.rightY;

// Triggers
bool leftTrigger = input->gamepad.trigger[GAMEPAD_AXIS_LEFT_TRIGGER];
bool rightTrigger = input->gamepad.trigger[GAMEPAD_AXIS_RIGHT_TRIGGER];

// Gamepad buttons
GAMEPAD_BUTTON_A, GAMEPAD_BUTTON_B
GAMEPAD_BUTTON_X, GAMEPAD_BUTTON_Y
GAMEPAD_BUTTON_START, GAMEPAD_BUTTON_BACK
GAMEPAD_BUTTON_DPAD_UP, GAMEPAD_BUTTON_DPAD_DOWN
```

---

## Collision System

### Overview

The engine uses a **spatial hash grid** for efficient broad-phase collision detection.

- 32x32 pixel cells
- Trigger and collision events
- Event deduplication (enter/stay/exit)

### Adding Colliders

```cpp
Entity entity = createEntity(ecs);

// Add transform
TransformComponent transform = {.position = {100, 100, 0}};
pushComponent(ecs, entity, TransformComponent, &transform);

// Add collider
Box2DCollider collider = {
    .type = Box2DCollider::DYNAMIC,  // or STATIC
    .offset = {0, 0},
    .size = {32, 32},
    .isTrigger = false  // true for triggers (no physics response)
};
pushComponent(ecs, entity, Box2DCollider, &collider);
```

### Collision Events

```cpp
// Get collision events (entities physically colliding)
CollisionEventArray* collisions = getCollisionEvents();

for (size_t i = 0; i < collisions->count; i++) {
    CollisionEvent event = collisions->item[i];
    Entity entityA = event.entityA.entity;
    Entity entityB = event.entityB.entity;

    if (hasComponent(ecs, entityA, PlayerTag) &&
        hasComponent(ecs, entityB, EnemyTag)) {
        // Player hit enemy
    }
}
```

### Trigger Events

```cpp
// Get trigger events (overlap detection, no physics)
TriggerEventArray* triggers = getTriggerEvents();

for (size_t i = 0; i < triggers->count; i++) {
    CollisionEvent event = triggers->item[i];
    Entity entityA = event.entityA.entity;
    Entity entityB = event.entityB.entity;

    if (hasComponent(ecs, entityA, PlayerTag) &&
        hasComponent(ecs, entityB, PickupTag)) {
        // Player entered pickup zone
        removeEntity(ecs, entityB);
    }
}
```

### Collision Types

```cpp
Box2DCollider::STATIC   // Doesn't move (walls, obstacles)
Box2DCollider::DYNAMIC  // Moves (player, enemies, projectiles)
```

### Setting Up Spatial Grid

The collision system automatically centers the grid on a target:

```cpp
// In your update loop
TransformComponent* playerTransform = getComponent(ecs, player, TransformComponent);
setGridCenter(playerTransform->position.x, playerTransform->position.y);

// Collision system updates automatically
```

---

## Animation System

### Loading Animations

```cpp
// Define animation
Animation playerIdle = {
    .frames = 4,
    .frameDuration = 0.2f,
    .loop = true,
    .indices = {{0, 0}, {1, 0}, {2, 0}, {3, 0}}  // Sprite sheet positions
};

// Register animation
registerAnimation("player_idle", playerIdle);
```

### Animating Entities

```cpp
Entity player = createEntity(ecs);

// Add sprite component
SpriteComponent sprite = {
    .texture = getTexture("player"),
    .size = {32, 32},
    .tileSize = {16, 16}
};
pushComponent(ecs, player, SpriteComponent, &sprite);

// Add animation component
AnimationComponent anim = {};
strncpy(anim.animationId, "player_idle", sizeof(anim.animationId));
anim.currentFrame = 0;
anim.elapsedTime = 0;
pushComponent(ecs, player, AnimationComponent, &anim);

// Animation system updates automatically
animationSystem(ecs, dt);
```

### Changing Animations

```cpp
AnimationComponent* anim = getComponent(ecs, player, AnimationComponent);

if (playerMoving) {
    strncpy(anim->animationId, "player_walk", sizeof(anim->animationId));
} else {
    strncpy(anim->animationId, "player_idle", sizeof(anim->animationId));
}

// Reset to frame 0
anim->currentFrame = 0;
anim->elapsedTime = 0;
```

---

## Audio System

### Loading Audio

```cpp
// In gameStart
loadAudio("sfx/music.wav", true);   // true = loop
loadAudio("sfx/jump.wav", false);   // false = one-shot
```

### Playing Audio

```cpp
// Play with volume (0.0 - 1.0)
playAudio("sfx/music.wav", 0.5f);   // 50% volume

// Play sound effect
playAudio("sfx/jump.wav", 1.0f);    // Full volume
```

### Audio Update

The engine automatically updates audio each frame. No manual intervention needed.

---

## UI System

### Overview

The engine provides an **immediate-mode UI system** similar to Dear ImGui.

### UI Frame

```cpp
// Begin UI rendering
beginUiFrame(
    glm::vec2(0, 0),                                    // Offset
    glm::vec2(screenWidth, screenHeight)               // Size
);

    // All UI code here
    UiText("Hello", {10, 10}, 0.3f);

    if (UiButton("Play", {100, 100}, {80, 30})) {
        startGame();
    }

endUiFrame();
```

### UI Elements

#### Text

```cpp
Font* font = getFont("Roboto-Regular");
setFontUI(font);

UiText(
    "Score: 100",
    glm::vec2(10, 10),  // Position
    0.3f                // Scale
);
```

#### Button

```cpp
if (UiButton(
    "Play",                      // Label
    glm::vec2(100, 100),        // Position
    glm::vec2(80, 30),          // Size
    glm::vec2(0, 0)             // Optional offset
)) {
    // Button was clicked
    startGame();
}
```

#### Image

```cpp
UiImage(
    getTexture("logo"),
    glm::vec2(100, 100),    // Position
    glm::vec2(200, 100),    // Size
    glm::vec2(0, 0),        // Source offset in texture
    glm::vec2(200, 100)     // Source size in texture
);
```

### Text Utilities

```cpp
Font* font = getFontUI();

// Calculate text width
float width = calculateTextWidth(font, "Hello World", 0.3f);

// Center text
float textWidth = calculateTextWidth(font, "Title", 0.5f);
float centerX = (screenWidth / 2) - (textWidth / 2);
UiText("Title", {centerX, 100}, 0.5f);
```

---

## Tilemap System

### Loading Tilemaps

```cpp
// Load from Tiled JSON export
TileMap map = LoadTilesetFromTiled("level1", ecs);
```

### Rendering Tilemaps

```cpp
TileMap bgMap = LoadTilesetFromTiled("background", ecs);

// In render loop
renderTileMap(&bgMap);
```

### Animating Tiles

```cpp
// Update animated tiles
animateTiles(&bgMap, dt);
```

### Creating Tilemaps

```cpp
// Create tileset
TileSet tileset = createTileSet(
    getTexture("tileset"),
    16,  // Tile width
    16   // Tile height
);

// Create tilemap
std::vector<int> tiles = {1, 2, 3, 4, 5, 6, ...};
TileMap map = createTilemap(
    tiles,
    32,    // Map width in tiles
    32,    // Map height in tiles
    16.0f, // Tile size
    tileset
);
```

---

## Best Practices

### Memory Management

1. **Use arenas for persistent data**
   ```cpp
   Enemy* enemies = arenaPushArray(gameArena, Enemy, 1000);
   ```

2. **Use ECS for runtime game objects**
   ```cpp
   Entity player = createEntity(ecs);
   addComponent(ecs, player, HealthComponent, {.hp = 100});
   ```

3. **Avoid malloc/new in GameState** (breaks hot reload)

4. **Use handles, not raw pointers**
   ```cpp
   struct GameState {
       Entity player;  // ✓ Handle
       // Enemy* enemy;  // ❌ Raw pointer
   };
   ```

### ECS Usage

1. **Components are data-only** (no methods)
   ```cpp
   struct HealthComponent {
       float hp;
       float maxHp;
       // No methods!
   };
   ```

2. **Systems are pure functions**
   ```cpp
   void healthRegenSystem(Ecs* ecs, float dt) {
       EntityArray entities = view(ecs, ECS_TYPE(HealthComponent));
       for (...) {
           // Update health
       }
   }
   ```

3. **Use tags for identification**
   ```cpp
   struct PlayerTag {};  // Empty struct
   registerComponent(ecs, PlayerTag);

   if (hasComponent(ecs, entity, PlayerTag)) {
       // Is player
   }
   ```

### Hot Reload Safety

1. **Keep GameState simple**
   ```cpp
   struct GameState {
       GameLevel currentLevel;
       bool paused;
       Entity player;
       // Keep it minimal!
   };
   ```

2. **Use ECS for complex data**

3. **Initialize explicitly**
   ```cpp
   GameState* state = arenaPushStructZero(arena, GameState);
   state->currentLevel = MAIN_MENU;
   state->paused = false;
   ```

### Performance

1. **Batch similar operations**
   ```cpp
   // Good: Process all enemies at once
   EntityArray enemies = view(ecs, ECS_TYPE(EnemyTag));
   for (...) { /* process */ }
   ```

2. **Use spatial partitioning** (collision grid does this automatically)

3. **Minimize state changes** (textures, shaders)

4. **Profile hot paths**
   ```cpp
   PROFILER_START();
   expensiveFunction();
   PROFILER_END();
   ```

### Code Organization

1. **One component per file**
   ```cpp
   // player.hpp
   struct PlayerComponent { ... };
   Entity createPlayer(Ecs* ecs);
   void playerUpdate(Ecs* ecs, float dt);
   ```

2. **Systems are functions, not classes**

3. **Group related components**
   ```cpp
   // weapons.hpp
   struct GunComponent { ... };
   struct ShotgunComponent { ... };
   // All weapon code in one place
   ```

---

## Platform Support

### Current Status

| Platform | Status | Notes |
|----------|--------|-------|
| **Windows** | ✅ Full support | Hot reload works |
| **Linux** | 🚧 Partial | Hot reload stub |

### Building

```bash
# Windows
make

# Linux (when implemented)
make -f Makefile.linux
```

---

## Debugging

### Debug Mode

Press **F5** during gameplay to toggle debug visualization:

- Collision boxes (green)
- Spatial grid (blue)
- Entity counts

### Logging

```cpp
#include "tracelog.hpp"

LOGINFO("Player spawned at (%f, %f)", x, y);
LOGWARN("Low health: %d", health);
LOGERROR("Failed to load texture: %s", filename);
```

### Profiling

```cpp
// Scope profiling
PROFILER_SCOPE_START("myFunction");
expensiveFunction();
PROFILER_SCOPE_END();

// Manual profiling
PROFILER_START();
// Code to profile
PROFILER_END();

// Save profiling data
PROFILER_SAVE("profile.json");
PROFILER_CLEANUP();
```

---

## Example Game Loop

```cpp
GAME_API void gameStart(Arena* gameArena) {
    // First allocation - GameState
    GameState* state = arenaPushStructZero(gameArena, GameState);
    state->currentLevel = MAIN_MENU;

    // Load assets
    loadTexture("player");
    loadFont("Roboto-Regular");
    loadAudio("sfx/music.wav", true);

    // Register components
    registerComponent(ecs, PlayerTag);
    registerComponent(ecs, HealthComponent);

    // Play music
    playAudio("sfx/music.wav", 0.3f);
}

GAME_API void gameUpdate(Arena* gameArena, EngineState* engine, float dt) {
    GameState* state = (GameState*)gameArena->base;
    Ecs* ecs = engine->ecs;

    // Clear background
    clearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // Update systems
    inputSystem(ecs, dt);
    moveSystem(ecs, dt);
    collisionSystem(ecs);
    animationSystem(ecs, dt);

    // Render
    beginScene(engine->mainCamera, RenderMode::NORMAL);
        renderSprites(ecs);
    endScene();

    // UI
    beginUiFrame({0, 0}, {engine->windowWidth, engine->windowHeight});
        Font* font = getFont("Roboto-Regular");
        setFontUI(font);
        UiText("Score: 100", {10, 10}, 0.3f);
    endUiFrame();
}

GAME_API void gameStop(Arena* gameArena) {
    // Cleanup if needed
}
```

---

## FAQ

### Q: Can I use std::vector in my game code?

**A:** Yes, but it will break hot reload. Use arenas if you need hot reload, or accept that you'll need to restart to reload code.

### Q: How do I reference entities across frames?

**A:** Store the Entity ID (handle), not a pointer. Entities persist in the ECS across frames and reloads.

### Q: Why arena allocators instead of malloc?

**A:** Arenas are faster (O(1)), prevent fragmentation, are cache-friendly, and enable hot reload.

### Q: Can I have multiple GameStates?

**A:** GameState should be singular. For multiple states, use a state machine or separate game modes.

### Q: How do I save/load game state?

**A:** Use the serialization system (see `serialization.hpp`), or implement custom save/load that writes GameState to disk.

### Q: Can I use this engine for 3D?

**A:** Not currently. It's designed for 2D games. You'd need to add a 3D renderer and transform system.

---

## Conclusion

This engine provides:

- ✅ Hot reload for rapid iteration
- ✅ ECS for flexible game object management
- ✅ Arena allocation for predictable performance
- ✅ Batch rendering for efficiency
- ✅ Spatial collision system
- ✅ Cross-platform foundation

**Start building your game and enjoy instant code reloading!**

For more examples, see the `src/game/` directory.
