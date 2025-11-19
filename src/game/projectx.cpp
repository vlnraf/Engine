//#include "projectx.hpp"
#include "components.hpp"

#include "player.hpp"
#include "projectile.hpp"
#include "lifetime.hpp"
#include "weapon.hpp"
#include "mainmenu.hpp"

#include "vampireclone.hpp"

ECS_DECLARE_COMPONENT(WallTag);
ECS_DECLARE_COMPONENT(PortalTag);
ECS_DECLARE_COMPONENT(PortalTag2);
ECS_DECLARE_COMPONENT(PlayerTag);
ECS_DECLARE_COMPONENT(InputComponent);
ECS_DECLARE_COMPONENT(WeaponTag);
ECS_DECLARE_COMPONENT(EnemyTag);
ECS_DECLARE_COMPONENT(PickupTag);
ECS_DECLARE_COMPONENT(HitboxTag);
ECS_DECLARE_COMPONENT(HurtboxTag);
ECS_DECLARE_COMPONENT(HealthComponent);
ECS_DECLARE_COMPONENT(DamageComponent);
ECS_DECLARE_COMPONENT(ExperienceDrop);


GameState* gameState;
EngineState* engine;

//NOTE: forward declaration
void loadLevel(GameLevels level);

void systemRenderSprites(Ecs* ecs){
    EntityArray entities = view(ecs, ECS_TYPE(TransformComponent), ECS_TYPE(SpriteComponent));

    for(size_t i = 0; i < entities.count; i++){
        Entity entity = entities.entities[i];
        TransformComponent* t= (TransformComponent*) getComponent(ecs, entity, TransformComponent);
        SpriteComponent* s= (SpriteComponent*) getComponent(ecs, entity, SpriteComponent);
        if(s->visible){
            renderDrawSprite(t->position, t->scale, t->rotation, s);
        }
    }
}

void moveSystem(Ecs* ecs, float dt){
    EntityArray entities = view(ecs, ECS_TYPE(TransformComponent), ECS_TYPE(VelocityComponent), ECS_TYPE(DirectionComponent));
    for(size_t i = 0; i < entities.count; i++){
        Entity e = entities.entities[i];
        TransformComponent* transform = (TransformComponent*) getComponent(ecs, e, TransformComponent);
        VelocityComponent* velocity  = (VelocityComponent*)  getComponent(ecs, e, VelocityComponent);
        DirectionComponent* direction  = (DirectionComponent*)  getComponent(ecs, e, DirectionComponent);
        transform->position += glm::vec3(direction->dir.x * velocity->vel.x * dt, direction->dir.y * velocity->vel.y * dt, 0.0f);
    }
}

void deathSystem(Ecs* ecs){
    EntityArray entities = view(ecs, ECS_TYPE(HealthComponent));
    for(size_t i = 0; i < entities.count; i++){
        Entity e = entities.entities[i];
        HealthComponent* health = getComponent(ecs, e, HealthComponent);
        if(hasComponent(ecs, e, PlayerTag)){
            //if(health->hp <= 0){
            //    gameState->gameLevels = GameLevels::GAME_OVER;
            //    break;
            //}
        }

        if(hasComponent(ecs, e, EnemyTag)){
            if(health->hp <= 0){
                TransformComponent* transform = getComponent(ecs, e, TransformComponent);
                spawnExperience(ecs, transform->position);
                if(hasComponent(ecs, e, Child)){
                    Child* childs = getComponent(ecs, e, Child);
                    for(size_t j = 0; j < childs->count; j++){
                        removeEntity(ecs, childs->entity[j]);
                    }
                }
                removeEntity(ecs, e);
            }
        }
    }
}

void cameraFollowSystem(Ecs* ecs, OrtographicCamera* camera){
    EntityArray entities = view(ecs, ECS_TYPE(PlayerTag));
    for(size_t i = 0; i < entities.count; i++){
        Entity entity = entities.entities[i];
        TransformComponent* t = (TransformComponent*)getComponent(ecs, entity, TransformComponent);
        if(!t){ break; }
        followTarget(camera, t->position);
    }
}

