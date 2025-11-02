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

//ECS_DECLARE_COMPONENT(Box2DCollider)
//ECS_DECLARE_COMPONENT(HitBox)
//ECS_DECLARE_COMPONENT(HurtBox)

#define ACTIVE_COLLIDER_COLOR glm::vec4(255.0f / 255.0f, 0, 255.0f / 255.0f, 255.0f  /255.0f)
#define DEACTIVE_COLLIDER_COLOR glm::vec4(128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 255.0f / 255.0f)
#define HIT_COLLIDER_COLOR glm::vec4(0 , 255.0f / 255.0f, 0, 255.0f  /255.0f)
#define HURT_COLLIDER_COLOR glm::vec4(255.0f / 255.0f, 0, 0, 255.0f / 255.0f)

static GameState* gameState;
static EngineState* engine;

void systemRenderColliders(Ecs* ecs){
    //setYsort(renderer, true);
    EntityArray entities = view(ecs, ECS_TYPE(Box2DCollider));

    //for(Entity entity : entities){
    for(size_t i = 0; i < entities.count; i++){
        Entity entity = entities.entities[i];
        //Need the position of the box which is dictated by the entity position + the box offset
        //glm::vec2 offset = {t->position.x + box->offset.x, t->position.y + box->offset.y};
        Box2DCollider* box= (Box2DCollider*) getComponent(ecs, entity, Box2DCollider);
        //TransformComponent* t= getComponent(ecs, entity, TransformComponent);
        //Box2DCollider b = calculateWorldAABB(t, box);
        //if(box->active){
            renderDrawRect(box->relativePosition, box->size, ACTIVE_COLLIDER_COLOR, 30);
        //}else{
        //    renderDrawRect(renderer, gameState->camera, b.offset, b.size, DEACTIVE_COLLIDER_COLOR, 30);
        //}
    }
}

//void systemRenderHitBox(Ecs* ecs){
//    EntityArray entities = view(ecs, ECS_TYPE(HitBox), ECS_TYPE(TransformComponent));
//
//    //for(Entity entity : entities){
//    for(size_t i = 0; i < entities.count; i++){
//        Entity entity = entities.entities[i];
//        HitBox* hitBox= (HitBox*) getComponent(ecs, entity, HitBox);
//        //TransformComponent* t= getComponent(ecs, entity, TransformComponent);
//        //Need the position of the box which is dictated by the entity position + the box offset
//        //glm::vec2 offset = {t->position.x + box->offset.x, t->position.y + box->offset.y};
//        //Box2DCollider hit = calculateCollider(t, hitBox->offset, hitBox->size);
//        //if(hitBox->area.active){
//            renderDrawRect(hitBox->relativePosition, hitBox->size, HIT_COLLIDER_COLOR, 30);
//        //}else{
//            //renderDrawRect(renderer, gameState->camera, hit.offset, hit.size, DEACTIVE_COLLIDER_COLOR, 30);
//        //}
//    }
//}
//
//void systemRenderHurtBox(Ecs* ecs){
//    EntityArray entities = view(ecs, ECS_TYPE(HurtBox), ECS_TYPE(TransformComponent));
//
//    //for(Entity entity : entities){
//    for(size_t i = 0; i < entities.count; i++){
//        Entity entity = entities.entities[i];
//        HurtBox* hurtBox= (HurtBox*)getComponent(ecs, entity, HurtBox);
//        //TransformComponent* t= getComponent(ecs, entity, TransformComponent);
//        //Need the position of the box which is dictated by the entity position + the box offset
//        //glm::vec2 offset = {t->position.x + box->offset.x, t->position.y + box->offset.y};
//        //Box2DCollider hurt = calculateCollider(t, hurtBox->offset, hurtBox->size);
//        //if(hurtBox->area.active){
//            renderDrawRect(hurtBox->relativePosition, hurtBox->size, HURT_COLLIDER_COLOR, 30);
//        //}else{
//            //renderDrawRect(renderer, gameState->camera, hurt.offset, hurt.size, DEACTIVE_COLLIDER_COLOR, 30);
//        //}
//    }
//}


