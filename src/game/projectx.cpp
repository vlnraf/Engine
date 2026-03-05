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
ECS_DECLARE_COMPONENT(ActiveEnemyTag);
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

    OrtographicCamera cam = gameState->mainCamera;
    float camLeft   = cam.position.x - (cam.width  / 2);
    float camRight  = cam.position.x + (cam.width  / 2);
    float camBottom = cam.position.y - (cam.height / 2);
    float camTop    = cam.position.y + (cam.height / 2);

    for(size_t i = 0; i < entities.count; i++){
        Entity entity = entities.entities[i];
        // Skip distant enemies that are definitely off-screen
        if(hasComponent(ecs, entity, EnemyTag) && !hasComponent(ecs, entity, ActiveEnemyTag)) continue;
        TransformComponent* t= (TransformComponent*) getComponent(ecs, entity, TransformComponent);
        SpriteComponent* s= (SpriteComponent*) getComponent(ecs, entity, SpriteComponent);
        if(s->visible){
            // TODO: move this check in the renderer to cull everything that is not on screen
            // when we are in the world position, not in screen position
            if( (t->position.x <= camLeft  || t->position.x >= camRight) ||
                (t->position.y <= camBottom || t->position.y >= camTop)) continue;
            // Calculate final size from sprite size * transform scale
            glm::vec2 size = s->size * glm::vec2(t->scale.x, t->scale.y);

            // Calculate position for rendering (center of sprite)
            glm::vec3 position = t->position;
            position.z = s->layer;

            // Use sourceRect if set, otherwise default to full texture
            Rect sourceRect = s->sourceRect;
            if(s->sourceRect.size.x == 0 || s->sourceRect.size.y == 0) {
                sourceRect = {.pos = {0, 0}, .size = {(float)s->texture->width, (float)s->texture->height}};
            }

            // Handle UV flipping for flipX/flipY
            if(s->flipX) {
                sourceRect.pos.x += sourceRect.size.x;
                sourceRect.size.x = -sourceRect.size.x;
            }
            if(s->flipY) {
                sourceRect.pos.y += sourceRect.size.y;
                sourceRect.size.y = -sourceRect.size.y;
            }

            // Call renderDrawQuadPro directly
            renderDrawQuadPro(
                position,
                size,
                t->rotation,
                sourceRect,
                {0.5f,0.5f},
                s->texture,
                s->color,
                s->ySort,
                s->ySortOffset  // Pass y-sort offset for depth sorting
            );
        }
    }
}

void flipSpriteSystem(Ecs* ecs){
    EntityArray enemiesVisible = view(ecs, ECS_TYPE(DirectionComponent), ECS_TYPE(SpriteComponent), ECS_TYPE(ActiveEnemyTag));
    for(size_t i = 0; i < enemiesVisible.count; i++){
        Entity e = enemiesVisible.entities[i];
        DirectionComponent* direction  = getComponent(ecs, e, DirectionComponent);
        SpriteComponent* sprite        =  getComponent(ecs, e, SpriteComponent);
        if(direction->dir.x > 0){
            sprite->flipX = false;
        }else if(direction->dir.x < 0){
            sprite->flipX = true;
        }
    }
    
}

