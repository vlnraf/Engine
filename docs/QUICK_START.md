# Quick Start Guide

Get your game running in 5 minutes!

---

## Project Setup

### 1. Clone and Build

```bash
cd D:\Engine
make
```

This produces:
- `engine.exe` - The engine runtime
- `game.dll` - Your game code (hot-reloadable)

### 2. Run the Engine

```bash
.\engine.exe
```

The engine will:
- Load `game.dll`
- Call `gameStart()`
- Run the game loop

---

## Your First Game

### Minimal Game (projectx.cpp)

```cpp
#include "projectx.hpp"

// Register components
ECS_DECLARE_COMPONENT(PlayerTag);

GAME_API void gameStart(Arena* gameArena) {
    // MUST be first allocation!
    GameState* state = arenaPushStructZero(gameArena, GameState);
    state->currentLevel = PLAYING;
    state->paused = false;

    // Register components
    registerComponent(engine->ecs, PlayerTag);

    // Load assets
    loadTexture("player");
    loadFont("Arial");
}

GAME_API void gameUpdate(Arena* gameArena, EngineState* engine, float dt) {
    GameState* state = (GameState*)gameArena->base;

    clearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Render
    beginScene(engine->mainCamera, RenderMode::NORMAL);
        renderDrawFilledRect({100, 100, 0}, {50, 50}, {0,0,0}, {1,0,0,1});
    endScene();

    // UI
    beginUiFrame({0,0}, {engine->windowWidth, engine->windowHeight});
        Font* font = getFont("Arial");
        setFontUI(font);
        UiText("Hello World!", {10, 10}, 0.5f);
    endUiFrame();
}

GAME_API void gameStop(Arena* gameArena) {
    // Cleanup
}
```

### Build and Run

```bash
make          # Builds game.dll
.\engine.exe  # Runs engine

# Edit projectx.cpp, save
make          # Rebuilds game.dll
# Engine auto-reloads! No restart needed!
```

---

## Creating Your Player

```cpp
// player.hpp
#pragma once
#include "core.hpp"

struct PlayerTag {};
struct PlayerInputComponent {
    float moveSpeed;
};

Entity createPlayer(Ecs* ecs, glm::vec3 position);
void updatePlayerInput(Ecs* ecs, float dt);
```

```cpp
// player.cpp
#include "player.hpp"

ECS_DECLARE_COMPONENT(PlayerTag);
ECS_DECLARE_COMPONENT(PlayerInputComponent);

Entity createPlayer(Ecs* ecs, glm::vec3 position) {
    Entity player = createEntity(ecs);

    // Transform
    TransformComponent transform = {
        .position = position,
        .scale = {2, 2, 0},
        .rotation = {0, 0, 0}
    };
    pushComponent(ecs, player, TransformComponent, &transform);

    // Sprite
    SpriteComponent sprite = {
        .texture = getTexture("player"),
        .index = {0, 0},
        .size = {32, 32},
        .tileSize = {16, 16},
        .visible = true
    };
    pushComponent(ecs, player, SpriteComponent, &sprite);

    // Velocity
    VelocityComponent velocity = {.vel = {200, 200}};
    pushComponent(ecs, player, VelocityComponent, &velocity);

    // Direction
    DirectionComponent direction = {.dir = {0, 0}};
    pushComponent(ecs, player, DirectionComponent, &direction);

    // Player tag
    PlayerTag tag = {};
    pushComponent(ecs, player, PlayerTag, &tag);

    // Input
    PlayerInputComponent input = {.moveSpeed = 200.0f};
    pushComponent(ecs, player, PlayerInputComponent, &input);

    return player;
}

void updatePlayerInput(Ecs* ecs, float dt) {
    EntityArray players = view(ecs,
        ECS_TYPE(PlayerTag),
        ECS_TYPE(DirectionComponent),
        ECS_TYPE(PlayerInputComponent)
    );

    for (size_t i = 0; i < players.count; i++) {
        Entity player = players.entities[i];
        DirectionComponent* dir = getComponent(ecs, player, DirectionComponent);

        // Reset direction
        dir->dir = {0, 0};

        // Keyboard input
        if (isPressed(KEYS::W)) dir->dir.y = 1;
        if (isPressed(KEYS::S)) dir->dir.y = -1;
        if (isPressed(KEYS::A)) dir->dir.x = -1;
        if (isPressed(KEYS::D)) dir->dir.x = 1;

        // Normalize diagonal movement
        if (dir->dir.x != 0 && dir->dir.y != 0) {
            float length = sqrt(dir->dir.x * dir->dir.x + dir->dir.y * dir->dir.y);
            dir->dir.x /= length;
            dir->dir.y /= length;
        }
    }
}
```

### Use in Game

```cpp
// In gameStart
Entity player = createPlayer(engine->ecs, {100, 100, 0});

// In gameUpdate
updatePlayerInput(engine->ecs, dt);

// Movement system (automatic if you have Transform + Velocity + Direction)
```

---

## Adding Enemies