//void systemUpdateHitBoxPosition(Ecs* ecs){
//    PROFILER_START();
//    EntityArray entities = view(ecs, ECS_TYPE(HitBox), ECS_TYPE(TransformComponent));
//
//    //for(Entity entity : entities){
//    for(size_t i = 0; i < entities.count; i++){
//        Entity entity = entities.entities[i];
//        HitBox* hitBox= (HitBox*) getComponent(ecs, entity, HitBox);
//        TransformComponent* t= (TransformComponent*)getComponent(ecs, entity, TransformComponent);
//        Box2DCollider hit = calculateCollider(t, hitBox->offset, hitBox->size);
//        hitBox->relativePosition = glm::vec2(hit.offset.x, hit.offset.y);
//    }
//    PROFILER_END();
//}
//
//void systemUpdateHurtBoxPosition(Ecs* ecs){
//    PROFILER_START();
//    EntityArray entities = view(ecs, ECS_TYPE(HurtBox), ECS_TYPE(TransformComponent));
//
//    //for(Entity entity : entities){
//    for(size_t i = 0; i < entities.count; i++){
//        Entity entity = entities.entities[i];
//        HurtBox* hurtbox= (HurtBox*)getComponent(ecs, entity, HurtBox);
//        TransformComponent* t= (TransformComponent*)getComponent(ecs, entity, TransformComponent);
//        Box2DCollider hurt = calculateCollider(t, hurtbox->offset, hurtbox->size);
//        hurtbox->relativePosition = glm::vec2(hurt.offset.x, hurt.offset.y);
//    }
//    PROFILER_END();
//}