void animationSystem(Ecs* ecs, float dt){
    PROFILER_START();
    EntityArray entities = view(ecs, ECS_TYPE(SpriteComponent), ECS_TYPE(AnimationComponent));

    //NOTE: It should not be a system or it runs every frame and so even if the animation
    // is not showing it's been computed
    for(size_t i = 0; i < entities.count; i++){
        Entity entity = entities.entities[i];
        SpriteComponent* s= (SpriteComponent*)getComponent(ecs, entity, SpriteComponent);
        AnimationComponent* animComp = (AnimationComponent*)getComponent(ecs, entity, AnimationComponent);
        Animation* anim = getAnimation(animComp->animationId);

        //if(component->id != component->previousId){ //NOTE: synchronize animation to frame 0 when it changes
        //    component->currentFrame = 0;
        //    component->previousId = component->id;
        //}

        if(anim->loop){
            if(animComp->elapsedTime > anim->frameDuration){
                animComp->currentFrame = (animComp->currentFrame + 1) % (anim->frames); // module to loop around
                animComp->elapsedTime = 0;
            }
        }else{
            //if(animComp->elapsedTime > anim->frameDuration){
            //    animComp->currentFrame = animComp->currentFrame + 1; 
            //    animComp->elapsedTime = 0;
            //    //component->frameCount = 0;
            //}
        }
        animComp->elapsedTime += dt;
        s->index = anim->indices[animComp->currentFrame];
    }
    PROFILER_END();
}

void loadLevel(GameLevels level){
    PROFILER_START();
    for(size_t entity = 1; entity < engine->ecs->entities; entity++){
        if(hasComponent(engine->ecs, entity, PersistentTag)){
            continue;
        }
        removeEntity(engine->ecs, entity);
    }
    switch(level){
        case GameLevels::MAIN_MENU:{
            break;
        }
        case GameLevels::FIRST_LEVEL:{
            gameState->bgMap = LoadTilesetFromTiled("test", engine->ecs);
            Entity player = createPlayer(engine->ecs, engine->mainCamera);
            {
                //Vampire survival clone teleport
                //TODO: remove from final game
                TransformComponent transform = {    
                    .position = {500.0f, 50.0f, 0.0f},
                    .scale = {1.0f, 1.0f, 0.0f},
                    .rotation = {0.0f, 0.0f, 0.0f}
                };
                SpriteComponent sprite = {
                    .texture = getTexture("dungeon"),
                    .index = {15,8},
                    .size = {32,32},
                    .tileSize = {16, 16},
                    .ySort = true,
                    .layer = 1.0f
                };
                Box2DCollider coll = {.type = Box2DCollider::STATIC, .offset = {0,0}, .size = {32, 32}, .isTrigger = true};
                PortalTag2 p = {};
                Entity portal = createEntity(engine->ecs);
                pushComponent(engine->ecs, portal, TransformComponent, &transform);
                pushComponent(engine->ecs, portal, Box2DCollider, &coll);
                pushComponent(engine->ecs, portal, SpriteComponent, &sprite);
                pushComponent(engine->ecs, portal, PortalTag2, &p);

            }

            HasWeaponComponent* hasWeapon = (HasWeaponComponent*)getComponent(engine->ecs, player, HasWeaponComponent);
            removeEntity(engine->ecs, hasWeapon->weaponId[0]);
            Entity gun = createGun(engine->ecs);
            hasWeapon->weaponId[0] = gun;
            hasWeapon->weaponType[0] = WEAPON_GUN;
            hasWeapon->weaponCount = 1;
            PersistentTag persistent = {};
            pushComponent(engine->ecs, gun, PersistentTag, &persistent);
            break;
        }
        case GameLevels::SECOND_LEVEL:{
            break;
        }
        case GameLevels::THIRD_LEVEL:{
            //createPlayer(engine->ecs, engine->mainCamera);
            gameState->gameLevels = GameLevels::THIRD_LEVEL;
            break;
        }
        default:
            break;
    }
    PROFILER_END();
}

void applyDmgUp(float dmgMultiplier){
    EntityArray entities = view(engine->ecs, ECS_TYPE(PlayerTag));

    for(size_t i = 0; i < entities.count; i++){
        Entity e = entities.entities[i];
        HasWeaponComponent* hasWeapon = (HasWeaponComponent*)getComponent(engine->ecs, e, HasWeaponComponent);
        if(hasWeapon->weaponType[0] == WEAPON_GUN){
            DamageComponent* damage = getComponent(engine->ecs, hasWeapon->weaponId[0], DamageComponent);
            damage->dmg = damage->dmg + (1.0f * dmgMultiplier);
            hasWeapon->weaponType[0] = WEAPON_GUN;
        }
    }
}

