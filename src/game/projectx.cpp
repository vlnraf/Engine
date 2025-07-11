#include "projectx.hpp"
#include "components.hpp"

#include "player.hpp"
#include "boss.hpp"
#include "projectile.hpp"
#include "spike.hpp"
#include "lifetime.hpp"
#include "weapon.hpp"

#include "vampireclone.hpp"

#define ACTIVE_COLLIDER_COLOR glm::vec4(255.0f / 255.0f, 0, 255.0f / 255.0f, 255.0f  /255.0f)
#define DEACTIVE_COLLIDER_COLOR glm::vec4(128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 255.0f / 255.0f)
#define HIT_COLLIDER_COLOR glm::vec4(0 , 255.0f / 255.0f, 0, 255.0f  /255.0f)
#define HURT_COLLIDER_COLOR glm::vec4(255.0f / 255.0f, 0, 0, 255.0f / 255.0f)

static GameState* gameState;

void systemRenderColliders(Ecs* ecs){
    //setYsort(renderer, true);
    std::vector<Entity> entities = view(ecs, Box2DCollider);

    for(Entity entity : entities){
        //Need the position of the box which is dictated by the entity position + the box offset
        //glm::vec2 offset = {t->position.x + box->offset.x, t->position.y + box->offset.y};
        Box2DCollider* box= getComponent(ecs, entity, Box2DCollider);
        //TransformComponent* t= getComponent(ecs, entity, TransformComponent);
        //Box2DCollider b = calculateWorldAABB(t, box);
        //if(box->active){
            renderDrawRect(box->relativePosition, box->size, ACTIVE_COLLIDER_COLOR, 30);
        //}else{
        //    renderDrawRect(renderer, gameState->camera, b.offset, b.size, DEACTIVE_COLLIDER_COLOR, 30);
        //}
    }
}
void systemRenderHitBox(Ecs* ecs){
    std::vector<Entity> entities = view(ecs, HitBox, TransformComponent);

    for(Entity entity : entities){
        HitBox* hitBox= getComponent(ecs, entity, HitBox);
        //TransformComponent* t= getComponent(ecs, entity, TransformComponent);
        //Need the position of the box which is dictated by the entity position + the box offset
        //glm::vec2 offset = {t->position.x + box->offset.x, t->position.y + box->offset.y};
        //Box2DCollider hit = calculateCollider(t, hitBox->offset, hitBox->size);
        //if(hitBox->area.active){
            renderDrawRect(hitBox->relativePosition, hitBox->size, HIT_COLLIDER_COLOR, 30);
        //}else{
            //renderDrawRect(renderer, gameState->camera, hit.offset, hit.size, DEACTIVE_COLLIDER_COLOR, 30);
        //}
    }
}

void systemRenderHurtBox(Ecs* ecs){
    std::vector<Entity> entities = view(ecs, HurtBox, TransformComponent);

    for(Entity entity : entities){
        HurtBox* hurtBox= getComponent(ecs, entity, HurtBox);
        //TransformComponent* t= getComponent(ecs, entity, TransformComponent);
        //Need the position of the box which is dictated by the entity position + the box offset
        //glm::vec2 offset = {t->position.x + box->offset.x, t->position.y + box->offset.y};
        //Box2DCollider hurt = calculateCollider(t, hurtBox->offset, hurtBox->size);
        //if(hurtBox->area.active){
            renderDrawRect(hurtBox->relativePosition, hurtBox->size, HURT_COLLIDER_COLOR, 30);
        //}else{
            //renderDrawRect(renderer, gameState->camera, hurt.offset, hurt.size, DEACTIVE_COLLIDER_COLOR, 30);
        //}
    }
}

void systemUpdateColliderPosition(Ecs* ecs){
    std::vector<Entity> entities = view(ecs, Box2DCollider, TransformComponent);

    for(Entity entity : entities){
        Box2DCollider* box= getComponent(ecs, entity, Box2DCollider);
        TransformComponent* t= getComponent(ecs, entity, TransformComponent);
        Box2DCollider boxx = calculateCollider(t, box->offset, box->size);
        box->relativePosition = glm::vec2(boxx.offset.x, boxx.offset.y);
    }
}

void systemUpdateHitBoxPosition(Ecs* ecs){
    std::vector<Entity> entities = view(ecs, HitBox, TransformComponent);

    for(Entity entity : entities){
        HitBox* hitBox= getComponent(ecs, entity, HitBox);
        TransformComponent* t= getComponent(ecs, entity, TransformComponent);
        Box2DCollider hit = calculateCollider(t, hitBox->offset, hitBox->size);
        hitBox->relativePosition = glm::vec2(hit.offset.x, hit.offset.y);
    }
}

void systemUpdateHurtBoxPosition(Ecs* ecs){
    std::vector<Entity> entities = view(ecs, HurtBox, TransformComponent);

    for(Entity entity : entities){
        HurtBox* hurtbox= getComponent(ecs, entity, HurtBox);
        TransformComponent* t= getComponent(ecs, entity, TransformComponent);
        Box2DCollider hurt = calculateCollider(t, hurtbox->offset, hurtbox->size);
        hurtbox->relativePosition = glm::vec2(hurt.offset.x, hurt.offset.y);
    }
}