void systemRenderSprites(Ecs* ecs){
    EntityArray entities = view(ecs, ECS_TYPE(TransformComponent), ECS_TYPE(SpriteComponent));

    //for(Entity entity : entities){
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
    //for(Entity e : entities){
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
    //for(Entity e : entities){
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

//NOTE: forward declaration
void loadLevel(GameLevels level);

void secondLevelSystem(Ecs* ecs){
    EntityArray player = view(ecs, ECS_TYPE(PlayerTag), ECS_TYPE(Box2DCollider));
    EntityArray portal = view(ecs, ECS_TYPE(PortalTag), ECS_TYPE(Box2DCollider));

    //for(Entity entityA : player){
    for(size_t i = 0; i < player.count; i++){
        Entity entityA = player.entities[i];
        //Box2DCollider* boxAent = getComponent(ecs, entityA, Box2DCollider);
        //for(Entity entityB : portal){
        for(size_t i = 0; i < portal.count; i++){
            Entity entityB = portal.entities[i];
            //Box2DCollider* boxBent = getComponent(ecs, entityB, Box2DCollider);
            //if(beginCollision(entityA , entityB)){
            //    gameState->gameLevels = GameLevels::SECOND_LEVEL;
            //    loadLevel(GameLevels::SECOND_LEVEL);
            //    break;
            //}
        }
    }
}

void cameraFollowSystem(Ecs* ecs, OrtographicCamera* camera){
    EntityArray entities = view(ecs, ECS_TYPE(PlayerTag));
    //for(Entity entity : entities){
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
    //for(Entity entity : entities){
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
    PROFILER_END();
}

void loadLevel(GameLevels level){
    PROFILER_START();
    for(size_t entity = 0; entity < engine->ecs->entities; entity++){
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
                PickupTag pickup = {};
                pushComponent(engine->ecs, gun, PickupTag, &pickup);
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
                PickupTag pickup = {};
                pushComponent(engine->ecs, shotgun, PickupTag, &pickup);
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
                PickupTag pickup = {};
                pushComponent(engine->ecs, sniper, PickupTag, &pickup);
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
    //for(Entity e : entities){
    for(size_t i = 0; i < entities.count; i++){
        Entity e = entities.entities[i];
        HasWeaponComponent* hasWeapon = (HasWeaponComponent*)getComponent(engine->ecs, e, HasWeaponComponent);
        if(hasWeapon->weaponType[0] == WEAPON_GUN){
            GunComponent* gun = (GunComponent*)getComponent(engine->ecs, hasWeapon->weaponId[0], GunComponent);
            gun->dmg = gun->dmg + (gun->dmg * dmgMultiplier);
            hasWeapon->weaponType[0] = WEAPON_GUN;
        }else if(hasWeapon->weaponType[0] == WEAPON_SHOTGUN){
            ShotgunComponent* gun = (ShotgunComponent*)getComponent(engine->ecs, hasWeapon->weaponId[0], ShotgunComponent);
            gun->dmg = gun->dmg + (gun->dmg * dmgMultiplier);
            hasWeapon->weaponType[0] = WEAPON_SHOTGUN;
        }else if(hasWeapon->weaponType[0] == WEAPON_SNIPER){
            SniperComponent* gun = (SniperComponent*)getComponent(engine->ecs, hasWeapon->weaponId[0], SniperComponent);
            gun->dmg = gun->dmg + (gun->dmg * dmgMultiplier);
            hasWeapon->weaponType[0] = WEAPON_SNIPER;
        }
    }
}

void applySpeedUp(float speedUp){
    EntityArray player = view(engine->ecs, ECS_TYPE(PlayerTag));
    //PlayerTag* playerTag = getComponent(engine->ecs, player[0], PlayerTag);
    VelocityComponent* vel = (VelocityComponent*)getComponent(engine->ecs, player.entities[0], VelocityComponent);
    vel->vel += (vel->vel * speedUp);
}

void applyIncreaseRadius(float radius){
    //EntityArray projectiles = view(engine->ecs, ProjectileTag, HitBox);
    EntityArray weapons = view(engine->ecs, ECS_TYPE(HasWeaponComponent));
    HasWeaponComponent* hasWeapon = (HasWeaponComponent*)getComponent(engine->ecs, weapons.entities[0], HasWeaponComponent);
    if(hasComponent(engine->ecs, hasWeapon->weaponId[0], GunComponent)){
        GunComponent* gun = (GunComponent*)getComponent(engine->ecs, hasWeapon->weaponId[0], GunComponent);
        gun->radius = gun->radius + (gun->radius * radius);
    }else if(hasComponent(engine->ecs, hasWeapon->weaponId[0], ShotgunComponent)){
        ShotgunComponent* gun = (ShotgunComponent*)getComponent(engine->ecs, hasWeapon->weaponId[0], ShotgunComponent);
        gun->radius = gun->radius + (gun->radius * radius);
    }else if(hasComponent(engine->ecs, hasWeapon->weaponId[0], SniperComponent)){
        SniperComponent* gun = (SniperComponent*)getComponent(engine->ecs, hasWeapon->weaponId[0], SniperComponent);
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
            Entity weapon = weapons.entities[0];
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
        case CardChoice::CARD_NONE:{
            break;
        }
    }
}

void drawCardSelectionMenu(){
    //horizontal layout
    int xCenter = gameState->camera.width * 0.5f;
    int yCenter = gameState->camera.height * 0.5f;
    int padding = 10;
    int layoutWidth = 0;

    int buttonWidth = 100;
    int buttonHeight = 100;

    Card* choice = NULL;// = {.cardChoice = CardChoice::CARD_NONE};

    //NOTE i am modifying the card catalogue so it won't reinitialize when restart a run
    if(!gameState->cardInit){
        for(int i = 0; i < 3; i++){
            int r = i + rand() % (5 - i);
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
    //buttonX += (layoutWidth / 4);
    //if(UiButton(gameState->cards[3].description, {buttonX, yCenter - (buttonHeight * 0.5f)},{buttonWidth, buttonHeight}, {0,0})){
    //    LOGINFO("Orbiting weapon Obtained");
    //}
}

static float secondsPassed = 1;
static float minutesPassed = 0;
void drawHud(float dt){
    //beginUIFrame({0,0}, {engine->windowWidth, engine->windowHeight});
        //clearColor(0.2f, 0.3f, 0.3f, 1.0f);

    //int y = gameState->camera.height;
    //int x = 0;
    //Font* font = getFont("Minecraft");
    EntityArray player = view(engine->ecs, ECS_TYPE(PlayerTag));
    HealthComponent* h = getComponent(engine->ecs, player.entities[0], HealthComponent);
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.0f / %d HP", h->hp, 100);
    UiText(buffer, {30, 20}, 0.2f);
    float hpBar = h->hp * 96 / 100; //health conversion to rect 98 is the hp size 100 is the black bar size
    renderDrawFilledRect(convertScreenCoords({30, 50}, {100, 10}, {gameState->camera.width, gameState->camera.height}), {100, 10}, {0,0}, {0,0,0,1});
    renderDrawFilledRect(convertScreenCoords({32, 52.5}, {96, 5}, {gameState->camera.width, gameState->camera.height}), {hpBar, 5}, {0,0}, {1,0,0,1});

    ExperienceComponent* exp = (ExperienceComponent*)getComponent(engine->ecs, player.entities[0], ExperienceComponent);
    float experienceBar = exp->currentXp * 550/ exp->maxXp; 
    renderDrawFilledRect(convertScreenCoords({30, 300}, {550, 10}, {gameState->camera.width, gameState->camera.height}), {550, 10}, {0,0}, {0,0,0,1});
    renderDrawFilledRect(convertScreenCoords({32, 302.5}, {500, 5}, {gameState->camera.width, gameState->camera.height}), {experienceBar, 5}, {0,0}, {0,1,1,1});
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
    //EntityArray entities =  view(engine->ecs, EnemyTag);
    //size_t entities = engine->ecs->entities - engine->ecs->removedEntitiesCount;
    //char entitiesNumber[30];
    //snprintf(entitiesNumber, sizeof(entitiesNumber), "%u", entities);
    //UiText(entitiesNumber, {gameState->camera.width - calculateTextWidth(getFont("Minecraft") , entitiesNumber, 0.2f) - 100, 20}, 0.2f);

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
        UiText(timePassedText, {(gameState->camera.width / 2) - (calculateTextWidth(getFont("Minecraft") , timePassedText, 0.3f) / 2), 20}, 0.3f);
    }
    //UiButton("ciao ciao \nciao ciao", {0, 0},{100, 100}, {0,0});
}

//ECS_DECLARE_COMPONENT(TEST);
//struct TEST{
//    int x = 0;
//    int y = 10;
//};


GAME_API void* gameStart(EngineState* engineState){
    //Always do that right now, i need to figure out how to remove this block of code
    #ifdef _WIN32
     if (!gladLoadGL()) {
        LOGERROR("GLAD not loaded properly in DLL.");
        return NULL;
    }
    #endif
    PROFILER_START();
    engine = engineState;

    importCollisionModule(engine->ecs);
    importBaseModule(engine->ecs);

    registerComponent(engine->ecs, PlayerTag);
    registerComponent(engine->ecs, ProjectileTag);
    registerComponent(engine->ecs, LifeTime);
    registerComponent(engine->ecs, WallTag);
    //gamepadSpriteTagId = registerComponent(engine->ecs, GamepadSpriteTag);
    registerComponent(engine->ecs, PortalTag);
    registerComponent(engine->ecs, GunComponent);
    registerComponent(engine->ecs, ShotgunComponent);
    registerComponent(engine->ecs, SniperComponent);
    registerComponent(engine->ecs, OrbitingWeaponComponent);
    registerComponent(engine->ecs, OrbitingProjectile);
    registerComponent(engine->ecs, GranadeComponent);
    registerComponent(engine->ecs, ExplosionComponent);
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

    gameState = new GameState();
    gameState->gameLevels = GameLevels::MAIN_MENU;
    //engine->gameState = gameState;
    gameState->camera = createCamera({0,0,0}, 640, 320);
    loadAudio("sfx/celeste-test.ogg");
    loadFont("Minecraft");
    loadFont("Creame");
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

    //setFontUI(getFont("Creame"));

    //TileSet simple = createTileSet(getTexture(engine->textureManager, "tileset01"), 32, 32);
    //std::vector<int> tileBg = loadTilemapFromFile("assets/map/map-bg.csv", simple, 30);
    //std::vector<int> tileFg = loadTilemapFromFile("assets/map/map-fg.csv", simple, 30);


    //gameState->bgMap = createTilemap(tileBg, 30, 20, 32, simple);
    //gameState->fgMap = createTilemap(tileFg, 30, 20, 32, simple);

    //-----------------------------------------------------------------------------------
    //initCollisionManager();
    //loadLevel(GameLevels::FIRST_LEVEL);
    PROFILER_END();
    return gameState;
}

void pickupWeapon(Ecs* ecs, Entity entityA, Entity entityB){
    int padding = 20;
    InputComponent* inputComponent = (InputComponent*)getComponent(ecs, entityA, InputComponent);
    if(!inputComponent->pickUp){return;}
    if(hasComponent(ecs, entityA, PlayerTag) && hasComponent(ecs, entityB, GunComponent)){
        //pickup event
        LOGINFO("Gun");
        HasWeaponComponent* hasWeapon = (HasWeaponComponent*)getComponent(ecs, entityA, HasWeaponComponent);
        removeEntity(ecs, hasWeapon->weaponId[0]);
        Entity gun = createGun(ecs);
        hasWeapon->weaponId[0] = gun;
        hasWeapon->weaponType[0] = WEAPON_GUN;
        hasWeapon->weaponCount = 1;
        PersistentTag p = {};
        pushComponent(ecs, gun, PersistentTag, &p);
        return;
    }else if(hasComponent(ecs, entityA, PlayerTag) && hasComponent(ecs, entityB, ShotgunComponent)){
        //pickup event
        LOGINFO("Shotgun");
        HasWeaponComponent* hasWeapon = (HasWeaponComponent*)getComponent(ecs, entityA, HasWeaponComponent);
        removeEntity(ecs, hasWeapon->weaponId[0]);
        Entity gun = createShotgun(ecs);
        hasWeapon->weaponId[0] = gun;
        hasWeapon->weaponType[0] = WEAPON_SHOTGUN;
        hasWeapon->weaponCount = 1;
        PersistentTag p = {};
        pushComponent(ecs, gun, PersistentTag, &p);
        return;
    }else if(hasComponent(ecs, entityA, PlayerTag) && hasComponent(ecs, entityB, SniperComponent)){
        //pickup event
        LOGINFO("Sniper");
        HasWeaponComponent* hasWeapon = (HasWeaponComponent*)getComponent(ecs, entityA, HasWeaponComponent);
        removeEntity(ecs, hasWeapon->weaponId[0]);
        Entity gun = createSniper(ecs);
        hasWeapon->weaponId[0] = gun;
        hasWeapon->weaponType[0] = WEAPON_SNIPER;
        hasWeapon->weaponCount = 1;
        PersistentTag p = {};
        pushComponent(ecs, gun, PersistentTag, &p);
        return;
    }
    if(hasComponent(ecs, entityA, HasWeaponComponent) && (getComponent(ecs, entityA, HasWeaponComponent))->weaponId[0] != entityB){
        removeComponent(ecs, entityA, PersistentTag);
    }
}

void pickupWeaponSystem(Ecs* ecs){
    TriggerEventArray* events = getTriggerEvents();
    for(size_t i = 0; i < events->count; i++){
        CollisionEvent event = events->item[i];
        Entity entityA = event.entityA.entity;
        Entity entityB = event.entityB.entity;
        if(hasComponent(ecs, entityA, PlayerTag) && hasComponent(ecs, entityB, PickupTag)){
            pickupWeapon(ecs, entityA, entityB);
        }else if(hasComponent(ecs, entityA, PickupTag) && hasComponent(ecs, entityB, PlayerTag)){
            pickupWeapon(ecs, entityB, entityA);
        }

        if(hasComponent(ecs, entityA, PlayerTag) && hasComponent(ecs, entityB, PortalTag2)){
            gameState->gameLevels = GameLevels::THIRD_LEVEL;
            loadLevel(GameLevels::THIRD_LEVEL);

        }else if(hasComponent(ecs, entityB, PlayerTag) && hasComponent(ecs, entityA, PortalTag2)){
            gameState->gameLevels = GameLevels::THIRD_LEVEL;
            loadLevel(GameLevels::THIRD_LEVEL);
        }

        if(hasComponent(ecs, entityA, PlayerTag) && hasComponent(ecs, entityB, ExperienceDrop)){
            ExperienceComponent* playerExp = getComponent(ecs, entityA, ExperienceComponent);
            ExperienceDrop* dropExp = getComponent(ecs, entityB, ExperienceDrop);
            playerExp->currentXp += dropExp->xpDrop;
            levelUp(gameState, playerExp);
            removeEntity(ecs, entityB);
        }else if(hasComponent(ecs, entityB, PlayerTag) && hasComponent(ecs, entityA, ExperienceDrop)){
            ExperienceComponent* playerExp = getComponent(ecs, entityB, ExperienceComponent);
            ExperienceDrop* dropExp = getComponent(ecs, entityA, ExperienceDrop);
            playerExp->currentXp += dropExp->xpDrop;
            levelUp(gameState, playerExp);
            removeEntity(ecs, entityA);
        }
    }
}

//void drawWeaponDescription(Ecs* ecs, GameState* gameState){
//    //glm::vec2 canvasSize = {gameState->camera.width, gameState->camera.height};
//    //beginUiFrame({0,0}, {canvasSize.x, canvasSize.y});
//    EntityArray entities = view(ecs, ECS_TYPE(WeaponTag), ECS_TYPE(SpriteComponent));
//    EntityArray players = view(ecs, ECS_TYPE(PlayerTag));
//    int padding = 20;
//    //for(Entity e : entities){
//    for(size_t i = 0; i < entities.count; i++){
//        Entity e = entities.entities[i];
//        if(isColliding(e, players.entities[0])){
//            if(hasComponent(ecs, e, GunComponent)){
//                //TransformComponent* t = getComponent(ecs, e, TransformComponent);
//                Box2DCollider* box = (Box2DCollider*)getComponent(ecs, e, Box2DCollider);
//                glm::vec2 position = box->relativePosition;
//                position.y += box->size.y;
//                position = worldToScreen(gameState->camera, position);
//                int textHeight = UigetTextHeight("Gun", 0.2f);
//                position.y -= textHeight;
//                position.y -= padding;
//                UiText("Gun", position, 0.4f);
//            }else if(hasComponent(ecs, e, ShotgunComponent)){
//                Box2DCollider* box = (Box2DCollider*)getComponent(ecs, e, Box2DCollider);
//                glm::vec2 position = box->relativePosition;
//                position.y += box->size.y;
//                position = worldToScreen(gameState->camera, position);
//                int textHeight = UigetTextHeight("Shotgun", 0.2f);
//                position.y -= textHeight;
//                position.y -= padding;
//                UiText("Shotgun", position, 0.4f);
//            }else if(hasComponent(ecs, e, SniperComponent)){
//                Box2DCollider* box = (Box2DCollider*)getComponent(ecs, e, Box2DCollider);
//                glm::vec2 position = box->relativePosition;
//                position.y += box->size.y;
//                position = worldToScreen(gameState->camera, position);
//                int textHeight = UigetTextHeight("Sniper", 0.2f);
//                position.y -= textHeight;
//                position.y -= padding;
//                UiText("Sniper", position, 0.4f);
//            }
//        }
//    }
//    //endUiFrame();
//}

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


GAME_API void gameUpdate(EngineState* engineState, float dt){
    PROFILER_START();
    engine = (EngineState*) engineState;
    gameState = (GameState*) engine->gameState;
    if(isJustPressed(KEYS::F5)){
        gameState->debugMode = !gameState->debugMode;
    }
    clearColor(0.2f, 0.3f, 0.3f, 1.0f);
    EntityArray players = view(engine->ecs, ECS_TYPE(PlayerTag));
    Entity player = players.entities[0];

    switch (gameState->gameLevels)
    {
        case GameLevels::MAIN_MENU:{
            if(isJustPressedGamepad(GAMEPAD_BUTTON_START)){
                gameState->gameLevels = GameLevels::FIRST_LEVEL;
                loadLevel(GameLevels::FIRST_LEVEL);
            }

            handleMenuInput(engine);
            beginScene(gameState->camera, RenderMode::NO_DEPTH);
                drawMenu();
            endScene();

            //beginUiFrame({0,0}, {canvasSize.x, canvasSize.y});
            //    Texture* controllerTexture = getTexture("Xone");
            //    //renderDrawQuad2D(controllerTexture, {x + (controllerTexture->width / 2), (y / 2) - (controllerTexture->height / 2)},{1,1},{0,0}, {0,0}, {controllerTexture->width, controllerTexture->height});
            //    UiImage(controllerTexture, {xo - (controllerTexture->width / 2), (yo / 2) - (controllerTexture->height / 2)}, {0,0});
            //    uint32_t textWidth = calculateTextWidth(getFont("Minecraft"), "Press Start to play the Game!!!", 0.5f);
            //    UiText("Press Start to play the Game!!!", {xo - (textWidth / 2) , (yo / 2) + (controllerTexture->height / 2) + padding}, 0.5f);
            //endUiFrame();
            break;
        }
        case GameLevels::FIRST_LEVEL:{
            //systemUpdateTransformChildEntities(engine->ecs);
            //systemUpdateColliderPosition(engine->ecs);
            //systemUpdateHitBoxPosition(engine->ecs);
            //systemUpdateHurtBoxPosition(engine->ecs);
            systemCheckRange(engine->ecs);
            systemProjectileHit(engine->ecs);
            animateTiles(&gameState->bgMap, dt);
            animationSystem(engine->ecs, dt);
            cooldownSystem(engine->ecs, dt);
            weaponFireSystem(engine->ecs);
            inputPlayerSystem(engine->ecs, getInputState(), dt);
            moveSystem(engine->ecs, dt);
            cameraFollowSystem(engine->ecs, &gameState->camera);
            pickupWeaponSystem(engine->ecs);
            secondLevelSystem(engine->ecs);
            //thidLevelSystem(engine->ecs);
            deathSystem(engine->ecs);

            PROFILER_SCOPE_START("rendering");
            beginScene(gameState->camera, RenderMode::NORMAL);
                systemRenderSprites(engine->ecs);
                //renderDrawQuad({10,10,10},{1,1,1},{0,0,0}, getTexture("XOne"), {0,0}, {200,200}, false);
                renderTileMap(&gameState->bgMap);
                //renderTileSet(engine->renderer, gameState->bgMap.tileset, gameState->camera);
                //renderTileMap(engine->renderer, gameState->fgMap, gameState->camera, 1.0f, true);
            endScene();
            beginUiFrame({0,0}, {gameState->camera.width, gameState->camera.height});
                drawHud(dt);
                //drawWeaponDescription(engine->ecs, gameState);
            endUiFrame();
            PROFILER_SCOPE_START("rendering");

            //TODO: delte
            //It's a code snippet to convert mouse position to world position and check collision
            //{
            //    glm::vec2 mousePos = getMousePos();
            //    mousePos = screenToWorld(gameState->camera, {engine->windowWidth, engine->windowHeight}, mousePos);

            //    auto player = view(engine->ecs, PortalTag);
            //    if(player.size() <= 0) break;
            //    TransformComponent* t = getComponent(engine->ecs, player[0], TransformComponent);
            //    Box2DCollider* h = getComponent(engine->ecs, player[0], Box2DCollider);
            //    Box2DCollider b = calculateCollider(t, h->offset, h->size);
            //    //LOGINFO("box: {pos :[%f / %f], size: [%f / %f]}, mouse : %f / %f", b.offset.x, b.offset.y, b.size.x, b.size.y, mousePos.x, mousePos.y);
            //    //LOGINFO("mouse: {pos :[%f / %f]", mousePos.x, mousePos.y);
            //    if(pointRectIntersection(mousePos, b.offset, b.size)){
            //        LOGINFO("CIAO");
            //    }
            //}
            break;
        }
        case GameLevels::SECOND_LEVEL:{
            //systemUpdateTransformChildEntities(engine->ecs);
            //systemUpdateColliderPosition(engine->ecs);
            //systemUpdateHitBoxPosition(engine->ecs);
            //systemUpdateHurtBoxPosition(engine->ecs);
            systemProjectileHit(engine->ecs);
            animationSystem(engine->ecs, dt);
            cooldownSystem(engine->ecs, dt);
            weaponFireSystem(engine->ecs);
            systemCheckRange(engine->ecs);
            deathSystem(engine->ecs);
            moveSystem(engine->ecs, dt);
            inputPlayerSystem(engine->ecs, getInputState(), dt);
            lifeTimeSystem(engine->ecs, dt);

            beginScene(gameState->camera, RenderMode::NORMAL);
                systemRenderSprites(engine->ecs);
            endScene();
            break;
        }
        case GameLevels::THIRD_LEVEL:{
            if(isJustPressedGamepad(GAMEPAD_BUTTON_START)){
                gameState->gameLevels = GameLevels::SELECT_CARD;
            }

            systemProjectileHit(engine->ecs);

            pickupWeaponSystem(engine->ecs);
            systemCheckRange(engine->ecs);
            cooldownSystem(engine->ecs, dt);
            weaponFireSystem(engine->ecs);
            //automaticFireSystem(engine->ecs, dt);
            animationSystem(engine->ecs, dt);
            moveSystem(engine->ecs, dt);
            systemOrbitMovement(engine->ecs, dt);
            gatherExperienceSystem(engine->ecs, gameState);
            inputPlayerSystem(engine->ecs, getInputState(), dt);
            lifeTimeSystem(engine->ecs, dt);
            cameraFollowSystem(engine->ecs, &gameState->camera);
            systemUpdateEnemyDirection(engine->ecs);
            systemSpawnEnemies(engine->ecs, dt);
            explosionSystem(engine->ecs);
            deathSystem(engine->ecs);


            PROFILER_SCOPE_START("rendering");
            beginScene(gameState->camera, RenderMode::NORMAL);
                systemRenderSprites(engine->ecs);
                //renderDrawQuad({10,10,10},{1,1,1},{0,0,0}, getTexture("XOne"), {0,0}, {200,200}, false);
                //renderTileSet(engine->renderer, gameState->bgMap.tileset, gameState->camera);
                //renderTileMap(engine->renderer, gameState->fgMap, gameState->camera, 1.0f, true);
            endScene();
            beginUiFrame({0,0}, {gameState->camera.width, gameState->camera.height});
                drawHud(dt);
            endUiFrame();
            PROFILER_SCOPE_END();
            break;
        }
        case GameLevels::SELECT_CARD:{
            beginScene(gameState->camera, RenderMode::NORMAL);
                systemRenderSprites(engine->ecs);
            endScene();
            beginUiFrame({0,0}, {gameState->camera.width, gameState->camera.height});
                drawCardSelectionMenu();
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
            //systemRenderHitBox(engine->ecs);
            //systemRenderHurtBox(engine->ecs);
        endScene();
    }
    PROFILER_END();
}

GAME_API void gameStop(EngineState* engine, GameState* gameState){
    destroyEcs(engine->ecs);
    delete gameState;
}