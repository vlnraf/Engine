# EXIS

Exis is a simple game engine I made to have fun and learn new things during my spare time. The idea is to have an engine that doesn't hand you everything on a silver plate — no drag-and-drop scene editors, no visual scripting. Just code. I like programming, so I wanted the engine to reflect that.

It's built on top of OpenGL, with a custom Entity Component System, arena-based memory management, and a hot-reload system that lets you change game logic without restarting the application.

## Features

- Modular architecture — the core library can be used independently
- Very limited set of dependencies
- Custom Entity Component System (sparse set, built from scratch)
- Hardware accelerated with OpenGL
- Arena allocators — no malloc/free in game code
- Spatial hashing collision system with hitbox/hurtbox support
- Resource loading (textures, fonts, tilemaps)
- Integrated with [Tiled](https://www.mapeditor.org/) for map generation
- Hot-reloading of game logic at runtime

## Architecture

The engine is split into three layers that get compiled separately:

- **application.exe** — the platform layer, owns the main loop, GLFW window, and hot-reload logic
- **core.dll** — the engine itself: ECS, renderer, audio, input, collision
- **game.dll** — your game logic, hot-reloadable while the application is running

The game layer just needs to implement four entry points:

```cpp
GAME_API void gameStart(Arena* gameArena, EngineState* engine);
GAME_API void gameUpdate(Arena* gameArena, EngineState* engine, float dt);
GAME_API void gameRender(Arena* gameArena, EngineState* engine, float dt);
GAME_API void gameStop(Arena* gameArena, EngineState* engine);
```

That's the entire contract between your game and the engine. Everything else is up to you.

## Memory — Arena Allocators

There is no `malloc` or `free` in game code. All memory goes through arena allocators.

An arena is just a big block of memory with a cursor. You ask for memory, the cursor moves forward, done. There's no per-allocation overhead, no fragmentation, and freeing is just resetting the cursor.

```cpp
// allocate a struct
MyComponent* c = arenaAllocStruct(arena, MyComponent);

// allocate an array
Entity* entities = arenaAllocArray(arena, Entity, 1000);
```

The engine manages a few arenas with different lifetimes:

- `engine->arena` — lives for the entire session
- `engine->gameArena` — your game's persistent storage, survives hot-reloads
- `ecs->frameArena` — cleared every frame, useful for temporary scratch data

If you need something temporary within a single scope, you can use a `TempArena` — it saves the current cursor position and restores it when released, so allocations made inside it are automatically "freed".

## Entity Component System

The ECS is built around sparse sets, which give O(1) component access and keep component data packed in contiguous memory for cache-friendly iteration.

You register component types at startup and then attach them to entities however you want:

```cpp
// register
registerComponent(ecs, TransformComponent);
registerComponent(ecs, VelocityComponent);

// create an entity and attach components
Entity e = createEntity(ecs);
TransformComponent t = { .position = {0, 0, 0} };
pushComponent(ecs, e, TransformComponent, &t);

// query all entities that have both components
EntityArray entities = view(ecs, ECS_TYPE(TransformComponent), ECS_TYPE(VelocityComponent));
for (size_t i = 0; i < entities.count; i++) {
    TransformComponent* t = getComponent(ecs, entities.entities[i], TransformComponent);
    VelocityComponent* v = getComponent(ecs, entities.entities[i], VelocityComponent);
    t->position += v->velocity * dt;
}
```

The `view()` macro returns only entities that have all the requested components, so you never touch entities that don't match your system.

## Collision

The collision system uses a uniform spatial grid to avoid checking every pair of entities against each other. The world is divided into fixed-size cells, entities are bucketed by position each frame, and only entities sharing a cell are tested.

Each entity can have multiple colliders — hitbox, hurtbox, and a regular collider — all as separate `Box2DCollider` components. Collision events are deduplicated through a hash set so you don't process the same pair twice in a frame.

It's not the most sophisticated broad-phase algorithm, but it handles tens of thousands of entities at 60fps without breaking a sweat.

```
A simple test with almost 30k entities with colliders, hitboxes, hurtboxes at +60 FPS.
```

https://youtu.be/nc1_4mu7ejY

## Build

```bash
make          # builds core.dll, game.dll, and application.exe
make game.dll # builds only the game library (for hot-reload)
make clean    # remove all build artifacts
```

Requires: `clang++`, GLFW3, FMOD, FreeType. All third-party libraries are under `external/`.

Supported platforms: Windows.
