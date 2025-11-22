# Engine TODO - Missing Features & Improvements

## Missing Core Features

### 1. Resource/Asset Manager
**Current Problem:**
- Resource loading is scattered and ad-hoc
- Individual getTexture(), getFont(), loadAudio() calls everywhere
- No centralized lifecycle management
- No way to unload/reload resources
- No reference counting or automatic cleanup
- All loading happens manually in gameStart()
- No asset manifest or data-driven resource loading

**Action:** Create a unified ResourceManager that handles loading, caching, and unloading of all asset types with a manifest file.

### 2. Particle System
**Current Problem:**
- Have projectiles and explosions but no general particle system
- Missing visual effects: trails, sparks, blood, dust, etc.
- Would significantly enhance visual feedback

**Action:** Implement a particle system for visual effects.

### 3. Entity Prefab/Template System
**Current Problem:**
- All entity creation is manual C++ code (createPlayer, createGun, etc.)
- No way to define entity templates in data
- Hard to iterate on entity designs
- No way for non-programmers to create content
- Code duplication for similar entities

**Action:** Build on serialization system to support entity prefabs that can be instantiated at runtime.

### 4. Proper Scene Management
**Current Problem:**
- scene.hpp/cpp exists but not integrated
- Level loading clears ECS and rebuilds everything manually
- No scene serialization/deserialization
- Can't save/load game state easily
- Level transitions are hardcoded

**Action:** Integrate scene system with serialization for proper scene management.

### 5. System Manager/Scheduler
**Current Problem:**
- gameUpdate() manually calls 15+ systems in specific order
- Error-prone and inflexible
- Hard to see dependencies between systems

**Current Code:**
```cpp
systemProjectileHit(engine->ecs);
cooldownSystem(engine->ecs, dt);
weaponFireSystem(engine->ecs, dt);
// ... 12 more systems
```

**Action:** Create a SystemManager that registers systems with dependencies/ordering and calls them automatically.

## Consolidation Opportunities

### 1. Component Registration Boilerplate
**Current Problem:**
- Manually register 25+ components in gameStart()

**Current Code:**
```cpp
registerComponent(engine->ecs, PlayerTag);
registerComponent(engine->ecs, ProjectileTag);
// ... 23 more lines
```

**Action:** Use a macro-based auto-registration system or component manifest.

### 2. Collision System Needs Expansion
**Current Limitations:**
- Basic AABB-only collision
- No circle colliders
- No polygon/tilemap collision
- Physics is minimal (no gravity, forces, velocity integration)
- No collision layers/masks for filtering

**Action:** Expand collision system with more collider types and proper physics.

### 3. Input System Split
**Current Problem:**
- Input handling fragmented between platform layer (GLFW callbacks) and game code

**Action Items:**
- Add action mapping system (map keys to actions like "jump", "fire")
- Add input buffering for fighting-game-style inputs
- Support rebindable controls

### 4. Render Pipeline Lacks Structure
**Current Limitations:**
- No formal render layers/sorting beyond float values
- No render passes (shadow pass, glow pass, etc.)
- No post-processing effects
- Hard to add new visual effects

**Action:** Create structured render pipeline with passes and post-processing support.

### 5. Debug Tools Missing
**Current State:**
- Basic profiler exists
- Basic collision debug rendering

**Missing:**
- In-engine console for commands
- Entity inspector (view/edit components at runtime)
- Performance graphs
- Memory visualization
- ECS query debugger

**Action:** Build comprehensive debug/dev tools.

## Lower Priority Features

### 6. Audio Enhancement
**Missing:**
- Spatial/3D audio positioning
- Audio mixing/groups (SFX, Music, Voice)
- Audio ducking/crossfading
- Volume control beyond fixed values

### 7. AI/Pathfinding
**Current State:**
- Enemies just move toward player

**Missing:**
- A* pathfinding around obstacles
- Navigation mesh
- Behavior trees or state machines for AI

### 8. Save/Load System
**Current State:**
- Serialization infrastructure exists
- No actual game save/load functionality

**Action:** Implement game state save/load using existing serialization.

## Priority: Quick Wins (Most Impact for Effort)

1. **Resource Manager** - Eliminates scattered loading code, enables hot-reloading assets
2. **Particle System** - Huge visual impact for relatively simple implementation
3. **System Manager** - Cleans up game loop, makes system dependencies explicit
4. **Entity Prefabs** - Enables data-driven content creation
5. **Better Physics** - Vampire survivors clone would benefit from proper collision layers, circle colliders

## Notes

- Focus on items 1-5 in Quick Wins for maximum impact
- Resource Manager and Particle System are good starting points
- Many features can build on existing systems (serialization, ECS, arena allocators)