void applySpeedUp(float speedUp){
    EntityArray player = view(engine->ecs, ECS_TYPE(PlayerTag));
    VelocityComponent* vel = (VelocityComponent*)getComponent(engine->ecs, player.entities[0], VelocityComponent);
    vel->vel += (vel->vel * speedUp);
}

void applyIncreaseRadius(float radius){
    EntityArray weapons = view(engine->ecs, ECS_TYPE(HasWeaponComponent));
    HasWeaponComponent* hasWeapon = (HasWeaponComponent*)getComponent(engine->ecs, weapons.entities[0], HasWeaponComponent);
    if(hasComponent(engine->ecs, hasWeapon->weaponId[0], GunComponent)){
        GunComponent* gun = (GunComponent*)getComponent(engine->ecs, hasWeapon->weaponId[0], GunComponent);
        gun->radius = gun->radius + (gun->radius * radius);
    }
}


void systemOrbitMovement(Ecs* ecs, float dt){
    EntityArray projectiles = view(ecs, ECS_TYPE(TransformComponent), ECS_TYPE(OrbitingProjectile));
    EntityArray weapons = view(ecs, ECS_TYPE(OrbitingWeaponComponent));
    for(Entity i = 0; i < weapons.count; i++){
        OrbitingWeaponComponent* orbit = (OrbitingWeaponComponent*)getComponent(ecs, weapons.entities[i], OrbitingWeaponComponent);
        if(!orbit){continue;}
        orbit->angle += 3 * dt;
        for(size_t j = 0; j < projectiles.count; j++){
            Entity projectile = projectiles.entities[j];
            OrbitingProjectile* orbitProjectile = (OrbitingProjectile*)getComponent(ecs, projectile, OrbitingProjectile);
            TransformComponent* transform = (TransformComponent*)getComponent(ecs, projectile, TransformComponent);
            float slotAngle = (2.0f * 3.14 / projectiles.count) * orbitProjectile->slotIndex;
            float finalAngle = orbit->angle + slotAngle;
            glm::vec3 offset = {cos(finalAngle) * 25,
                                sin(finalAngle) * 25,
                                0};
            glm::vec3 center = getComponent(ecs, orbit->target, TransformComponent)->position;
            transform->position = glm::vec3(center + offset);
        }
    }
}

void applyCard(Card* choice){
    if(!choice) return;

    switch(choice->cardChoice){
        case CardChoice::CARD_DMG_UP:{
            float dmgMultiplier = gameState->cards[0].dmg;
            applyDmgUp(dmgMultiplier);
            break;
        }
        case CardChoice::CARD_SPEED_UP:{
            float speedMultiplier = gameState->cards[1].speed;
            applySpeedUp(speedMultiplier);
            break;
        }
        case CardChoice::CARD_SIZE_UP:{
            float radius = gameState->cards[2].radius;
            applyIncreaseRadius(radius);
            break;
        }
        case CardChoice::CARD_ORBIT:{

            bool alreadyHave = false;
            Entity weaponId;
            EntityArray players = view(engine->ecs, ECS_TYPE(PlayerTag));
            HasWeaponComponent* hasWeapon = (HasWeaponComponent*)getComponent(engine->ecs, players.entities[0], HasWeaponComponent);
            for(size_t i = 0; i < hasWeapon->weaponCount; i++){
                if(hasWeapon->weaponType[i] == WeaponType::WEAPON_ORBIT){
                    weaponId = hasWeapon->weaponId[i];
                    alreadyHave = true;
                }
            }
            if(!alreadyHave){
                weaponId = createOrbitWeapon(engine->ecs);
            }
            addOrbitProjectile(engine->ecs, weaponId);
            break;
        }
        case CardChoice::CARD_GRANADE:{
            EntityArray players = view(engine->ecs, ECS_TYPE(PlayerTag));

            HasWeaponComponent* hasWeapon = (HasWeaponComponent*)getComponent(engine->ecs, players.entities[0], HasWeaponComponent);
            Entity granade = createGranade(engine->ecs);
            hasWeapon->weaponId[hasWeapon->weaponCount] = granade;
            hasWeapon->weaponType[hasWeapon->weaponCount] = WEAPON_GRANADE;
            hasWeapon->weaponCount++;

            choice->pickable = false;
            LOGINFO("Granade added");
            break;
        }
        case CardChoice::CARD_ADD_PROJECTILE:{
            EntityArray players = view(engine->ecs, ECS_TYPE(PlayerTag));
            HasWeaponComponent* hasWeapon = (HasWeaponComponent*)getComponent(engine->ecs, players.entities[0], HasWeaponComponent);
            for(size_t i = 0; i < hasWeapon->weaponCount; i++){
                if(hasWeapon->weaponType[i] == WeaponType::WEAPON_GUN){
                    GunComponent* gun = getComponent(engine->ecs, hasWeapon->weaponId[i], GunComponent);
                    gun->numProjectiles++;
                    break;
                }
            }
            break;
        }
        case CardChoice::CARD_NONE:{
            break;
        }
    }
}