```cpp
Entity createEnemy(Ecs* ecs, glm::vec3 position) {
    Entity enemy = createEntity(ecs);

    TransformComponent transform = {.position = position, .scale = {1,1,0}};
    pushComponent(ecs, enemy, TransformComponent, &transform);

    SpriteComponent sprite = {
        .texture = getTexture("enemy"),
        .index = {0, 0},
        .size = {32, 32}
    };
    pushComponent(ecs, enemy, SpriteComponent, &sprite);

    HealthComponent health = {.hp = 50, .maxHp = 50};
    pushComponent(ecs, enemy, HealthComponent, &health);

    EnemyTag tag = {};
    pushComponent(ecs, enemy, EnemyTag, &tag);

    return enemy;
}

// Spawn enemies
for (int i = 0; i < 10; i++) {
    float x = rand() % 800;
    float y = rand() % 600;
    createEnemy(ecs, {x, y, 0});
}
```

---

## Adding Collision

```cpp
// Add to player
Box2DCollider playerCollider = {
    .type = Box2DCollider::DYNAMIC,
    .offset = {0, 0},
    .size = {32, 32},
    .isTrigger = false
};
pushComponent(ecs, player, Box2DCollider, &playerCollider);

// Add to enemy
Box2DCollider enemyCollider = {
    .type = Box2DCollider::DYNAMIC,
    .offset = {0, 0},
    .size = {32, 32},
    .isTrigger = false
};
pushComponent(ecs, enemy, Box2DCollider, &enemyCollider);

// Handle collisions
void handleCollisions(Ecs* ecs) {
    CollisionEventArray* events = getCollisionEvents();

    for (size_t i = 0; i < events->count; i++) {
        Entity a = events->item[i].entityA.entity;
        Entity b = events->item[i].entityB.entity;

        if (hasComponent(ecs, a, PlayerTag) && hasComponent(ecs, b, EnemyTag)) {
            HealthComponent* playerHealth = getComponent(ecs, a, HealthComponent);
            playerHealth->hp -= 10;
            LOGINFO("Player hit! HP: %.0f", playerHealth->hp);
        }
    }
}

// In gameUpdate
handleCollisions(engine->ecs);
```

---

## Common Patterns

### Game State Machine

```cpp
enum GameState {
    MAIN_MENU,
    PLAYING,
    PAUSED,
    GAME_OVER
};

// In gameUpdate
switch (state->currentLevel) {
    case MAIN_MENU:
        drawMainMenu();
        if (isJustPressed(KEYS::ENTER)) {
            state->currentLevel = PLAYING;
            loadLevel();
        }
        break;

    case PLAYING:
        updatePlayerInput(ecs, dt);
        updateEnemies(ecs, dt);

        if (isJustPressed(KEYS::ESCAPE)) {
            state->currentLevel = PAUSED;
        }
        break;

    case PAUSED:
        drawPauseMenu();
        if (isJustPressed(KEYS::ESCAPE)) {
            state->currentLevel = PLAYING;
        }
        break;
}
```

### Spawning System

```cpp
void spawnEnemies(Ecs* ecs, float dt) {
    static float spawnTimer = 0;
    spawnTimer += dt;

    if (spawnTimer >= 2.0f) {  // Spawn every 2 seconds
        float x = rand() % 800;
        float y = rand() % 600;
        createEnemy(ecs, {x, y, 0});

        spawnTimer = 0;
    }
}
```

### Camera Follow

```cpp
void cameraFollowPlayer(Ecs* ecs, OrtographicCamera* camera) {
    EntityArray players = view(ecs, ECS_TYPE(PlayerTag), ECS_TYPE(TransformComponent));

    if (players.count > 0) {
        TransformComponent* transform = getComponent(ecs, players.entities[0], TransformComponent);
        followTarget(camera, transform->position);
    }
}

// In gameUpdate
cameraFollowPlayer(engine->ecs, &engine->mainCamera);
```

---

## Hot Reload Tips

### ✓ Do This

```cpp
// Store entity handles
struct GameState {
    Entity player;
    Entity boss;
};

// Use fixed-size arrays
struct GameState {
    int highScores[10];
    Card powerups[6];
};

// Allocate from arena
Enemy* enemyPool = arenaPushArray(gameArena, Enemy, 100);
```

### ✗ Don't Do This

```cpp
// Don't store raw pointers
struct GameState {
    Enemy* enemyList;  // ❌ Breaks on reload
};

// Don't use heap containers
struct GameState {
    std::vector<Enemy> enemies;  // ❌ Breaks on reload
};

// Don't allocate with new/malloc for persistent data
Enemy* enemy = new Enemy();  // ❌ Breaks on reload
```

---

## Debugging

### Enable Debug Mode

Press **F5** during gameplay to see:
- Collision boxes
- Spatial grid
- Entity counts

### Add Logging

```cpp
#include "tracelog.hpp"

LOGINFO("Player created at (%.0f, %.0f)", x, y);
LOGWARN("Low health: %.0f", health);
LOGERROR("Failed to load: %s", filename);
```

### Check Entity Counts

```cpp
LOGINFO("Total entities: %zu", engine->ecs->entitiesCount);
```

---

## Next Steps

1. Read the full [ENGINE_DOCUMENTATION.md](ENGINE_DOCUMENTATION.md)
2. Study the example game in `src/game/`
3. Experiment with hot reload - edit code while running!
4. Join our community (if applicable)

**Happy game developing! 🎮**