void systemCollision(Ecs* ecs){

    systemCheckCollisions(ecs);
    systemResolvePhysicsCollisions(ecs);

    systemProjectileHit(ecs);
    systemSpikeHit(ecs);
    systemRespondBossHitStaticEntity(ecs);
}


void systemRenderSprites(Ecs* ecs){
    std::vector<Entity> entities = view(ecs, TransformComponent, SpriteComponent);

    for(Entity entity : entities){
        TransformComponent* t= (TransformComponent*) getComponent(ecs, entity, TransformComponent);
        SpriteComponent* s= (SpriteComponent*) getComponent(ecs, entity, SpriteComponent);
        if(s->visible){
            renderDrawSprite(t->position, t->scale, t->rotation, s);
        }
    }
}

void moveSystem(Ecs* ecs, float dt){
    auto entities = view(ecs, TransformComponent, VelocityComponent, DirectionComponent);
    for(Entity e : entities){
        TransformComponent* transform = (TransformComponent*) getComponent(ecs, e, TransformComponent);
        VelocityComponent* velocity  = (VelocityComponent*)  getComponent(ecs, e, VelocityComponent);
        DirectionComponent* direction  = (DirectionComponent*)  getComponent(ecs, e, DirectionComponent);
        transform->position += glm::vec3(direction->dir.x * velocity->vel.x * dt, direction->dir.y * velocity->vel.y * dt, transform->position.z);
    }
}

void deathSystem(Ecs* ecs){
    auto entities = view(ecs, HurtBox);
    for(Entity e : entities){
        HurtBox* hurtbox = getComponent(ecs, e, HurtBox);
        if(hasComponent(ecs, e, PlayerTag)) continue;
        if(hurtbox->health <= 0){
            removeEntity(ecs, e);
        }
    }
}

void gameOverSystem(Ecs* ecs, GameState* gameState){
    auto entities = view(ecs, HurtBox, PlayerTag);
    for(Entity e : entities){
        HurtBox* hurtbox = getComponent(ecs, e, HurtBox);
        if(hurtbox->health <= 0){
            gameState->gameLevels = GameLevels::GAME_OVER;
        }
    }
}

//NOTE: forward declaration
void loadLevel(GameState* gameState, EngineState* engine, GameLevels level);

void secondLevelSystem(Ecs* ecs, EngineState* engine, GameState* gameState){
    auto player = view(ecs, PlayerTag, Box2DCollider);
    auto portal = view(ecs, PortalTag, Box2DCollider);

    for(Entity entityA : player){
        //Box2DCollider* boxAent = getComponent(ecs, entityA, Box2DCollider);
        for(Entity entityB : portal){
            //Box2DCollider* boxBent = getComponent(ecs, entityB, Box2DCollider);
            if(beginCollision(entityA , entityB)){
                gameState->gameLevels = GameLevels::SECOND_LEVEL;
                loadLevel(gameState, engine, GameLevels::SECOND_LEVEL);
                break;
            }
        }
    }
}

void thidLevelSystem(Ecs* ecs, EngineState* engine, GameState* gameState){
    auto player = view(ecs, PlayerTag, Box2DCollider);
    auto portal = view(ecs, PortalTag2, Box2DCollider);

    for(Entity entityA : player){
        //Box2DCollider* boxAent = getComponent(ecs, entityA, Box2DCollider);
        for(Entity entityB : portal){
            //Box2DCollider* boxBent = getComponent(ecs, entityB, Box2DCollider);
            if(beginCollision(entityA , entityB)){
                gameState->gameLevels = GameLevels::THIRD_LEVEL;
                loadLevel(gameState, engine, GameLevels::THIRD_LEVEL);
                break;
            }
        }
    }

}

void cameraFollowSystem(Ecs* ecs, OrtographicCamera* camera){
    std::vector<Entity> entities = view(ecs, PlayerTag);
    for(Entity entity : entities){
        TransformComponent* t = getComponent(ecs, entity, TransformComponent);
        if(!t){ break; }
        followTarget(camera, t->position);
    }
}