void drawCardSelectionMenu(){
    //horizontal layout
    int xCenter = engine->mainCamera.width * 0.5f;
    int yCenter = engine->mainCamera.height * 0.5f;
    int padding = 10;
    int layoutWidth = 0;

    int buttonWidth = 100;
    int buttonHeight = 100;

    Card* choice = NULL;// = {.cardChoice = CardChoice::CARD_NONE};

    //NOTE i am modifying the card catalogue so it won't reinitialize when restart a run
    if(!gameState->cardInit){
        for(int i = 0; i < 3; i++){
            int r = i + rand() % (CARDS_NUMBER - i);
            if(!gameState->cards[r].pickable){
                i--;
                continue;
            }
            Card tmp = gameState->cards[i];
            gameState->cards[i] = gameState->cards[r];
            gameState->cards[r] = tmp;
        }
        gameState->cardInit = true;
    }

    // calculate 3 buttons
    layoutWidth += (3 * (buttonWidth + padding));
    int buttonX = xCenter - (layoutWidth / 2);
    if(UiButton(gameState->cards[0].description, {buttonX, yCenter - (buttonHeight * 0.5f)},{buttonWidth, buttonHeight}, {0,0})){
        choice = &gameState->cards[0];
        gameState->cardInit = false;
        gameState->gameLevels = GameLevels::THIRD_LEVEL;
    }
    buttonX += (layoutWidth / 3);
    if(UiButton(gameState->cards[1].description, {buttonX, yCenter - (buttonHeight * 0.5f)},{buttonWidth, buttonHeight}, {0,0})){
        choice = &gameState->cards[1];
        gameState->cardInit = false;
        gameState->gameLevels = GameLevels::THIRD_LEVEL;
    }
    buttonX += (layoutWidth / 3);
    if(UiButton(gameState->cards[2].description, {buttonX, yCenter - (buttonHeight * 0.5f)},{buttonWidth, buttonHeight}, {0,0})){
        choice = &gameState->cards[2];
        gameState->cardInit = false;
        gameState->gameLevels = GameLevels::THIRD_LEVEL;
    }

    applyCard(choice);
}