void moveSystem(Ecs* ecs, float dt){
    static float distantAccumulatedDt = 0.0f;
    static uint32_t moveFrameCount = 0;
    moveFrameCount++;
    distantAccumulatedDt += dt;
    bool moveDistant = (moveFrameCount % DISTANT_UPDATE_INTERVAL) == 0;

    // Active enemies (near player) — full movement every frame
    EntityArray activeEnemies = view(ecs, ECS_TYPE(TransformComponent), ECS_TYPE(VelocityComponent), ECS_TYPE(DirectionComponent), ECS_TYPE(ActiveEnemyTag));
    for(size_t i = 0; i < activeEnemies.count; i++){
        Entity e = activeEnemies.entities[i];
        TransformComponent* transform = (TransformComponent*) getComponent(ecs, e, TransformComponent);
        VelocityComponent* velocity  = (VelocityComponent*)  getComponent(ecs, e, VelocityComponent);
        DirectionComponent* direction  = (DirectionComponent*)  getComponent(ecs, e, DirectionComponent);
        transform->position += glm::vec3(direction->dir.x * velocity->vel.x * dt, direction->dir.y * velocity->vel.y * dt, 0.0f);
    }
    // Distant enemies — move every N frames using accumulated dt so speed is correct
    if(moveDistant){
        EntityArray allEnemies = view(ecs, ECS_TYPE(TransformComponent), ECS_TYPE(VelocityComponent), ECS_TYPE(DirectionComponent), ECS_TYPE(EnemyTag));
        for(size_t i = 0; i < allEnemies.count; i++){
            Entity e = allEnemies.entities[i];
            if(hasComponent(ecs, e, ActiveEnemyTag)) continue; // already moved above
            TransformComponent* transform = (TransformComponent*) getComponent(ecs, e, TransformComponent);
            VelocityComponent* velocity  = (VelocityComponent*)  getComponent(ecs, e, VelocityComponent);
            DirectionComponent* direction  = (DirectionComponent*)  getComponent(ecs, e, DirectionComponent);
            transform->position += glm::vec3(direction->dir.x * velocity->vel.x * distantAccumulatedDt, direction->dir.y * velocity->vel.y * distantAccumulatedDt, 0.0f);
        }
        distantAccumulatedDt = 0.0f;
    }
    // Non-enemy entities (player, projectiles, XP drops) — always move
    EntityArray others = view(ecs, ECS_TYPE(TransformComponent), ECS_TYPE(VelocityComponent), ECS_TYPE(DirectionComponent));
    for(size_t i = 0; i < others.count; i++){
        Entity e = others.entities[i];
        if(hasComponent(ecs, e, EnemyTag)) continue;
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
            if(health->hp <= 0){
                gameState->gameLevels = GameLevels::GAME_OVER;
                break;
            }
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
        if(hasComponent(ecs, entity, EnemyTag) && !hasComponent(ecs, entity, ActiveEnemyTag)) continue;
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
        // Convert grid index to pixel coordinates
        s->sourceRect = gridToPixelRect(anim->indices[animComp->currentFrame], anim->tileSize);
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
    gameState->restart = false;
    switch(level){
        case GameLevels::MAIN_MENU:{
            gameState->gameLevels = GameLevels::MAIN_MENU;
            break;
        }
        case GameLevels::FIRST_LEVEL:{
            gameState->bgMap = LoadTilesetFromTiled("test", engine->ecs);
            Entity player = createPlayer(engine->ecs, gameState->mainCamera);
            {
                //Vampire survival clone teleport
                //TODO: remove from final game
                TransformComponent transform = {    
                    .position = {500.0f, 50.0f, 0.0f},
                    .scale = {1.0f, 1.0f, 0.0f},
                    .rotation = {0.0f, 0.0f, 0.0f}
                };
                SpriteComponent sprite = {
                    .texture = getTextureByName("dungeon"),
                    .size = {32,32},
                    .sourceRect = {.pos = {15 * 16, 8 * 16}, .size ={32, 32}},
                    .ySort = true,
                    .layer = 1.0f,
                    .visible = true
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
        orbit->angle += 2 * dt;
        for(size_t j = 0; j < projectiles.count; j++){
            Entity projectile = projectiles.entities[j];
            OrbitingProjectile* orbitProjectile = (OrbitingProjectile*)getComponent(ecs, projectile, OrbitingProjectile);
            TransformComponent* transform = (TransformComponent*)getComponent(ecs, projectile, TransformComponent);
            float slotAngle = (2.0f * 3.14 / projectiles.count) * orbitProjectile->slotIndex;
            float finalAngle = orbit->angle + slotAngle;
            glm::vec3 offset = {cos(finalAngle) * 50,
                                sin(finalAngle) * 50,
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
            float dmgMultiplier = choice->dmg;
            if(choice->level >= choice->maxLevel){
                choice->pickable = false;
            }
            choice->level++;
            applyDmgUp(dmgMultiplier);
            break;
        }
        case CardChoice::CARD_SPEED_UP:{
            float speedMultiplier = choice->speed;
            if(choice->level >= choice->maxLevel){
                choice->pickable = false;
            }
            choice->level++;
            applySpeedUp(speedMultiplier);
            break;
        }
        case CardChoice::CARD_SIZE_UP:{
            float radius = choice->radius;
            if(choice->level >= choice->maxLevel){
                choice->pickable = false;
            }
            choice->level++;
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
            if(choice->level >= choice->maxLevel){
                choice->pickable = false;
            }
            choice->level++;
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
            //LOGINFO("Granade added");
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

    //Selection logic with keyboard
    static int focus = 0;
    int selected = 0;
    glm::vec4 focusColor = {1,1,0,1};
    glm::vec4 color = {0,0,0,1.0};
    glm::vec4 activeColor = color;
    if(isJustPressed(KEYS::A)){
        focus -= 1;
        if(focus < 0) focus += 3;
    }else if(isJustPressed(KEYS::D)){
        focus = (focus + 1) % 3;
    }else if(isJustPressed(KEYS::Enter)){
        selected = focus;
        choice = &gameState->cards[selected];
        gameState->cardInit = false;
        gameState->gameLevels = GameLevels::THIRD_LEVEL;
    }


    uint32_t xo = 0;
    uint32_t yo = 0;
    uint32_t padding = 10;
    // calculate 3 buttons
    uint32_t layoutWidth = (getScreenSize().x * 2/3);
    uint32_t layoutHeight = (getScreenSize().y / 2);
    uint32_t layoutPosX = (getScreenSize().x / 2) - (layoutWidth / 2);
    uint32_t layoutPosY = (getScreenSize().y / 3);

    uint32_t buttonWidth = (layoutWidth / 3) - padding; //3 cards in half screen
    uint32_t buttonHeight = layoutHeight;

    uint32_t xpos = xo + (layoutPosX);
    uint32_t ypos = yo + (layoutPosY);

    Font* font = getFont("Roboto-Regular");
    float fontScale = 1.0f;

    if(focus == 0) activeColor = focusColor;
    uint32_t buttonBorder = padding;
    renderDrawFilledRect({xpos, ypos}, {buttonWidth, buttonHeight}, 0, activeColor);
    renderDrawFilledRect({xpos + buttonBorder/2, ypos + buttonBorder/2}, {buttonWidth - buttonBorder, buttonHeight - buttonBorder}, 0, {0,0,0,1});
    uint32_t fontHeight = calculateTextHeight(font, gameState->cards[0].description, fontScale);
    glm::vec2 fontPos = {xpos + buttonBorder/2, ypos + buttonHeight - fontHeight};
    renderDrawText2D(font, gameState->cards[0].description, fontPos, fontScale);
    activeColor = color;
    //if(UiButton(gameState->cards[0].description, {xpos, ypos},{buttonWidth, buttonHeight}, {0,0})){
    //    choice = &gameState->cards[0];
    //    gameState->cardInit = false;
    //    gameState->gameLevels = GameLevels::THIRD_LEVEL;
    //}
    //buttonX += (layoutWidth / 3);
    if(focus == 1) activeColor = focusColor;
    xpos = xpos + buttonWidth + padding;
    renderDrawFilledRect({xpos, ypos}, {buttonWidth, buttonHeight}, 0, activeColor);
    renderDrawFilledRect({xpos + buttonBorder/2, ypos + buttonBorder/2}, {buttonWidth - buttonBorder, buttonHeight - buttonBorder}, 0, {0,0,0,1});
    fontHeight = calculateTextHeight(font, gameState->cards[1].description, fontScale);
    fontPos = {xpos + buttonBorder/2, ypos + buttonHeight - fontHeight};
    renderDrawText2D(font, gameState->cards[1].description, fontPos, fontScale);
    activeColor = color;
    //if(UiButton(gameState->cards[1].description, {xpos, ypos},{buttonWidth, buttonHeight}, {0,0})){
    //    choice = &gameState->cards[1];
    //    gameState->cardInit = false;
    //    gameState->gameLevels = GameLevels::THIRD_LEVEL;
    //}
    //buttonX += (layoutWidth / 3);
    if(focus == 2) activeColor = focusColor;
    xpos = xpos + buttonWidth + padding;
    renderDrawFilledRect({xpos, ypos}, {buttonWidth, buttonHeight}, 0, activeColor);
    renderDrawFilledRect({xpos + buttonBorder/2, ypos + buttonBorder/2}, {buttonWidth - buttonBorder, buttonHeight - buttonBorder}, 0, {0,0,0,1});
    fontHeight = calculateTextHeight(font, gameState->cards[2].description, fontScale);
    fontPos = {xpos + buttonBorder/2, ypos + buttonHeight - fontHeight};
    renderDrawText2D(font, gameState->cards[2].description, fontPos, fontScale);
    activeColor = color;
    //if(UiButton(gameState->cards[2].description, {xpos, ypos},{buttonWidth, buttonHeight}, {0,0})){
    //    choice = &gameState->cards[2];
    //    gameState->cardInit = false;
    //    gameState->gameLevels = GameLevels::THIRD_LEVEL;
    //}

    applyCard(choice);
}

static float secondsPassed = 1;
static float minutesPassed = 0;
void drawHud(float dt){
    uint32_t xo = 0;
    uint32_t yo = getScreenSize().y; //padding 20 pixel from top
    Font* font = getFont("Roboto-Regular");
    float fontScale = 1.0f;
    EntityArray player = view(engine->ecs, ECS_TYPE(PlayerTag));
    HealthComponent* hp = getComponent(engine->ecs, player.entities[0], HealthComponent);
    float hpMax = 100.0f; //TODO: bake into the component

    uint32_t padding = 10;
    uint32_t xpos = xo + padding;
    uint32_t ypos = yo - padding;
    float hpBarWidth = 300;
    float hpBarHeight =  20;
    xpos = xo + padding;
    ypos = yo - hpBarHeight - padding;
    renderDrawFilledRectPro({xpos, ypos}, {hpBarWidth, hpBarHeight}, 0, {0,0}, {0,0,0,1});

    uint32_t xOff = 10;
    uint32_t yOff = 5;
    float hpFill = (hpBarWidth - xOff*2) * (hp->hp / hpMax);
    renderDrawFilledRectPro({xpos + xOff, ypos + yOff}, {hpFill, hpBarHeight - yOff*2}, 0, {0,0}, {1,0,0,1});

    fontScale = 0.6f;

    TempArena tmp = getTempArena(gameState->arena);
    String8 hpText = pushString8F(tmp.arena, "%.0f / %.0f HP", hp->hp, hpMax);
    int textHeight = calculateTextHeight(font, hpText.str, 1);
    int textWidth = calculateTextWidth(font, hpText.str, fontScale);
    xpos = xpos + (hpBarWidth / 2) - (textWidth / 2);
    renderDrawText2D(font, hpText.str, {xpos, ypos}, fontScale);


    ExperienceComponent* exp = (ExperienceComponent*)getComponent(engine->ecs, player.entities[0], ExperienceComponent);
    xpos = 0 + 10;
    ypos = 0 + 10; 
    xOff = 10;
    yOff = 5;
    float expBarWidth = getScreenSize().x - 20;
    float expBarHeight = 20;
    //float expBar = exp->currentXp * 550/ exp->maxXp; 
    float expBarFill = (expBarWidth - xOff * 2) * (exp->currentXp / exp->maxXp);
    renderDrawFilledRectPro({xpos, ypos}, {expBarWidth, expBarHeight}, 0, {0,0}, {0,0,0,1});
    renderDrawFilledRectPro({xpos + xOff, ypos + yOff}, {expBarFill, expBarHeight - yOff*2}, 0, {0,0}, {0,1,1,1});

    //char fpsText[30];
    //String8 fps;
    //static float timer = 0;
    //static float ffps = 0;
    //timer += dt;
    //if(timer >= 1.0f){ //each second
        //ffps = engine->fps;
        //timer = 0;
    //}
    String8 fps = pushString8F(tmp.arena, "FPS: %.0f", getFPS());
    xpos = getScreenSize().x - calculateTextWidth(font, fps.str, fontScale) - 10;
    ypos = yo - hpBarHeight - padding;
    renderDrawText2D(font, fps.str, {xpos, ypos}, fontScale);
    String8 numEntity = pushString8F(tmp.arena, "#Entities: %u", engine->ecs->entitiesCount);
    textHeight = calculateTextHeight(font, numEntity.str, fontScale);
    xpos = getScreenSize().x - calculateTextWidth(font, numEntity.str, fontScale) - 10;
    ypos = ypos - textHeight - padding;
    renderDrawText2D(font, numEntity.str, {xpos, ypos}, fontScale);

    if(gameState->gameLevels == GameLevels::THIRD_LEVEL){
        secondsPassed += dt;
        if(secondsPassed > 60){
            secondsPassed = 0;
        }
        if(secondsPassed == 0){
            minutesPassed++;
        }
        String8 time = pushString8F(tmp.arena, "Survived Time : %02.0f: %02.0f", minutesPassed, secondsPassed);
        textHeight = calculateTextHeight(font, time.str, fontScale);
        textWidth = calculateTextWidth(font, time.str, fontScale);
        xpos = (getScreenSize().x / 2) - (textWidth / 2);
        ypos = getScreenSize().y - textHeight;
        renderDrawText2D(font, time.str, {xpos, ypos}, fontScale);
    }
    releaseTempArena(tmp);
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
    registerComponent(engine->ecs, ActiveEnemyTag);
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
    //gameState->mainCamera = createCamera({0,0,0}, 640, 320);
    gameState = arenaAllocStruct(gameArena, GameState);
    gameState->arena = gameArena;
    // Resolution-independent camera: shows 640 world units horizontally, 320 vertically, centered at origin
    // Bounds: -320 to +320 horizontally, -160 to +160 vertically
    gameState->restart = false;
    gameState->mainCamera = createCamera(-640.0f / 2, 640.0f / 2, -320.0f / 2, 320.0f / 2);
    setActiveCamera(&gameState->mainCamera);
    //engineState->gameState = gameState;
    //engineState->gameState = arenaAllocStruct(&engine->arena, GameState);
    gameState->cards[0] = {.description = "Damage up", .dmg = 1.5f, .speed = 0, .cardChoice = CardChoice::CARD_DMG_UP};
    gameState->cards[0].pickable = true;
    gameState->cards[0].level = 0;
    gameState->cards[0].maxLevel = UINT32_MAX;
    gameState->cards[1] = {.description = "Speed up", .dmg = 0.0f, .speed = 0.05f, .cardChoice = CardChoice::CARD_SPEED_UP};
    gameState->cards[1].pickable = true;
    gameState->cards[1].level = 0;
    gameState->cards[1].maxLevel = UINT32_MAX;
    gameState->cards[2] = {.description = "Size up", .dmg = 0.0f, .speed = 0.0f, .radius = 0.1f, .cardChoice = CardChoice::CARD_SIZE_UP};
    gameState->cards[2].pickable = true;
    gameState->cards[2].level = 0;
    gameState->cards[2].maxLevel = 10;
    gameState->cards[3] = {.description = "+1 proje\nctiles", .cardChoice = CardChoice::CARD_ADD_PROJECTILE};
    gameState->cards[3].pickable = true;
    gameState->cards[3].level = 0;
    gameState->cards[3].maxLevel = UINT32_MAX;
    gameState->cards[4] = {.description = "Orbit", .dmg = 0.0f, .speed = 0.0f, .radius = 0.2f, .cardChoice = CardChoice::CARD_ORBIT};
    gameState->cards[4].pickable = true;
    gameState->cards[4].level = 0;
    gameState->cards[4].maxLevel = 5;
    gameState->cards[5] = {.description = "Granade", .cardChoice = CardChoice::CARD_GRANADE};
    gameState->cards[5].pickable = true;
    gameState->cards[5].level = 0;
    gameState->cards[5].maxLevel = 1;

    //gameState->gameLevels = GameLevels::MAIN_MENU;
    //engine->gameState = gameState;
    loadFont("Creame");
    loadFont("Roboto-Regular");
    loadTexture("background");
    gameState->backGround = getTextureByName("background");
    loadTexture("Golem-hurt");
    loadTexture("Slime_Green");
    loadTexture("slime_move");
    loadTexture("idle-walk");
    loadTexture("XOne");
    loadTexture("tileset01");
    loadTexture("dungeon");
    loadTexture("monster-1");
    loadTexture("hp_and_mp");
    loadTexture("weaponSprites");
    loadTexture("gobu walk");
    loadTexture("granade");
    gameState->renderTexture = loadRenderTexture(640, 640);
    gameState->shader = loadShader(gameArena, "shaders/custom-shader.vs", "shaders/custom-shader.fs");
    gameState->gridShader = loadShader(gameArena, "shaders/grid-shader.vs", "shaders/grid-shader.fs");
    gameState->defaultFont = getFont("Roboto-Regular");
    //playAudio("sfx/gaming-music.wav", 0.1f); //background sound

    //gameState->mainCamera = createCamera({0,0,0}, 1920, 1080);

    //gameState->mainCamera = engine->mainCamera;
    //return engineState->gameState;
}

GAME_API void gameRender(Arena* gameArena, EngineState* engine, float dt){}

static int scale = 2;
GAME_API void gameUpdate(Arena* gameArena, EngineState* engineState, float dt){
    PROFILER_START();
    engine = (EngineState*) engineState;
    gameState = (GameState*)gameArena->memory;
    //dt *= 10;

    if(isJustPressed(KEYS::T)){
        applicationRequestQuit();
    }
    if(isJustPressed(KEYS::L)){
        applicationSetResolution(1920, 1080);
    }
    float width = 640;
    float height = 320;
    if(isJustPressed(KEYS::I)){
        scale++;
        setProjection(&gameState->mainCamera, -width / scale, width / scale, -height / scale, height / scale);
    }
    if(isJustPressed(KEYS::J)){
        if(!(scale <= 1)){
            scale--;
        }
        setProjection(&gameState->mainCamera, -width / scale, width / scale, -height / scale, height / scale);
    }

    cameraFollowSystem(engine->ecs, &gameState->mainCamera);
    //beginTextureMode(&gameState->renderTexture);
    //clearColor(0,1,1,1);
    //    //beginScene();
    //    beginMode2D(gameState->mainCamera);
    //    //beginUiFrame({0,0}, {gameState->mainCamera.width, gameState->mainCamera.height});
    //        //drawMenu();
    //        renderDrawFilledRect({0, 0}, {200, 200}, 0, {1,0,0,1});
    //        systemRenderSprites(engine->ecs);
    //    //endUiFrame();
    //    endMode2D();
    //    //endScene();
    //endTextureMode();
    clearColor(0.2f, 0.3f, 0.3f, 1.0f);
    //NOTE: can be cached in the gameState
    EntityArray players = view(engine->ecs, ECS_TYPE(PlayerTag));
    Entity player = players.entities[0];
    TransformComponent* playerT = getComponent(engine->ecs, player, TransformComponent);
    if(playerT){
        setGridCenter(playerT->position.x, playerT->position.y);
    }

    // Accumulate time for shader animations
    gameState->shaderTime += dt;

    //beginScene();
    //    beginShaderMode(&gameState->shader);
    //        setUniform(&gameState->shader, "dt", gameState->shaderTime);  // Use accumulated time
    //        renderDrawQuad2D({50, 640 + 50}, {gameState->renderTexture.texture.width, -gameState->renderTexture.texture.height}, 0, &gameState->renderTexture.texture);
    //        //renderDrawRect({0, 0}, {200,100}, {1,0,0,1}, 6);
    //        //renderDrawText2D(getFont("Roboto-Regular"), "CIAO", {100,100}, 5);
    //    endShaderMode();
    //endScene();


    switch (gameState->gameLevels)
    {
        case GameLevels::MAIN_MENU:{
            if(isJustPressedGamepad(GAMEPAD_BUTTON_START)){
                loadLevel(GameLevels::FIRST_LEVEL);
                gameState->gameLevels = GameLevels::FIRST_LEVEL;
            }

            handleMenuInput();
            //beginUiFrame({0,0}, {gameState->mainCamera.width, gameState->mainCamera.height});
            beginScene(RenderMode::NO_DEPTH);
                drawMenu();
                //beginMode2D(gameState->mainCamera);
                //    renderDrawText2D(getFont("Roboto-Regular"), "CIAO CIAO", {50,50}, 0.5f);
                //    systemRenderSprites(engine->ecs);
                //endMode2D();
            endScene();
            //endUiFrame();
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
            flipSpriteSystem(engine->ecs);
            cameraFollowSystem(engine->ecs, &gameState->mainCamera);
            nextLevelSystem(engine->ecs);
            deathSystem(engine->ecs);

            PROFILER_SCOPE_START("rendering");
            beginScene(RenderMode::NORMAL);
                beginMode2D(gameState->mainCamera);
                    renderTileMap(&gameState->bgMap);
                    systemRenderSprites(engine->ecs);
                endMode2D();
            endScene();

            beginScene(RenderMode::NO_DEPTH);
                drawHud(dt);
            endScene();

            //beginUiFrame({0,0}, {gameState->mainCamera.width, gameState->mainCamera.height});
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
            flipSpriteSystem(engine->ecs);
            systemOrbitMovement(engine->ecs, dt);
            gatherExperienceSystem(engine->ecs, gameState);
            inputPlayerSystem(engine->ecs, getInputState(), dt);
            lifeTimeSystem(engine->ecs, dt);
            cameraFollowSystem(engine->ecs, &gameState->mainCamera);
            systemUpdateEnemyDirection(engine->ecs);
            systemSpawnEnemies(engine->ecs, dt);
            explosionSystem(engine->ecs);
            deathSystem(engine->ecs);


            beginScene(RenderMode::NORMAL);
                beginMode2D(gameState->mainCamera);
                    beginShaderMode(&gameState->gridShader);
                        setUniform(&gameState->gridShader, "cellSize", 32.0f);
                        renderDrawFilledRectPro({playerT->position.x, playerT->position.y}, {4000, 4000}, 0, {0.5f, 0.5f}, {1,1,1,1});
                    endShaderMode();
                    systemRenderSprites(engine->ecs);
                endMode2D();
            endScene();

            beginScene(RenderMode::NO_DEPTH);
                drawHud(dt);
            endScene();
            break;
        }
        case GameLevels::SELECT_CARD:{
            beginScene(RenderMode::NORMAL);
                beginMode2D(gameState->mainCamera);
                    beginShaderMode(&gameState->gridShader);
                        setUniform(&gameState->gridShader, "cellSize", 32.0f);
                        renderDrawFilledRectPro({playerT->position.x, playerT->position.y}, {4000, 4000}, 0, {0.5f, 0.5f}, {1,1,1,1});
                    endShaderMode();
                    systemRenderSprites(engine->ecs);
                endMode2D();

            endScene();
            beginScene(RenderMode::NO_DEPTH);
                drawCardSelectionMenu();
            endScene();
            //beginUiFrame({0,0}, {getScreenSize()});
            //    drawCardSelectionMenu();
            //endUiFrame();
            break;
        }
        case GameLevels::GAME_OVER:{
            beginScene(RenderMode::NORMAL);
                int fontHeight = calculateTextHeight(gameState->defaultFont, "GAME OVER!", 1);
                int fontWidth = calculateTextWidth(gameState->defaultFont, "GAME OVER!", 1);
                float xpos = getScreenSize().x / 2 - (fontWidth / 2);
                float ypos = getScreenSize().y / 2 - (fontHeight / 2);
                clearColor(1.0f, 0.3f, 0.3f, 1.0f);
                renderDrawText2D(gameState->defaultFont, "GAME OVER!", {xpos, ypos}, 1);
                fontHeight = calculateTextHeight(gameState->defaultFont, "Press R to restart", 0.5);
                int fontWidth2 = calculateTextWidth(gameState->defaultFont, "Press R to restart", 0.5);
                xpos += (fontWidth / 2) - (fontWidth2 / 2);
                ypos -= (fontHeight / 2) + 20;
                renderDrawText2D(gameState->defaultFont, "Press R to restart", {xpos, ypos}, 0.5);
                if(isJustPressed(KEYS::R)){
                    secondsPassed = 0;
                    minutesPassed = 0;
                    //gameState->restart = true;
                    clearEcs(engine->ecs);
                    loadLevel(GameLevels::MAIN_MENU);
                }
            endScene();
            break;
        }
        case GameLevels::WIN:{
            beginScene(RenderMode::NORMAL);
                int fontHeight = calculateTextHeight(gameState->defaultFont, "YOU WIN!", 1);
                int fontWidth = calculateTextWidth(gameState->defaultFont, "YOU WIN!", 1);
                float xpos = getScreenSize().x / 2 - (fontWidth / 2);
                float ypos = getScreenSize().y / 2 - (fontHeight / 2);
                clearColor(0.0f, 0.7f, 0.3f, 1.0f);
                renderDrawText2D(gameState->defaultFont, "YOU WIN!", {xpos, ypos}, 1);
                fontHeight = calculateTextHeight(gameState->defaultFont, "Press R to restart", 0.5);
                int fontWidth2 = calculateTextWidth(gameState->defaultFont, "Press R to restart", 0.5);
                xpos += (fontWidth / 2) - (fontWidth2 / 2);
                ypos -= (fontHeight / 2) + 20;
                renderDrawText2D(gameState->defaultFont, "Press R to restart", {xpos, ypos}, 0.5);
                if(isJustPressed(KEYS::R)){
                    secondsPassed = 0;
                    minutesPassed = 0;
                    clearEcs(engine->ecs);
                    loadLevel(GameLevels::MAIN_MENU);
                }
            endScene();
            break;
        }
    }
    PROFILER_END();
}

GAME_API void gameStop(Arena* gameArena, EngineState* engine){
    //destroyEcs(engine->ecs);
}