void animationSystem(Ecs* ecs, float dt){
    std::vector<Entity> entities = view(ecs, SpriteComponent, AnimationComponent);

    //NOTE: It should not be a system or it runs every frame and so even if the animation
    // is not showing it's been computed
    for(Entity entity : entities){
        SpriteComponent* s= getComponent(ecs, entity, SpriteComponent);
        AnimationComponent* animComp = getComponent(ecs, entity, AnimationComponent);
        Animation* anim = getAnimation(animComp->animationId);

        //if(component->id != component->previousId){ //NOTE: synchronize animation to frame 0 when it changes
        //    component->currentFrame = 0;
        //    component->previousId = component->id;
        //}

        if(anim->loop){
            if(animComp->elapsedTime > anim->frameDuration){
                animComp->currentFrame = (animComp->currentFrame + 1) % (anim->frames); // module to loop around
                animComp->elapsedTime = 0;
                //component->frameCount = 0;
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
}

void loadLevel(GameState* gameState, EngineState* engine, GameLevels level){
    PROFILER_START();
    clearEcs(engine->ecs);
    switch(level){
        case GameLevels::MAIN_MENU:{
            break;
        }
        case GameLevels::FIRST_LEVEL:{
            gameState->bgMap = LoadTilesetFromTiled("test", engine->ecs);
            createPlayer(engine->ecs, gameState->camera);
            //TODO: make default values for the components
            //directly in hpp file or just make utility functions to create the components???
            TransformComponent transform = {    
                .position = {50.0f, 50.0f, 0.0f},
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
            PortalTag p = {};
            Entity portal = createEntity(engine->ecs);
            //transform.position = {0,0,0};
            //sprite.size = {10, gameState->camera.height};
            pushComponent(engine->ecs, portal, TransformComponent, &transform);
            pushComponent(engine->ecs, portal, Box2DCollider, &coll);
            pushComponent(engine->ecs, portal, SpriteComponent, &sprite);
            pushComponent(engine->ecs, portal, PortalTag, &p);

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
                //transform.position = {0,0,0};
                //sprite.size = {10, gameState->camera.height};
                pushComponent(engine->ecs, portal, TransformComponent, &transform);
                pushComponent(engine->ecs, portal, Box2DCollider, &coll);
                pushComponent(engine->ecs, portal, SpriteComponent, &sprite);
                pushComponent(engine->ecs, portal, PortalTag2, &p);

            }
            int center = (gameState->bgMap.layers[1].mapWidth * gameState->bgMap.tileWidth) / 2;
            int nextPosition = 0;
            int padding = 40;
            {
                //Entity gun = createEntity(engine->ecs);
                Entity gun = createGun(engine->ecs);
                SpriteComponent sprite = {
                    .texture = getTexture("weaponSprites"),
                    .index = {0,7},
                    .size = {16, 16},
                    .tileSize {16, 16},
                    .ySort = true,
                    .layer = 1.0f
                };
                int position = center - (sprite.size.x / 2.0f) - padding;
                nextPosition = center - padding + (sprite.size.x / 2.0f);
                TransformComponent transform = {    
                    .position = {position, 200.0f, 0.0f},
                    .scale = {1.0f, 1.0f, 0.0f},
                    .rotation = {0.0f, 0.0f, 0.0f}
                };
                Box2DCollider coll = {.type = Box2DCollider::STATIC, .offset = {0,0}, .size = {16, 16}, .isTrigger = true};
                pushComponent(engine->ecs, gun, TransformComponent, &transform);
                pushComponent(engine->ecs, gun, Box2DCollider, &coll);
                pushComponent(engine->ecs, gun, SpriteComponent, &sprite);
            }
            {
                //Entity shotgun = createEntity(engine->ecs);
                Entity shotgun = createShotgun(engine->ecs);
                SpriteComponent sprite = {
                    .texture = getTexture("weaponSprites"),
                    .index = {4,5},
                    .size = {32, 16},
                    .tileSize {16, 16},
                    .ySort = true,
                    .layer = 1.0f
                };
                int position = nextPosition + padding;
                nextPosition = position + (sprite.size.x / 2.0f) + (padding - (sprite.size.x / 2.0f));
                TransformComponent transform = {    
                    .position = {position, 200.0f, 0.0f},
                    .scale = {1.0f, 1.0f, 0.0f},
                    .rotation = {0.0f, 0.0f, 0.0f}
                };
                Box2DCollider coll = {.type = Box2DCollider::STATIC, .offset = {0,0}, .size = {32, 16}, .isTrigger = true};
                pushComponent(engine->ecs, shotgun, TransformComponent, &transform);
                pushComponent(engine->ecs, shotgun, Box2DCollider, &coll);
                pushComponent(engine->ecs, shotgun, SpriteComponent, &sprite);
            }
            {
                Entity sniper = createSniper(engine->ecs);
                SpriteComponent sprite = {
                    .texture = getTexture("weaponSprites"),
                    .index = {10,3},
                    .size = {48, 16},
                    .tileSize {16, 16},
                    .ySort = true,
                    .layer = 1.0f
                };
                int position = nextPosition + padding;
                TransformComponent transform = {    
                    .position = {position, 200.0f, 0.0f},
                    .scale = {1.0f, 1.0f, 0.0f},
                    .rotation = {0.0f, 0.0f, 0.0f}
                };
                Box2DCollider coll = {.type = Box2DCollider::STATIC, .offset = {0,0}, .size = {48, 16}, .isTrigger = true};
                pushComponent(engine->ecs, sniper, TransformComponent, &transform);
                pushComponent(engine->ecs, sniper, Box2DCollider, &coll);
                pushComponent(engine->ecs, sniper, SpriteComponent, &sprite);
            }
            break;
        }
        case GameLevels::SECOND_LEVEL:{
            gameState->camera.position = {0,0,0};
            gameState->camera.view = glm::mat4(1.0f);
            //Load Sound
            playAudio("sfx/celeste-test.ogg");

            //gameState->ecs = initEcs();

            //createPlayer(engine->ecs, gameState->camera);

            createBoss(engine->ecs, gameState->camera);

            //Walls
            {
                WallTag wallTag = {};
                //TODO: make default values for the components
                //directly in hpp file or just make utility functions to create the components???
                TransformComponent transform = {    
                    .position = {50.0f, 50.0f, 0.0f},
                    .scale = {1.0f, 1.0f, 0.0f},
                    .rotation = {0.0f, 0.0f, 0.0f}
                };
                SpriteComponent sprite = {
                    .texture = getTexture("default"),
                    .index = {0,0},
                    .size = {16, 16},
                    .ySort = true,
                    .layer = 1.0f
                };
                Box2DCollider collider = {};
                Entity leftEdge = createEntity(engine->ecs);
                transform.position = {0,0,0};
                sprite.size = {10, gameState->camera.height};
                collider = {.type = Box2DCollider::STATIC, .offset = {0, 0}, .size = {10, gameState->camera.height}};
                pushComponent(engine->ecs, leftEdge, TransformComponent, &transform);
                pushComponent(engine->ecs, leftEdge, Box2DCollider, &collider);
                pushComponent(engine->ecs, leftEdge, SpriteComponent, &sprite);
                pushComponent(engine->ecs, leftEdge, WallTag, &wallTag);
                Entity rightEdge = createEntity(engine->ecs);
                transform.position = {gameState->camera.width - 10,0,0};
                sprite.size = {10, gameState->camera.height};
                collider = {.type = Box2DCollider::STATIC, .offset = {0, 0}, .size = {10, gameState->camera.height}};
                pushComponent(engine->ecs, rightEdge, TransformComponent, &transform);
                pushComponent(engine->ecs, rightEdge, Box2DCollider, &collider);
                pushComponent(engine->ecs, rightEdge, SpriteComponent, &sprite);
                pushComponent(engine->ecs, rightEdge, WallTag, &wallTag);
                Entity bottomEdge = createEntity(engine->ecs);
                transform.position = {0,0,0};
                sprite.size = {gameState->camera.width, 10};
                collider = {.type = Box2DCollider::STATIC, .offset = {0, 0}, .size = {gameState->camera.width, 10}};
                pushComponent(engine->ecs, bottomEdge, TransformComponent, &transform);
                pushComponent(engine->ecs, bottomEdge, Box2DCollider, &collider);
                pushComponent(engine->ecs, bottomEdge, SpriteComponent, &sprite);
                pushComponent(engine->ecs, bottomEdge, WallTag, &wallTag);
                Entity topEdge = createEntity(engine->ecs);
                transform.position = {0,gameState->camera.height - 10,0};
                sprite.size = {gameState->camera.width, 10};
                collider = {.type = Box2DCollider::STATIC, .offset = {0,0}, .size = {gameState->camera.width, 10}};
                pushComponent(engine->ecs, topEdge, TransformComponent, &transform);
                pushComponent(engine->ecs, topEdge, Box2DCollider, &collider);
                pushComponent(engine->ecs, topEdge, SpriteComponent, &sprite);
                pushComponent(engine->ecs, topEdge, WallTag, &wallTag);
            }
            //UI
            //button("ciao Mondo", {10,10}, {200,200});
            break;
        }
        case GameLevels::THIRD_LEVEL:{
            //createPlayer(engine->ecs, gameState->camera);
            break;
        }
        default:
            break;
    }
    PROFILER_END();
}

void applyDmgUp(EngineState* engine, GameState* gameState, float dmgMultiplier){
    auto entities = view(engine->ecs, PlayerTag);
    for(Entity e : entities){
        HasWeaponComponent* hasWeapon = getComponent(engine->ecs, e, HasWeaponComponent);
        if(hasWeapon->weaponType == GUN){
            GunComponent* gun = getComponent(engine->ecs, hasWeapon->weaponId, GunComponent);
            gun->dmg = gun->dmg + (gun->dmg * dmgMultiplier);
            hasWeapon->weaponType = GUN;
        }else if(hasWeapon->weaponType == SHOTGUN){
            ShotgunComponent* gun = getComponent(engine->ecs, hasWeapon->weaponId, ShotgunComponent);
            gun->dmg = gun->dmg + (gun->dmg * dmgMultiplier);
            hasWeapon->weaponType = SHOTGUN;
        }else if(hasWeapon->weaponType == SNIPER){
            SniperComponent* gun = getComponent(engine->ecs, hasWeapon->weaponId, SniperComponent);
            gun->dmg = gun->dmg + (gun->dmg * dmgMultiplier);
            hasWeapon->weaponType = SNIPER;
        }
    }
    gameState->pause = false;
    gameState->gameLevels = GameLevels::THIRD_LEVEL;
}

void applySpeedUp(EngineState* engine, GameState* gameState, float speedUp){
    auto player = view(engine->ecs, PlayerTag);
    //PlayerTag* playerTag = getComponent(engine->ecs, player[0], PlayerTag);
    VelocityComponent* vel = getComponent(engine->ecs, player[0], VelocityComponent);
    vel->vel += (vel->vel * speedUp);
    gameState->pause = false;
    gameState->gameLevels = GameLevels::THIRD_LEVEL;
}

void applyIncreaseRadius(EngineState* engine, GameState* gameState, float radius){
    auto projectiles = view(engine->ecs, ProjectileTag, HitBox);
    auto weapons = view(engine->ecs, HasWeaponComponent);
    HasWeaponComponent* hasWeapon = getComponent(engine->ecs, weapons[0], HasWeaponComponent);
    if(hasComponent(engine->ecs, hasWeapon->weaponId, GunComponent)){
        GunComponent* gun = getComponent(engine->ecs, hasWeapon->weaponId, GunComponent);
        gun->radius = gun->radius + (gun->radius * radius);
    }else if(hasComponent(engine->ecs, hasWeapon->weaponId, ShotgunComponent)){
        ShotgunComponent* gun = getComponent(engine->ecs, hasWeapon->weaponId, ShotgunComponent);
        gun->radius = gun->radius + (gun->radius * radius);
    }else if(hasComponent(engine->ecs, hasWeapon->weaponId, SniperComponent)){
        SniperComponent* gun = getComponent(engine->ecs, hasWeapon->weaponId, SniperComponent);
        gun->radius = gun->radius + (gun->radius * radius);
    }
    gameState->pause = false;
    gameState->gameLevels = GameLevels::THIRD_LEVEL;
}

void drawCardSelectionMenu(EngineState* engine, GameState* gameState){
    //horizontal layout
    int xCenter = gameState->camera.width * 0.5f;
    int yCenter = gameState->camera.height * 0.5f;
    int padding = 10;
    int layoutWidth = 0;

    int buttonWidth = 100;
    int buttonHeight = 100;
    // calculate 3 buttons
    layoutWidth += (3 * (buttonWidth + padding));
    int buttonX = xCenter - (layoutWidth / 2);
    if(UiButton(gameState->cards[0].description, {buttonX, yCenter - (buttonHeight * 0.5f)},{buttonWidth, buttonHeight}, {0,0})){
        LOGINFO("attack up");
        float dmgMultiplier = gameState->cards[0].dmg;
        applyDmgUp(engine, gameState, dmgMultiplier);
    }
    buttonX += (layoutWidth / 3);
    if(UiButton(gameState->cards[1].description, {buttonX, yCenter - (buttonHeight * 0.5f)},{buttonWidth, buttonHeight}, {0,0})){
        LOGINFO("speed up");
        float speedMultiplier = gameState->cards[1].speed;
        applySpeedUp(engine, gameState, speedMultiplier);
    }
    buttonX += (layoutWidth / 3);
    if(UiButton(gameState->cards[2].description, {buttonX, yCenter - (buttonHeight * 0.5f)},{buttonWidth, buttonHeight}, {0,0})){
        float radius = gameState->cards[2].radius;
        applyIncreaseRadius(engine, gameState, radius);
        LOGINFO("nothing");
    }
}

void drawHud(EngineState* engine, GameState* gameState, float dt){
    //beginUIFrame({0,0}, {engine->windowWidth, engine->windowHeight});
        //clearColor(0.2f, 0.3f, 0.3f, 1.0f);

    //int y = gameState->camera.height;
    //int x = 0;
    //Font* font = getFont("Minecraft");
    auto player = view(engine->ecs, PlayerTag, HurtBox);
    HurtBox* h = getComponent(engine->ecs, player[0], HurtBox);
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.0f / %d HP", h->health, 100);
    UiText(buffer, {30, 20}, 0.2f);
    char fpsText[30];
    static float timer = 0;
    static float ffps = 0;
    timer += dt;
    if(timer >= 1.0f){ //each second
        ffps = engine->fps;
        timer = 0;
    }
    snprintf(fpsText, sizeof(fpsText), "%.2f FPS", ffps);
    UiText(fpsText, {gameState->camera.width - calculateTextWidth(getFont("Minecraft") , fpsText, 0.2f) - 10, 20}, 0.2f);
    //UiButton("ciao ciao \nciao ciao", {0, 0},{100, 100}, {0,0});
}

GAME_API void gameStart(EngineState* engine){
    //Always do that right now, i need to figure out how to remove this block of code
    #ifdef _WIN32
     if (!gladLoadGL()) {
        LOGERROR("GLAD not loaded properly in DLL.");
        return;
    }
    #endif
    PROFILER_START();

    registerComponent(engine->ecs, PersistentTag);
    registerComponent(engine->ecs, TransformComponent);
    registerComponent(engine->ecs, SpriteComponent);
    registerComponent(engine->ecs, DirectionComponent);
    registerComponent(engine->ecs, VelocityComponent);
    registerComponent(engine->ecs, Box2DCollider);
    registerComponent(engine->ecs, PlayerTag);
    registerComponent(engine->ecs, ProjectileTag);
    registerComponent(engine->ecs, BossTag);
    registerComponent(engine->ecs, HitBox);
    registerComponent(engine->ecs, HurtBox);
    registerComponent(engine->ecs, SpikeTag);
    registerComponent(engine->ecs, LifeTime);
    registerComponent(engine->ecs, WallTag);
    registerComponent(engine->ecs, GamepadSpriteTag);
    registerComponent(engine->ecs, PortalTag);
    registerComponent(engine->ecs, AnimationComponent);
    registerComponent(engine->ecs, GunComponent);
    registerComponent(engine->ecs, ShotgunComponent);
    registerComponent(engine->ecs, SniperComponent);
    registerComponent(engine->ecs, HasWeaponComponent);
    registerComponent(engine->ecs, CooldownComponent);
    registerComponent(engine->ecs, InputComponent);
    registerComponent(engine->ecs, WeaponTag);

    //TODO: remove, it's vampire survival clone
    registerComponent(engine->ecs, PortalTag2);
    registerComponent(engine->ecs, EnemyTag);
    registerComponent(engine->ecs, ExperienceComponent);

    gameState = new GameState();
    gameState->gameLevels = GameLevels::MAIN_MENU;
    engine->gameState = gameState;
    gameState->camera = createCamera({0,0,0}, 640, 320);
    loadAudio("sfx/celeste-test.ogg");
    loadFont("Minecraft");
    loadFont("Creame");
    loadTexture("Golem-hurt");
    loadTexture("idle-walk");
    loadTexture("XOne");
    loadTexture("tileset01");
    loadTexture("dungeon");
    loadTexture("idle-walk");
    loadTexture("monster-1");
    loadTexture("hp_and_mp");
    loadTexture("weaponSprites");
    loadTexture("gobu walk");

    //setFontUI(getFont("Creame"));

    //TileSet simple = createTileSet(getTexture(engine->textureManager, "tileset01"), 32, 32);
    //std::vector<int> tileBg = loadTilemapFromFile("assets/map/map-bg.csv", simple, 30);
    //std::vector<int> tileFg = loadTilemapFromFile("assets/map/map-fg.csv", simple, 30);


    //gameState->bgMap = createTilemap(tileBg, 30, 20, 32, simple);
    //gameState->fgMap = createTilemap(tileFg, 30, 20, 32, simple);

    //-----------------------------------------------------------------------------------
    loadLevel(gameState, engine, GameLevels::MAIN_MENU);
    PROFILER_END();
}

void pickupWeaponSystem(Ecs* ecs){
    std::vector<Entity> entities = view(ecs, WeaponTag);
    std::vector<Entity> players = view(ecs, PlayerTag);
    for(Entity e : entities){
        InputComponent* inputComponent = getComponent(ecs, players[0], InputComponent);
        if(!inputComponent->pickUp){continue;}
        if(isColliding(e, players[0]) && hasComponent(ecs, players[0], HasWeaponComponent)){
            if(hasComponent(ecs, e, GunComponent)){
                LOGINFO("Gun");
                HasWeaponComponent* hasWeapon = getComponent(ecs, players[0], HasWeaponComponent);
                removeEntity(ecs, e);
                Entity gun = createGun(ecs);
                hasWeapon->weaponId = gun;
                hasWeapon->weaponType = GUN;
                PersistentTag p = {};
                pushComponent(ecs, gun, PersistentTag, &p);
                continue;
            }else if(hasComponent(ecs, e, ShotgunComponent)){
                LOGINFO("Shotgun");
                HasWeaponComponent* hasWeapon = getComponent(ecs, players[0], HasWeaponComponent);
                removeEntity(ecs, e);
                Entity gun = createShotgun(ecs);
                hasWeapon->weaponId = gun;
                hasWeapon->weaponType = SHOTGUN;
                PersistentTag p = {};
                pushComponent(ecs, gun, PersistentTag, &p);
                continue;
            }else if(hasComponent(ecs, e, SniperComponent)){
                LOGINFO("Sniper");
                HasWeaponComponent* hasWeapon = getComponent(ecs, players[0], HasWeaponComponent);
                removeEntity(ecs, e);
                Entity gun = createSniper(ecs);
                hasWeapon->weaponId = gun;
                hasWeapon->weaponType = SNIPER;
                PersistentTag p = {};
                pushComponent(ecs, gun, PersistentTag, &p);
                continue;
            }
        }else if(isColliding(e, players[0]) && !hasComponent(ecs, players[0], HasWeaponComponent)){
            if(hasComponent(ecs, e, GunComponent)){
                LOGINFO("Gun");
                removeEntity(ecs, e);
                Entity gun = createGun(ecs);
                HasWeaponComponent hasWeapon = {.weaponId = gun};
                hasWeapon.weaponType = GUN;
                pushComponent(ecs, players[0], HasWeaponComponent, &hasWeapon);
                PersistentTag p = {};
                pushComponent(ecs, gun, PersistentTag, &p);
                continue;
            }else if(hasComponent(ecs, e, ShotgunComponent)){
                LOGINFO("Shotgun");
                removeEntity(ecs, e);
                Entity gun = createShotgun(ecs);
                HasWeaponComponent hasWeapon = {.weaponId = gun};
                hasWeapon.weaponType = SHOTGUN;
                pushComponent(ecs, players[0], HasWeaponComponent, &hasWeapon);
                PersistentTag p = {};
                pushComponent(ecs, gun, PersistentTag, &p);
                continue;
            }else if(hasComponent(ecs, e, SniperComponent)){
                LOGINFO("Sniper");
                removeEntity(ecs, e);
                Entity gun = createSniper(ecs);
                HasWeaponComponent hasWeapon = {.weaponId = gun};
                hasWeapon.weaponType = SNIPER;
                pushComponent(ecs, players[0], HasWeaponComponent, &hasWeapon);
                PersistentTag p = {};
                pushComponent(ecs, gun, PersistentTag, &p);
                continue;
            }
        }
        if(hasComponent(ecs, players[0], HasWeaponComponent) && getComponent(ecs, players[0], HasWeaponComponent)->weaponId != e){
            removeComponent(ecs, e, PersistentTag);
        }
    }
}

void drawWeaponDescription(Ecs* ecs, GameState* gameState){
    //glm::vec2 canvasSize = {gameState->camera.width, gameState->camera.height};
    //beginUiFrame({0,0}, {canvasSize.x, canvasSize.y});
    std::vector<Entity> entities = view(ecs, WeaponTag, SpriteComponent);
    std::vector<Entity> players = view(ecs, PlayerTag);
    int padding = 20;
    for(Entity e : entities){
        if(isColliding(e, players[0])){
            if(hasComponent(ecs, e, GunComponent)){
                //TransformComponent* t = getComponent(ecs, e, TransformComponent);
                Box2DCollider* box = getComponent(ecs, e, Box2DCollider);
                glm::vec2 position = box->relativePosition;
                position.y += box->size.y;
                position = worldToScreen(gameState->camera, {position, 0.0f});
                int textHeight = UigetTextHeight("Gun", 0.2f);
                position.y -= textHeight;
                position.y -= padding;
                UiText("Gun", position, 0.4f);
            }else if(hasComponent(ecs, e, ShotgunComponent)){
                Box2DCollider* box = getComponent(ecs, e, Box2DCollider);
                glm::vec2 position = box->relativePosition;
                position.y += box->size.y;
                position = worldToScreen(gameState->camera, {position, 0.0f});
                int textHeight = UigetTextHeight("Shotgun", 0.2f);
                position.y -= textHeight;
                position.y -= padding;
                UiText("Shotgun", position, 0.4f);
            }else if(hasComponent(ecs, e, SniperComponent)){
                Box2DCollider* box = getComponent(ecs, e, Box2DCollider);
                glm::vec2 position = box->relativePosition;
                position.y += box->size.y;
                position = worldToScreen(gameState->camera, {position, 0.0f});
                int textHeight = UigetTextHeight("Sniper", 0.2f);
                position.y -= textHeight;
                position.y -= padding;
                UiText("Sniper", position, 0.4f);
            }
        }
    }
    //endUiFrame();
}

GAME_API void gameRender(EngineState* engine, GameState* gameState, float dt){
//    PROFILER_START();
//    //if(gameState->pause){
//    //    renderPowerUpCards();
//    //    //return;
//    //}
//    //beginScene(gameState->camera, RenderMode::NORMAL);
//    //    clearColor(0.2f, 0.3f, 0.3f, 1.0f);
//    //    animationSystem(engine->ecs, dt);
//    //    systemRenderSprites(gameState, engine->ecs, dt);
//    //endScene();
//
//
//        switch (gameState->gameLevels)
//        {
//            case GameLevels::MAIN_MENU:{
//                break;
//            }
//            case GameLevels::FIRST_LEVEL:
//                break;
//            case GameLevels::SECOND_LEVEL:
//                break;
//
//            case GameLevels::THIRD_LEVEL:
//                break;
//
//            case GameLevels::GAME_OVER:
//                //TODO: refactor because i don't wanna begin and end a new scene
//                //The problem is that i still render game sprites but i render this on top
//                //so it's poorly optimized and can generate multiple errors
//                break;
//        }
//    PROFILER_END();
}

GAME_API void gameUpdate(EngineState* engine, GameState* gameState, float dt){
    PROFILER_START();
    //if(gameState->pause){
    //    return;
    //}
    if(isJustPressed(KEYS::F5)){
        gameState->debugMode = !gameState->debugMode;
    }
    clearColor(0.2f, 0.3f, 0.3f, 1.0f);

    switch (gameState->gameLevels)
    {
        case GameLevels::MAIN_MENU:{
            if(isJustPressedGamepad(GAMEPAD_BUTTON_START)){
                gameState->gameLevels = GameLevels::FIRST_LEVEL;
                loadLevel(gameState, engine, GameLevels::FIRST_LEVEL);
            }
            glm::vec2 canvasSize = {gameState->camera.width, gameState->camera.height};
            int y = gameState->camera.height;
            int x = 0;
            int padding = 10;

            beginUiFrame({0,0}, {canvasSize.x, canvasSize.y});
                Texture* controllerTexture = getTexture("Xone");
                //renderDrawQuad2D(controllerTexture, {x + (controllerTexture->width / 2), (y / 2) - (controllerTexture->height / 2)},{1,1},{0,0}, {0,0}, {controllerTexture->width, controllerTexture->height});
                UiImage(controllerTexture, {x + (controllerTexture->width / 2), (y / 2) - (controllerTexture->height / 2)}, {0,0});
                uint32_t textWidth = calculateTextWidth(getFont("Minecraft"), "Press Start to play the Game!!!", 0.5f);
                UiText("Press Start to play the Game!!!", {x + (canvasSize.x / 2) - (textWidth / 2) , (y / 2) + (controllerTexture->height / 2) + padding}, 0.5f);
            endUiFrame();
            break;
        }
        case GameLevels::FIRST_LEVEL:{
            animateTiles(&gameState->bgMap, dt);
            animationSystem(engine->ecs, dt);
            cooldownSystem(engine->ecs, dt);
            weaponFireSystem(engine->ecs);
            systemUpdateColliderPosition(engine->ecs);
            systemUpdateHitBoxPosition(engine->ecs);
            systemUpdateHurtBoxPosition(engine->ecs);
            systemCollision(engine->ecs);
            systemCheckRange(engine->ecs);
            deathSystem(engine->ecs);
            inputPlayerSystem(engine->ecs, getInputState(), dt);
            moveSystem(engine->ecs, dt);
            cameraFollowSystem(engine->ecs, &gameState->camera);
            pickupWeaponSystem(engine->ecs);
            secondLevelSystem(engine->ecs, engine, gameState);
            thidLevelSystem(engine->ecs, engine, gameState);


            beginScene(gameState->camera, RenderMode::NORMAL);
                systemRenderSprites(engine->ecs);
                //renderDrawQuad({10,10,10},{1,1,1},{0,0,0}, getTexture("XOne"), {0,0}, {200,200}, false);
                renderTileMap(&gameState->bgMap);
                //renderTileSet(engine->renderer, gameState->bgMap.tileset, gameState->camera);
                //renderTileMap(engine->renderer, gameState->fgMap, gameState->camera, 1.0f, true);
            endScene();
            beginUiFrame({0,0}, {gameState->camera.width, gameState->camera.height});
                drawHud(engine, gameState, dt);
                drawWeaponDescription(engine->ecs, gameState);
            endUiFrame();

            //TODO: delte
            //It's a code snippet to convert mouse position to world position and check collision
            {
                glm::vec2 mousePos = getMousePos();
                mousePos = screenToWorld(gameState->camera, {engine->windowWidth, engine->windowHeight}, mousePos);

                auto player = view(engine->ecs, PortalTag);
                if(player.size() <= 0) break;
                TransformComponent* t = getComponent(engine->ecs, player[0], TransformComponent);
                Box2DCollider* h = getComponent(engine->ecs, player[0], Box2DCollider);
                Box2DCollider b = calculateCollider(t, h->offset, h->size);
                //LOGINFO("box: {pos :[%f / %f], size: [%f / %f]}, mouse : %f / %f", b.offset.x, b.offset.y, b.size.x, b.size.y, mousePos.x, mousePos.y);
                //LOGINFO("mouse: {pos :[%f / %f]", mousePos.x, mousePos.y);
                if(pointRectIntersection(mousePos, b.offset, b.size)){
                    LOGINFO("CIAO");
                }
            }
            break;
        }
        case GameLevels::SECOND_LEVEL:{
            systemUpdateColliderPosition(engine->ecs);
            systemUpdateHitBoxPosition(engine->ecs);
            systemUpdateHurtBoxPosition(engine->ecs);
            cooldownSystem(engine->ecs, dt);
            weaponFireSystem(engine->ecs);
            systemCollision(engine->ecs);
            gameOverSystem(engine->ecs, gameState);
            systemCheckRange(engine->ecs);
            bossAiSystem(engine->ecs, dt);
            deathSystem(engine->ecs);
            moveSystem(engine->ecs, dt);
            inputPlayerSystem(engine->ecs, getInputState(), dt);
            lifeTimeSystem(engine->ecs, dt);
            changeBossTextureSystem(engine->ecs);

            beginScene(gameState->camera, RenderMode::NORMAL);
                animationSystem(engine->ecs, dt);
                systemRenderSprites(engine->ecs);
            endScene();
            break;
        }
        case GameLevels::THIRD_LEVEL:{
            systemUpdateColliderPosition(engine->ecs);
            systemUpdateHitBoxPosition(engine->ecs);
            systemUpdateHurtBoxPosition(engine->ecs);
            cooldownSystem(engine->ecs, dt);
            weaponFireSystem(engine->ecs);
            systemCollision(engine->ecs);
            gameOverSystem(engine->ecs, gameState);
            systemCheckRange(engine->ecs);
            deathEnemySystem(engine->ecs);
            animationSystem(engine->ecs, dt);
            moveSystem(engine->ecs, dt);
            gatherExperienceSystem(engine->ecs, gameState);
            inputPlayerSystem(engine->ecs, getInputState(), dt);
            lifeTimeSystem(engine->ecs, dt);
            cameraFollowSystem(engine->ecs, &gameState->camera);
            systemSpawnEnemies(engine->ecs, 1, dt);
            systemUpdateEnemyDirection(engine->ecs);
            systemEnemyHitPlayer(engine->ecs);

            beginScene(gameState->camera, RenderMode::NORMAL);
                systemRenderSprites(engine->ecs);
                //renderDrawQuad({10,10,10},{1,1,1},{0,0,0}, getTexture("XOne"), {0,0}, {200,200}, false);
                //renderTileSet(engine->renderer, gameState->bgMap.tileset, gameState->camera);
                //renderTileMap(engine->renderer, gameState->fgMap, gameState->camera, 1.0f, true);
            endScene();
            beginUiFrame({0,0}, {gameState->camera.width, gameState->camera.height});
                drawHud(engine, gameState, dt);
            endUiFrame();
            break;
        }
        case GameLevels::SELECT_CARD:{
            beginScene(gameState->camera, RenderMode::NORMAL);
                systemRenderSprites(engine->ecs);
            endScene();
            beginUiFrame({0,0}, {gameState->camera.width, gameState->camera.height});
                drawCardSelectionMenu(engine, gameState);
            endUiFrame();
            break;
        }
        case GameLevels::GAME_OVER:{
            clearColor(1.0f, 0.3f, 0.3f, 1.0f);
            beginScene(gameState->camera, RenderMode::NORMAL);
                renderDrawText2D(getFont("Minecraft"),
                            "GAME OVER!",
                            {(gameState->camera.width  / 2) - 120,
                            (gameState->camera.height / 2) - 24},
                            1.0);
            endScene();
            break;
        }
    }
    //LOGINFO("%d", engine->ecs->entities);
    if(gameState->debugMode){
        beginScene(gameState->camera, RenderMode::NO_DEPTH);
            systemRenderColliders(engine->ecs);
            systemRenderHitBox(engine->ecs);
            systemRenderHurtBox(engine->ecs);
        endScene();
    }
    PROFILER_END();
}

GAME_API void gameStop(EngineState* engine, GameState* gameState){
    destroyEcs(engine->ecs);
    delete gameState;
}