static float secondsPassed = 1;
static float minutesPassed = 0;
void drawHud(OrtographicCamera* camera, float dt){
    Font* font = getFont("Roboto-Regular");
    setFontUI(font);
    EntityArray player = view(engine->ecs, ECS_TYPE(PlayerTag));
    HealthComponent* h = getComponent(engine->ecs, player.entities[0], HealthComponent);
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.0f / %d HP", h->hp, 100);
    UiText(buffer, {30, 20}, 0.2f);

    float hpBarWidth = 100;
    float hpBarHeight =  5;
    float hpBar = h->hp * hpBarWidth / 100; //health conversion to rect 98 is the hp size 000 is the black bar size
    renderDrawFilledRect(convertScreenCoords({30, 50}, {hpBarWidth + 10, hpBarHeight + 5}, {engine->mainCamera.width, engine->mainCamera.height}), {hpBarWidth + 10, hpBarHeight + 5}, {0,0}, {0,0,0,1});
    renderDrawFilledRect(convertScreenCoords({35, 52.5}, {hpBarWidth, hpBarHeight}, {engine->mainCamera.width, engine->mainCamera.height}), {hpBar, hpBarHeight}, {0,0}, {1,0,0,1});

    ExperienceComponent* exp = (ExperienceComponent*)getComponent(engine->ecs, player.entities[0], ExperienceComponent);
    float expBarWidth = camera->width - 40;
    float expBarHeight =  5;
    float expBar = exp->currentXp * 550/ exp->maxXp; 
    renderDrawFilledRect(convertScreenCoords({10, camera->height - 20}, {expBarWidth + 20, expBarHeight + 5}, {engine->mainCamera.width, engine->mainCamera.height}), {expBarWidth + 20, expBarHeight + 5}, {0,0}, {0,0,0,1});
    renderDrawFilledRect(convertScreenCoords({20, camera->height - 20 + 2.5}, {expBarWidth, expBarHeight}, {engine->mainCamera.width, engine->mainCamera.height}), {expBar, expBarHeight}, {0,0}, {0,1,1,1});
    char fpsText[30];
    static float timer = 0;
    static float ffps = 0;
    timer += dt;
    if(timer >= 1.0f){ //each second
        ffps = engine->fps;
        timer = 0;
    }
    snprintf(fpsText, sizeof(fpsText), "%.2f FPS", ffps);
    UiText(fpsText, {engine->mainCamera.width - calculateTextWidth(getFontUI(), fpsText, 0.2f) - 10, 20}, 0.2f);
    char entitiesNumber[30];
    snprintf(entitiesNumber, sizeof(entitiesNumber), "%zu", engine->ecs->entitiesCount);
    UiText(entitiesNumber, {engine->mainCamera.width - calculateTextWidth(getFontUI(), entitiesNumber, 0.2f) - 100, 20}, 0.2f);

    if(gameState->gameLevels == GameLevels::THIRD_LEVEL){
        secondsPassed += dt;
        if(secondsPassed > 60){
            secondsPassed = 0;
        }
        if(secondsPassed == 0){
            minutesPassed ++;
        }
        char timePassedText[50];
        snprintf(timePassedText, sizeof(timePassedText), "Survived Time %.0f:%.0f", minutesPassed, secondsPassed);
        UiText(timePassedText, {(engine->mainCamera.width / 2) - (calculateTextWidth(getFontUI(), timePassedText, 0.3f) / 2), 20}, 0.3f);
    }
}

void nextLevelSystem(Ecs* ecs){
    TriggerEventArray* events = getTriggerEvents();
    for(size_t i = 0; i < events->count; i++){
        CollisionEvent event = events->item[i];
        Entity entityA = event.entityA.entity;
        Entity entityB = event.entityB.entity;

        if(hasComponent(ecs, entityA, PlayerTag) && hasComponent(ecs, entityB, PortalTag2)){
            gameState->gameLevels = GameLevels::THIRD_LEVEL;
            loadLevel(GameLevels::THIRD_LEVEL);

        }else if(hasComponent(ecs, entityB, PlayerTag) && hasComponent(ecs, entityA, PortalTag2)){
            gameState->gameLevels = GameLevels::THIRD_LEVEL;
            loadLevel(GameLevels::THIRD_LEVEL);
        }
    }
}

GAME_API void gameStart(Arena* gameArena, EngineState* engineState){
    engine = engineState;

    registerComponent(engine->ecs, PlayerTag);
    registerComponent(engine->ecs, ProjectileTag);
    registerComponent(engine->ecs, LifeTime);
    registerComponent(engine->ecs, WallTag);
    registerComponent(engine->ecs, PortalTag);
    registerComponent(engine->ecs, GunComponent);
    registerComponent(engine->ecs, OrbitingWeaponComponent);
    registerComponent(engine->ecs, OrbitingProjectile);
    registerComponent(engine->ecs, GranadeComponent);
    registerComponent(engine->ecs, ExplosionComponent);
    registerComponent(engine->ecs, ExplosionTag);
    registerComponent(engine->ecs, HasWeaponComponent);
    registerComponent(engine->ecs, CooldownComponent);
    registerComponent(engine->ecs, InputComponent);
    registerComponent(engine->ecs, WeaponTag);
    registerComponent(engine->ecs, PickupTag);
    registerComponent(engine->ecs, HitboxTag);
    registerComponent(engine->ecs, HurtboxTag);
    registerComponent(engine->ecs, HealthComponent);
    registerComponent(engine->ecs, DamageComponent);

    ////TODO: remove, it's vampire survival clone
    registerComponent(engine->ecs, PortalTag2);
    registerComponent(engine->ecs, EnemyTag);
    registerComponent(engine->ecs, ExperienceComponent);
    registerComponent(engine->ecs, ExperienceDrop);

    //if(engineState->gameState){
    //    return;
    //}
    //gameState = new GameState();
    //setRenderResolution(640, 320);
    if(gameArena->index > 0){
        return;
    }
    engine->mainCamera = createCamera({0,0,0}, 640, 320);
    gameState = arenaAllocStruct(gameArena, GameState);
    //engineState->gameState = gameState;
    //engineState->gameState = arenaAllocStruct(&engine->arena, GameState);
    gameState->cards[0] = {.description = "increase \ndamage \nof 20%", .dmg = 0.2f, .speed = 0, .cardChoice = CardChoice::CARD_DMG_UP};
    gameState->cards[1] = {.description = "increase \nspeed \nof 20%", .dmg = 0.0f, .speed = 0.2f, .cardChoice = CardChoice::CARD_SPEED_UP};
    gameState->cards[2] = {.description = "increase \nprojectile \nof 20%", .dmg = 0.0f, .speed = 0.0f, .radius = 0.2f, .cardChoice = CardChoice::CARD_SIZE_UP};
    gameState->cards[3] = {.description = "+1 projectiles", .cardChoice = CardChoice::CARD_ADD_PROJECTILE};
    gameState->cards[4] = {.description = "Add \nOrbit Weapon", .dmg = 0.0f, .speed = 0.0f, .radius = 0.2f, .cardChoice = CardChoice::CARD_ORBIT};
    gameState->cards[5] = {.description = "launch a\ngranade each\nsecond", .cardChoice = CardChoice::CARD_GRANADE};


    //gameState->gameLevels = GameLevels::MAIN_MENU;
    //engine->gameState = gameState;
    loadAudio("sfx/gaming-music.wav", true);
    loadAudio("sfx/gunshot.wav", false);
    loadFont("Creame");
    loadFont("Roboto-Regular");
    loadTexture("Golem-hurt");
    loadTexture("idle-walk");
    loadTexture("XOne");
    loadTexture("tileset01");
    loadTexture("dungeon");
    loadTexture("monster-1");
    loadTexture("hp_and_mp");
    loadTexture("weaponSprites");
    loadTexture("gobu walk");
    loadTexture("granade");
    playAudio("sfx/gaming-music.wav", 0.1f); //background sound

    //engine->mainCamera = createCamera({0,0,0}, 1920, 1080);

    //engine->mainCamera = engine->mainCamera;
    //return engineState->gameState;
}

GAME_API void gameRender(Arena* gameArena, EngineState* engine, float dt){}

GAME_API void gameUpdate(Arena* gameArena, EngineState* engineState, float dt){
    PROFILER_START();
    engine = (EngineState*) engineState;
    gameState = (GameState*)gameArena->memory;

    static Texture t = beginTextureMode(640, 320);
    clearColor(0,1,1,1);
        //beginUiFrame({0,0}, {engine->mainCamera.width, engine->mainCamera.height});
            //drawMenu();
            renderDrawFilledRect({0, 0}, {100, 100}, {0,0}, {1,0,0,1});
        //endUiFrame();
    endTextureMode();
    clearColor(0.2f, 0.3f, 0.3f, 1.0f);
    //NOTE: can be cached in the gameState
    EntityArray players = view(engine->ecs, ECS_TYPE(PlayerTag));
    Entity player = players.entities[0];
    TransformComponent* playerT = getComponent(engine->ecs, player, TransformComponent);
    if(playerT){
        setGridCenter(playerT->position.x, playerT->position.y);
    }

    beginScene();
        renderDrawQuad2D(&t, {0, 0}, {1,1}, {0,0}, {0,0}, t.size);
    endScene();


    switch (gameState->gameLevels)
    {
        case GameLevels::MAIN_MENU:{
            if(isJustPressedGamepad(GAMEPAD_BUTTON_START)){
                loadLevel(GameLevels::FIRST_LEVEL);
                gameState->gameLevels = GameLevels::FIRST_LEVEL;
            }

            handleMenuInput();
            beginUiFrame({0,0}, {engine->mainCamera.width, engine->mainCamera.height});
                drawMenu();
            endUiFrame();
            break;
        }
        case GameLevels::FIRST_LEVEL:{
            systemCheckRange(engine->ecs);
            systemProjectileHit(engine->ecs);
            animateTiles(&gameState->bgMap, dt);
            animationSystem(engine->ecs, dt);
            cooldownSystem(engine->ecs, dt);
            weaponFireSystem(engine->ecs, dt);
            inputPlayerSystem(engine->ecs, getInputState(), dt);
            moveSystem(engine->ecs, dt);
            cameraFollowSystem(engine->ecs, &engine->mainCamera);
            nextLevelSystem(engine->ecs);
            deathSystem(engine->ecs);

            PROFILER_SCOPE_START("rendering");
            beginScene(RenderMode::NORMAL);
                beginMode2D(engine->mainCamera);
                    systemRenderSprites(engine->ecs);
                    renderTileMap(&gameState->bgMap);
                endMode2D();

                drawHud(&engine->mainCamera, dt);
            endScene();
            //beginUiFrame({0,0}, {engine->mainCamera.width, engine->mainCamera.height});
            //endUiFrame();
            PROFILER_SCOPE_START("rendering");
            break;
        }
        case GameLevels::SECOND_LEVEL:{
            break;
        }
        case GameLevels::THIRD_LEVEL:{
            if(isJustPressedGamepad(GAMEPAD_BUTTON_START)){
                gameState->gameLevels = GameLevels::SELECT_CARD;
            }

            systemProjectileHit(engine->ecs);

            nextLevelSystem(engine->ecs);
            systemCheckRange(engine->ecs);
            cooldownSystem(engine->ecs, dt);
            weaponFireSystem(engine->ecs, dt);
            animationSystem(engine->ecs, dt);
            moveSystem(engine->ecs, dt);
            systemOrbitMovement(engine->ecs, dt);
            gatherExperienceSystem(engine->ecs, gameState);
            inputPlayerSystem(engine->ecs, getInputState(), dt);
            lifeTimeSystem(engine->ecs, dt);
            cameraFollowSystem(engine->ecs, &engine->mainCamera);
            systemUpdateEnemyDirection(engine->ecs);
            systemSpawnEnemies(engine->ecs, dt);
            explosionSystem(engine->ecs);
            deathSystem(engine->ecs);


            PROFILER_SCOPE_START("rendering");
            beginScene(RenderMode::NORMAL);
                beginMode2D(engine->mainCamera);
                    systemRenderSprites(engine->ecs);
                endMode2D();

                drawHud(&engine->mainCamera, dt);
            endScene();
            //beginUiFrame({0,0}, {engine->mainCamera.width, engine->mainCamera.height});
            //endUiFrame();
            PROFILER_SCOPE_END();
            break;
        }
        case GameLevels::SELECT_CARD:{
            beginScene(RenderMode::NORMAL);
                beginMode2D(engine->mainCamera);
                    systemRenderSprites(engine->ecs);
                endMode2D();

                drawCardSelectionMenu();
            endScene();
            //beginUiFrame({0,0}, {engine->mainCamera.width, engine->mainCamera.height});
            //endUiFrame();
            break;
        }
        case GameLevels::GAME_OVER:{
            clearColor(1.0f, 0.3f, 0.3f, 1.0f);
            //beginScene(engine->mainCamera, RenderMode::NORMAL);
            //    renderDrawText2D(getFont("Minecraft"),
            //                "GAME OVER!",
            //                {(engine->mainCamera.width  / 2) - 120,
            //                (engine->mainCamera.height / 2) - 24},
            //                1.0);
            //endScene();
            break;
        }
    }
    PROFILER_END();
}

GAME_API void gameStop(Arena* gameArena, EngineState* engine){
    destroyEcs(engine->ecs);
}