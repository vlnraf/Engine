#include <malloc.h>

#include "game.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>


#define ACTIVE_COLLIDER_COLOR glm::vec4(255.0f / 255.0f, 0, 255.0f / 255.0f, 255.0f  /255.0f)
#define DEACTIVE_COLLIDER_COLOR glm::vec4(128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 255.0f / 255.0f)
#define HIT_COLLIDER_COLOR glm::vec4(0 , 255.0f / 255.0f, 0, 255.0f  /255.0f)
#define HURT_COLLIDER_COLOR glm::vec4(255.0f / 255.0f, 0, 0, 255.0f / 255.0f)
//#define TRIGGER_COLLIDER_COLOR glm::vec4(0, 255.0f /255.0f, 0, 255.0f / 255.0f)

MyProfiler prof;

// Physics engine to be detached from game
Box2DCollider calculateWorldAABB(TransformComponent* transform, Box2DCollider* box);


void systemRenderSprites(GameState* gameState, Ecs* ecs, Renderer* renderer, std::vector<ComponentType> types, float dt){
    PROFILER_START();
    //setYsort(renderer, true);
    std::vector<Entity> entities = view(ecs, types);

    for(Entity entity : entities){
        TransformComponent* t= (TransformComponent*) getComponent(ecs, entity, ECS_TRANSFORM);
        SpriteComponent* s= (SpriteComponent*) getComponent(ecs, entity, ECS_SPRITE);
        if(s->visible){
            renderDrawSprite(renderer, gameState->camera, t->position, t->scale, t->rotation, s);
        }
    }
    PROFILER_END();
}

void systemRenderColliders(GameState* gameState, Ecs* ecs, Renderer* renderer, std::vector<ComponentType> types, float dt){
    PROFILER_START();
    //setYsort(renderer, true);
    std::vector<Entity> entities = view(ecs, types);

    for(Entity entity : entities){
        Box2DCollider* box= (Box2DCollider*) getComponent(ecs, entity, ECS_2D_BOX_COLLIDER);
        TransformComponent* t= (TransformComponent*) getComponent(ecs, entity, ECS_TRANSFORM);
        //Need the position of the box which is dictated by the entity position + the box offset
        //glm::vec2 offset = {t->position.x + box->offset.x, t->position.y + box->offset.y};
        Box2DCollider b = calculateWorldAABB(t, box);
        if(box->active){
            renderDrawRect(renderer, gameState->camera, b.offset, b.size, ACTIVE_COLLIDER_COLOR, 30);
        }else{
            renderDrawRect(renderer, gameState->camera, b.offset, b.size, DEACTIVE_COLLIDER_COLOR, 30);
        }
    }
    PROFILER_END();
}

void systemRenderHitBox(GameState* gameState, Ecs* ecs, Renderer* renderer, std::vector<ComponentType> types, float dt){
    PROFILER_START();
    std::vector<Entity> entities = view(ecs, types);

    for(Entity entity : entities){
        HitBox* hitBox= (HitBox*) getComponent(ecs, entity, ECS_HITBOX);
        TransformComponent* t= (TransformComponent*) getComponent(ecs, entity, ECS_TRANSFORM);
        //Need the position of the box which is dictated by the entity position + the box offset
        //glm::vec2 offset = {t->position.x + box->offset.x, t->position.y + box->offset.y};
        Box2DCollider hit = calculateWorldAABB(t, &hitBox->area);
        renderDrawRect(renderer, gameState->camera, hit.offset, hit.size, HIT_COLLIDER_COLOR, 30);
    }
    PROFILER_END();
}

void systemRenderHurtBox(GameState* gameState, Ecs* ecs, Renderer* renderer, std::vector<ComponentType> types, float dt){
    PROFILER_START();
    std::vector<Entity> entities = view(ecs, types);

    for(Entity entity : entities){
        HurtBox* hurtBox= (HurtBox*) getComponent(ecs, entity, ECS_HURTBOX);
        TransformComponent* t= (TransformComponent*) getComponent(ecs, entity, ECS_TRANSFORM);
        //Need the position of the box which is dictated by the entity position + the box offset
        //glm::vec2 offset = {t->position.x + box->offset.x, t->position.y + box->offset.y};
        Box2DCollider hurt = calculateWorldAABB(t, &hurtBox->area);
        renderDrawRect(renderer, gameState->camera, hurt.offset, hurt.size, HURT_COLLIDER_COLOR, 30);
    }
    PROFILER_END();
}

bool isColliding(const Box2DCollider* a, const Box2DCollider* b) {
      return (a->offset.x < b->offset.x + b->size.x &&
              a->offset.x + a->size.x > b->offset.x &&
              a->offset.y < b->offset.y + b->size.y &&
              a->offset.y + a->size.y > b->offset.y);
}

void systemCheckHitBox(Ecs* ecs, const std::vector<Entity> entitiesA, const std::vector<Entity> entitiesB, const float dt){
    for(Entity entityA : entitiesA){
        HitBox* boxAent= (HitBox*) getComponent(ecs, entityA, ECS_HITBOX);
        TransformComponent* tA= (TransformComponent*) getComponent(ecs, entityA, ECS_TRANSFORM);
        //WeaponTag* weapon= (WeaponTag*) getComponent(ecs, entityA, ECS_WEAPON);
        DebugNameComponent* nameComponentA = (DebugNameComponent*) getComponent(ecs, entityA, ECS_DEBUG_NAME);
        for(Entity entityB : entitiesB){
            HurtBox* boxBent = (HurtBox*) getComponent(ecs, entityB, ECS_HURTBOX);
            TransformComponent* tB = (TransformComponent*) getComponent(ecs, entityB, ECS_TRANSFORM);
            //HealthComponent* health= (HealthComponent*) getComponent(ecs, entityB, ECS_HEALTH);
            DebugNameComponent* nameComponentB = (DebugNameComponent*) getComponent(ecs, entityB, ECS_DEBUG_NAME);
            //I need the position of the box which is dictated by the entity position + the box offset
            Box2DCollider boxA = calculateWorldAABB(tA, &boxAent->area); 
            Box2DCollider boxB = calculateWorldAABB(tB, &boxBent->area); 

            if(isColliding(&boxA, &boxB)){
                //health->value -= weapon->dmg;
                LOGINFO("Entity %s is hitting entity %s", nameComponentA, nameComponentB);
                //LOGINFO("Health: %d", health->value);
            }
        }
    }
}

void systemCheckHurtBox(Ecs* ecs, const std::vector<Entity> entitiesA, const std::vector<Entity> entitiesB, const float dt){
    for(Entity entityA : entitiesA){
        HurtBox* boxAent= (HurtBox*) getComponent(ecs, entityA, ECS_HURTBOX);
        TransformComponent* tA= (TransformComponent*) getComponent(ecs, entityA, ECS_TRANSFORM);
        //HealthComponent* health= (HealthComponent*) getComponent(ecs, entityA, ECS_HEALTH);
        DebugNameComponent* nameComponentA = (DebugNameComponent*) getComponent(ecs, entityA, ECS_DEBUG_NAME);
        for(Entity entityB : entitiesB){
            HitBox* boxBent = (HitBox*) getComponent(ecs, entityB, ECS_HITBOX);
            TransformComponent* tB = (TransformComponent*) getComponent(ecs, entityB, ECS_TRANSFORM);
            //EnemyTag* enemy= (EnemyTag*) getComponent(ecs, entityB, ECS_ENEMY_TAG);
            DebugNameComponent* nameComponentB = (DebugNameComponent*) getComponent(ecs, entityB, ECS_DEBUG_NAME);
            //I need the position of the box which is dictated by the entity position + the box offset
            Box2DCollider boxA = calculateWorldAABB(tA, &boxAent->area); 
            Box2DCollider boxB = calculateWorldAABB(tB, &boxBent->area); 

            if(isColliding(&boxA, &boxB)){
                LOGINFO("Entity %s is being hitted by entity %s", nameComponentA, nameComponentB);
                //health->value -= enemy->dmg;
                //LOGINFO("Health: %d", health->value);
            }
        }
    }
}

void resolveDynamicDynamicCollision(Ecs* ecs, const Entity entityA, const Entity entityB, const Box2DCollider* boxA, const Box2DCollider* boxB){
    TransformComponent* tA = (TransformComponent*)getComponent(ecs, entityA, ECS_TRANSFORM);
    TransformComponent* tB = (TransformComponent*)getComponent(ecs, entityB, ECS_TRANSFORM);

    VelocityComponent* velA = (VelocityComponent*)getComponent(ecs, entityA, ECS_VELOCITY);
    VelocityComponent* velB = (VelocityComponent*)getComponent(ecs, entityB, ECS_VELOCITY);

    // Calculate overlap (penetration depth)
    float overlapX = std::min(boxA->offset.x + boxA->size.x, boxB->offset.x + boxB->size.x) -
                     std::max(boxA->offset.x, boxB->offset.x);
    float overlapY = std::min(boxA->offset.y + boxA->size.y, boxB->offset.y + boxB->size.y) -
                     std::max(boxA->offset.y, boxB->offset.y);

    // Push entities apart along the axis of least penetration
    if (overlapX < overlapY) {
        // Resolve along X-axis
        float correction = overlapX / 2.0f;
        if (boxA->offset.x < boxB->offset.x) {
            tA->position.x -= correction;
            tB->position.x += correction;
        } else {
            tA->position.x += correction;
            tB->position.x -= correction;
        }
    } else {
        // Resolve along Y-axis
        float correction = overlapY / 2.0f;
        if (boxA->offset.y < boxB->offset.y) {
            tA->position.y -= correction;
            tB->position.y += correction;
        } else {
            tA->position.y += correction;
            tB->position.y -= correction;
        }
    }
}

void resolveDynamicStaticCollision(Ecs* ecs, const Entity entityA, const Entity entityB, const Box2DCollider* boxA, const Box2DCollider* boxB){
    TransformComponent* tA = (TransformComponent*)getComponent(ecs, entityA, ECS_TRANSFORM);
    TransformComponent* tB = (TransformComponent*)getComponent(ecs, entityB, ECS_TRANSFORM);

    VelocityComponent* velA = (VelocityComponent*)getComponent(ecs, entityA, ECS_VELOCITY);

    // Calculate overlap (penetration depth)
    float overlapX = std::min(boxA->offset.x + boxA->size.x, boxB->offset.x + boxB->size.x) -
                     std::max(boxA->offset.x, boxB->offset.x);
    float overlapY = std::min(boxA->offset.y + boxA->size.y, boxB->offset.y + boxB->size.y) -
                     std::max(boxA->offset.y, boxB->offset.y);

    // Push entities apart along the axis of least penetration
    if (overlapX < overlapY) {
        // Resolve along X-axis
        float correction = overlapX / 2.0f;
        if (boxA->offset.x < boxB->offset.x) {
            tA->position.x -= correction;
        } else {
            tA->position.x += correction;
        }
    } else {
        // Resolve along Y-axis
        float correction = overlapY / 2.0f;
        if (boxA->offset.y < boxB->offset.y) {
            tA->position.y -= correction;
        } else {
            tA->position.y += correction;
        }
    }
}

void systemCheckCollisionDynamicStatic(Ecs* ecs, const std::vector<Entity> entitiesA, const std::vector<Entity> entitiesB, const float dt){
    for(Entity entityA : entitiesA){
        Box2DCollider* boxAent= (Box2DCollider*) getComponent(ecs, entityA, ECS_2D_BOX_COLLIDER);
        TransformComponent* tA= (TransformComponent*) getComponent(ecs, entityA, ECS_TRANSFORM);
        VelocityComponent* velA = (VelocityComponent*) getComponent(ecs, entityA, ECS_VELOCITY);
        DirectionComponent* dirA = (DirectionComponent*) getComponent(ecs, entityA, ECS_DIRECTION);
        for(Entity entityB : entitiesB){
            Box2DCollider* boxBent = (Box2DCollider*) getComponent(ecs, entityB, ECS_2D_BOX_COLLIDER);
            TransformComponent* tB = (TransformComponent*) getComponent(ecs, entityB, ECS_TRANSFORM);
            //I need the position of the box which is dictated by the entity position + the box offset
            Box2DCollider boxA = calculateWorldAABB(tA, boxAent); 
            Box2DCollider boxB = calculateWorldAABB(tB, boxBent); 
            boxA.offset = {boxA.offset.x + (velA->vel.x * dirA->dir.x * dt), boxA.offset.y + (velA->vel.y * dirA->dir.y * dt)}; 
            //boxB.offset = {boxB.offset.x + (velB->vel.x * dirB->dir.x * dt), boxB.offset.y + (velB->vel.y * dirB->dir.y * dt)}; 
            boxB.offset = {boxB.offset.x, boxB.offset.y};
            boxA.size = {boxA.size.x, boxA.size.y}; 
            boxB.size = {boxB.size.x, boxB.size.y}; 

            if(boxAent->active && boxBent->active){
                if(isColliding(&boxA, &boxB)){
                    boxAent->onCollision = true;
                    boxBent->onCollision = true;
                    resolveDynamicStaticCollision(ecs, entityA, entityB, &boxA, &boxB);
                }else{
                    boxAent->onCollision = false;
                    boxBent->onCollision = false;
                }
            }
        }
    }
}

void systemCheckCollisionDynamicDynamic(Ecs* ecs, const std::vector<Entity> entitiesA, const std::vector<Entity> entitiesB, const float dt){
    for(Entity entityA : entitiesA){
        Box2DCollider* boxAent= (Box2DCollider*) getComponent(ecs, entityA, ECS_2D_BOX_COLLIDER);
        TransformComponent* tA= (TransformComponent*) getComponent(ecs, entityA, ECS_TRANSFORM);
        VelocityComponent* velA = (VelocityComponent*) getComponent(ecs, entityA, ECS_VELOCITY);
        DirectionComponent* dirA = (DirectionComponent*) getComponent(ecs, entityA, ECS_DIRECTION);
        for(Entity entityB : entitiesB){
            Box2DCollider* boxBent = (Box2DCollider*) getComponent(ecs, entityB, ECS_2D_BOX_COLLIDER);
            TransformComponent* tB = (TransformComponent*) getComponent(ecs, entityB, ECS_TRANSFORM);
            //I need the position of the box which is dictated by the entity position + the box offset
            Box2DCollider boxA = calculateWorldAABB(tA, boxAent); 
            Box2DCollider boxB = calculateWorldAABB(tB, boxBent); 
            //NOTE: check the collision on the next frame only for dynamic colliders
            VelocityComponent* velB = (VelocityComponent*) getComponent(ecs, entityB, ECS_VELOCITY);
            DirectionComponent* dirB = (DirectionComponent*) getComponent(ecs, entityB, ECS_DIRECTION);
            boxA.offset = {boxA.offset.x + (velA->vel.x * dirA->dir.x * dt), boxA.offset.y + (velA->vel.y * dirA->dir.y * dt)}; 
            boxB.offset = {boxB.offset.x + (velB->vel.x * dirB->dir.x * dt), boxB.offset.y + (velB->vel.y * dirB->dir.y * dt)}; 
            boxA.size = {boxA.size.x, boxA.size.y}; 
            boxB.size = {boxB.size.x, boxB.size.y}; 

            if(boxAent->active && boxBent->active){
                if(isColliding(&boxA, &boxB)){
                    boxAent->onCollision = true;
                    boxBent->onCollision = true;
                    resolveDynamicDynamicCollision(ecs, entityA, entityB, &boxA, &boxB);
                }else{
                    boxAent->onCollision = false;
                    boxBent->onCollision = false;
                }
            }
        }
    }
}

void systemCollision(GameState* gameState, Ecs* ecs, float dt){
    PROFILER_START();
    std::vector<Entity> dynamicEntities;
    std::vector<Entity> staticEntities;
    std::vector<Entity> colliderEntities = view(ecs, {ECS_2D_BOX_COLLIDER});
    for(Entity e : colliderEntities){
        Box2DCollider* collider = (Box2DCollider*) getComponent(ecs, e, {ECS_2D_BOX_COLLIDER});
        if(collider->type == Box2DCollider::STATIC){
            staticEntities.push_back(e);
        }else{
            dynamicEntities.push_back(e);
        }
    }
    systemCheckCollisionDynamicDynamic(ecs, dynamicEntities, dynamicEntities, dt);
    systemCheckCollisionDynamicStatic(ecs, dynamicEntities, staticEntities, dt);

    std::vector<Entity> hitBoxes = view(ecs, {ECS_HITBOX});
    std::vector<Entity> hurtBoxes = view(ecs, {ECS_HURTBOX});
    systemCheckHitBox(ecs, hitBoxes, hurtBoxes, dt);
    systemCheckHurtBox(ecs, hurtBoxes, hitBoxes, dt);
    PROFILER_END();
}

void animationSystem(GameState* gameState, Ecs* ecs, std::vector<ComponentType> types, float dt){
    PROFILER_START();
    std::vector<Entity> entities = view(ecs, types);

    //NOTE: It should not be a system or it runs every frame and so even if the animation
    // is not showing it's been computed
    for(Entity entity : entities){
        SpriteComponent* s= (SpriteComponent*) getComponent(ecs, entity, ECS_SPRITE);
        AnimationComponent* component = (AnimationComponent*) getComponent(ecs, entity, ECS_ANIMATION);
        Animation* animation = getAnimation(&gameState->animationManager, component->id.c_str());
        component->frames = animation->frames;
        component->loop = animation->loop;
        component->frameCount += dt;

        if(component->loop){
            if(component->frameCount >= animation->frameDuration){
                component->currentFrame = (component->currentFrame + 1) % (animation->frames); // module to loop around
                component->frameCount = 0;
            }
        }else{
            if(component->frameCount >= animation->frameDuration){
                component->currentFrame = component->currentFrame + 1; // module to loop around
                component->frameCount = 0;
            }
        }
        s->index = animation->indices[component->currentFrame];
    }
    PROFILER_END();
}

void moveSystem(Ecs* ecs, std::vector<ComponentType> types, float dt){
    PROFILER_START();
    std::vector<Entity> entities = view(ecs, types);
    for(Entity entity : entities){
        TransformComponent* transform = (TransformComponent*) getComponent(ecs, entity, ECS_TRANSFORM);
        VelocityComponent* vel = (VelocityComponent*) getComponent(ecs, entity, ECS_VELOCITY);
        DirectionComponent* dir = (DirectionComponent*) getComponent(ecs, entity, ECS_DIRECTION);
        transform->position.x += dir->dir.x * vel->vel.x * dt;
        transform->position.y += dir->dir.y * vel->vel.y * dt;
    }
    PROFILER_END();
}

void systemUpdateAttachedEntity(Ecs* ecs, std::vector<ComponentType> types){
    PROFILER_START();
    std::vector<Entity> entities = view(ecs, types);
    for(Entity entity : entities){
        AttachedEntity* attach = (AttachedEntity*) getComponent(ecs, entity, ECS_ATTACHED_ENTITY);
        TransformComponent* transform = (TransformComponent*) getComponent(ecs, entity, ECS_TRANSFORM);
        TransformComponent* transformAttach = (TransformComponent*) getComponent(ecs, attach->entity, ECS_TRANSFORM);
        if(!transform || ! transformAttach){
            PROFILER_END();
            return;
        }
        transform->position.x = transformAttach->position.x + attach->offset.x;
        transform->position.y = transformAttach->position.y + attach->offset.y;
    }
    PROFILER_END();
}

void inputSystemWeapon(GameState* gameState, Ecs* ecs, Input* input, std::vector<ComponentType> types, float dt){
    PROFILER_START();
    std::vector<Entity> entities = view(ecs, types);
    //TODO: create a parametrized animation???
    const float speed = 300.0f;
    static float currentFrame = 0;
    float animationDuration = 0.30;
    static bool startAnimation = false;

    float speedDash = 300.0f;
    for(Entity entity : entities){
        //HitBox* hitBox = (HitBox*) getComponent(ecs, entity, ECS_HITBOX);
        SpriteComponent* sprite = (SpriteComponent*) getComponent(ecs, entity, ECS_SPRITE);
        TransformComponent* transform = (TransformComponent*) getComponent(ecs, entity, ECS_TRANSFORM);
        AttachedEntity* attach = (AttachedEntity*) getComponent(ecs, entity, ECS_ATTACHED_ENTITY);

        //TODO: aggiungere bottoni gamepad
        if((input->keys[KEYS::Space] || input->gamepad.buttons[GAMEPAD_BUTTON_X]) && (currentFrame < animationDuration) && !startAnimation){
            startAnimation = true;
        }
        if(input->gamepad.buttons[GAMEPAD_BUTTON_A]){
            VelocityComponent* velT = (VelocityComponent*) getComponent(ecs, attach->entity, ECS_VELOCITY);
            velT->vel = {speedDash, speedDash};
        }
        //if(hitBox->hit){
        //    LOGINFO("Weapon hit");
        //}
        if(startAnimation){
            currentFrame += dt;
            //box->active = true;
            sprite->visible = true;
            transform->rotation.z -= dt * speed;
            transform->scale.x = 1.0f;
        }
        if(currentFrame > animationDuration){
            //box->active = false;
            transform->rotation.z = 0;
            sprite->visible = false;
            currentFrame = 0;
            startAnimation = false;
        }
    }

    PROFILER_END();
}

void inputSystem(GameState* gameState, Ecs* ecs, Input* input, std::vector<ComponentType> types, const float dt){
    PROFILER_START();
    static float frameCount = 1;
    float cooldown = 1;

    float speed = 100.0f;
    std::vector<Entity> entities = view(ecs, types);
    for(Entity entity : entities){
        DirectionComponent* direction = (DirectionComponent*) getComponent(ecs, entity, ECS_DIRECTION);
        VelocityComponent* velocity = (VelocityComponent*) getComponent(ecs, entity, ECS_VELOCITY);
        AnimationComponent* data = (AnimationComponent*) getComponent(ecs, entity, ECS_ANIMATION);
        SpriteComponent* sprite = (SpriteComponent*) getComponent(ecs, entity, ECS_SPRITE);
        Box2DCollider* box = (Box2DCollider*) getComponent(ecs, entity, ECS_2D_BOX_COLLIDER);
        //HurtBox* hurtBox = (HurtBox*) getComponent(ecs, entity, ECS_HURTBOX);
        HealthComponent* health = (HealthComponent*) getComponent(ecs, entity, ECS_HEALTH);
        direction->dir = {0,0};
        {   //GamePad
            //Animation* anim = getAnimation(&gameState->animationManager, data->id.c_str());//->animations.at("idleRight");
            //data->frameDuration = 1.0f / anim->frames;
            data->id = "idleLeft";
            if(input->gamepad.trigger[GAMEPAD_AXIS_LEFT_TRIGGER]){LOGINFO("Trigger Sinistro");}
            if(input->gamepad.trigger[GAMEPAD_AXIS_RIGHT_TRIGGER]){LOGINFO("Trigger Destro");}
            if(fabs(input->gamepad.leftX) > 0.1 || fabs(input->gamepad.leftY) > 0.1){ //threshold because it's never 0.0
                //NOTE: Input entity can have no animation component, error prone
                //TODO: just implement the change of animation logic elsewhere
                //I think the input system is not the system where do this logic
                //And implement direction logic instead of threshold
                velocity->vel.x = speed * fabs(input->gamepad.leftX); //NOTE: aboslute value because then will be multiplied with the direction
                velocity->vel.y = speed * fabs(input->gamepad.leftY); //NOTE: aboslute value because then will be multiplied with the direction
                //LOGINFO("%f / %f", velocity->vel.x, velocity->vel.y);
                if(input->gamepad.leftX < -0.3){
                    direction->dir = {input->gamepad.leftX, input->gamepad.leftY};
                    sprite->flipX = true;
                    data->id = "walkLeft";
                }else if(input->gamepad.leftX > 0.3){
                    direction->dir = {input->gamepad.leftX, input->gamepad.leftY};
                    sprite->flipX = false;
                    data->id = "walkRight";
                }else if(input->gamepad.leftY > 0.3){
                    direction->dir = {input->gamepad.leftX, input->gamepad.leftY};
                    data->id = "walkTop";
                }else if(input->gamepad.leftY < -0.3){
                    direction->dir = {input->gamepad.leftX, input->gamepad.leftY};
                    data->id = "walkBottom";
                }
            }
            //setComponent(ecs, entity, data, ECS_ANIMATION);
        }
        if(input->keys[KEYS::W]){ 
            direction->dir.y = 1;
            velocity->vel.y = speed;
            data->id = "walkTop";
        }
        if(input->keys[KEYS::S]){
            direction->dir.y = -1;
            velocity->vel.y = speed;
            data->id = "walkBottom";
        }
        if(input->keys[KEYS::A]){ 
            direction->dir.x = -1;
            velocity->vel.x = speed;
            sprite->flipX = true;
            data->id = "walkLeft";
        }
        if(input->keys[KEYS::D]){ 
            direction->dir.x = 1;
            velocity->vel.x = speed;
            sprite->flipX = false;
            data->id = "walkRight";
        }

        //if(box->onCollision){
            //frameCount += dt;
            //if(frameCount >= cooldown){
            //    health->value -= 10;
            //    LOGINFO("Player %d health: %d", entity, health->value);
            //    frameCount = 0;
            //}
        //NOTE: resolve collision easy way right now
        //tA->position.x = tA->position.x - (velA->vel.x * dirA->dir.x * dt);
        if(health->value <= 0){
            LOGINFO("Game Over");
            //removeEntity(ecs, entityA);
            //exit(0);
            //NOTE: create scene manager to restart the scene??
            //gameStart()
        }
        //}

        //NOTE: should i normalize the direction???
        if (direction->dir.x != 0 || direction->dir.y != 0) {
            direction->dir = glm::normalize(direction->dir);
        }
    }
    PROFILER_END();
}

void cameraFollowSystem(Ecs* ecs, OrtographicCamera* camera, Entity id){
    PROFILER_START();
    TransformComponent* t = (TransformComponent*) getComponent(ecs, id, ECS_TRANSFORM);
    if(!t){
        PROFILER_END();
        return;
    }

    followTarget(camera, t->position);
    PROFILER_END();
}

void enemyFollowPlayerSystem(Ecs* ecs, Entity player, std::vector<ComponentType> types, float dt){
    PROFILER_START();
    std::vector<Entity> entities = view(ecs, types);
    TransformComponent* transformP = (TransformComponent*) getComponent(ecs, player, ECS_TRANSFORM);
    Box2DCollider* boxP = (Box2DCollider*) getComponent(ecs, player, ECS_2D_BOX_COLLIDER);
    Box2DCollider boxPlayer = calculateWorldAABB(transformP, boxP);

    float attackRadius = 20.0f;

    //NOTE: If the entity is not in the map enymore do nothing
    if(!transformP){
        PROFILER_END();
        return;
    }
    for(Entity entity : entities){
        TransformComponent* t = (TransformComponent*) getComponent(ecs, entity, ECS_TRANSFORM);
        DirectionComponent* dir = (DirectionComponent*) getComponent(ecs, entity, ECS_DIRECTION);
        AnimationComponent* anim = (AnimationComponent*) getComponent(ecs, entity, ECS_ANIMATION);
        Box2DCollider* box = (Box2DCollider*) getComponent(ecs, entity, ECS_2D_BOX_COLLIDER);
        Box2DCollider boxEnemy = calculateWorldAABB(t, box);
        //HurtBox* hurtBox = (HurtBox*) getComponent(ecs, entity, ECS_HURTBOX);

        anim->id = "monsterWalk";
        dir->dir.x = (boxPlayer.offset.x + boxPlayer.size.x) - (boxEnemy.offset.x);
        dir->dir.y = (boxPlayer.offset.y) - boxEnemy.offset.y;
        if(fabs(dir->dir.x) < attackRadius){
            anim->id = "monsterAttack";
        }
        //if(hurtBox->hit){
        //    LOGINFO("Monster hitted");
        //}
        //if(box->hitted == true){
        //    anim->id = "monsterHit";
        //    if(anim->currentFrame >= anim->frames){
        //        anim->id = "monsterWalk";
        //        box->hitted = false;
        //    }
        //}
        //normalize a {0,0} vector is mathematically impossible it gives infinite you can't perform 0/n
        if(fabs(dir->dir.x) > 0 && fabs(dir->dir.y) > 0){
            dir->dir = glm::normalize(dir->dir);
        }else{
            dir->dir = {0,0};
        }
    }
    PROFILER_END();
}


GAME_API GameState* gameStart(Renderer* renderer){
    if (!gladLoadGL()) {
        LOGERROR("GLAD not loaded properly in DLL.");
        return nullptr;
    }
    PROFILER_SAVE("profiler.json");
    LOGINFO("Game Start");

    GameState* gameState = new GameState();
    PROFILER_START();
    gameState->ecs = initEcs();
    //TODO: make a resource manager
    //I think this also slow down the boot-up, so we can load textures with another thread
    Texture* demonSprite = loadTexture("assets/demon.png");
    Texture* white = getWhiteTexture();
    Texture* tileSet = loadTexture("assets/sprites/tileset01.png");
    Texture* idleWalk = loadTexture("assets/idle-walk.png");
    Texture* treeSprite = loadTexture("assets/sprites/tree.png");
    Texture* weaponSprite = loadTexture("assets/sprites/wood.png");

    TileSet simple = createTileSet(tileSet, 32);

    std::vector<int> tileBg = loadTilemapFromFile("assets/map/map-bg.csv", simple, 30);
    std::vector<int> tileFg = loadTilemapFromFile("assets/map/map-fg.csv", simple, 30);

    gameState->bgMap = createTilemap(tileBg, 30, 20, 32, simple);
    gameState->fgMap = createTilemap(tileFg, 30, 20, 32, simple);

    TransformComponent transform = {};
    transform.position = glm ::vec3(10.0f, 10.0f, 0.0f);
    transform.scale = glm ::vec3(1.0f, 1.0f , 0.0f);
    transform.rotation = glm ::vec3(0.0f, 0.0f, 0.0f);

    SpriteComponent sprite = {};
    sprite.texture = white;

    InputComponent inputC = {};

    VelocityComponent velocity = {.vel = {0, 0}};

    DirectionComponent direction = {.dir = {1,0}};


    gameState->camera = createCamera(glm::vec3(0.0f, 0.0f, 0.0f), 640, 320);

    transform.position = glm ::vec3(200.0f, 200.0f, 0.0f);
    transform.scale = glm ::vec3(1.0f, 1.0f, 0.0f);
    transform.rotation = glm ::vec3(0.0f, 0.0f, 0.0f);
    Entity player = createEntity(gameState->ecs, "player", ECS_TRANSFORM, &transform, sizeof(TransformComponent));
    sprite.texture = idleWalk;
    sprite.index = {0,0};
    sprite.size = {16, 16};
    sprite.layer = 1.0f;
    sprite.ySort = true;
    Box2DCollider collider = {.type = Box2DCollider::DYNAMIC, .offset = {0, 0}, .size = {16, 5}};

    gameState->animationManager = initAnimationManager();

    {   //Animatioin registry
        registryAnimation(&gameState->animationManager, "idleRight", 4, (uint16_t[]){0,1,2,3}, 0, true);
        registryAnimation(&gameState->animationManager, "idleLeft", 4, (uint16_t[]){0,1,2,3}, 0, true);
        registryAnimation(&gameState->animationManager, "idleBottom", 4, (uint16_t[]){0,1,2,3}, 1, true);
        registryAnimation(&gameState->animationManager, "idleTop", 4, (uint16_t[]){0,1,2,3}, 2, true);
        registryAnimation(&gameState->animationManager, "walkRight", 8, (uint16_t[]){0,1,2,3}, 3, true);
        registryAnimation(&gameState->animationManager, "walkLeft", 8, (uint16_t[]){0,1,2,3}, 3, true);
        registryAnimation(&gameState->animationManager, "walkBottom", 8, (uint16_t[]){0,1,2,3}, 4, true);
        registryAnimation(&gameState->animationManager, "walkTop", 8, (uint16_t[]){0,1,2,3}, 5, true);
    }

    AnimationComponent anim = {};
    anim.id = "idleRight";

    PlayerTag playerTag = {};
    HealthComponent health = {.value = 100};
    HurtBox hurtBox = {.area = {.offset = {4, 0}, .size = {10, 16}}};

    pushComponent(gameState->ecs, player, ECS_SPRITE, &sprite, sizeof(SpriteComponent));
    pushComponent(gameState->ecs, player, ECS_DIRECTION, &direction, sizeof(DirectionComponent));
    pushComponent(gameState->ecs, player, ECS_2D_BOX_COLLIDER, &collider, sizeof(Box2DCollider));
    pushComponent(gameState->ecs, player, ECS_HURTBOX, &hurtBox, sizeof(HurtBox));
    pushComponent(gameState->ecs, player, ECS_INPUT, &inputC, sizeof(InputComponent));
    pushComponent(gameState->ecs, player, ECS_VELOCITY, &velocity, sizeof(VelocityComponent));
    pushComponent(gameState->ecs, player, ECS_ANIMATION, &anim, sizeof(AnimationComponent));
    pushComponent(gameState->ecs, player, ECS_PLAYER_TAG, &playerTag, sizeof(PlayerTag));
    pushComponent(gameState->ecs, player, ECS_HEALTH, &health, sizeof(HealthComponent));
    gameState->player = player;


    AttachedEntity attached = {.entity = player, .offset ={5, 0}};
    WeaponTag weaponTag = {};
    transform.scale = glm ::vec3(0.5f, 1.0f, 0.0f);
    transform.rotation = glm ::vec3(0.0f, 0.0f, 0.0f);
    sprite.texture = weaponSprite;
    sprite.pivot = SpriteComponent::PIVOT_BOT_LEFT;
    sprite.index = {0,0};
    sprite.size = {15, 48};
    sprite.layer = 1.0f;
    sprite.ySort = true;
    sprite.visible = false;
    //collider = {.active = false, .offset = {30, -20}, .size = {20, 50}};//.size = {sprite.size.x * transform.scale.x, sprite.size.y * transform.scale.y}};
    HitBox hitBox = {.area = {.offset = {30, -20}, .size = {20, 50}}};
    Entity weapon = createEntity(gameState->ecs, "weapon", ECS_TRANSFORM, &transform, sizeof(TransformComponent));
    pushComponent(gameState->ecs, weapon, ECS_SPRITE, &sprite, sizeof(SpriteComponent));
    //pushComponent(gameState->ecs, weapon, ECS_2D_BOX_COLLIDER, &collider, sizeof(Box2DCollider));
    pushComponent(gameState->ecs, weapon, ECS_HITBOX, &hitBox, sizeof(HitBox));
    pushComponent(gameState->ecs, weapon, ECS_ATTACHED_ENTITY, &attached, sizeof(AttachedEntity));
    pushComponent(gameState->ecs, weapon, ECS_INPUT, &attached, sizeof(InputComponent));
    pushComponent(gameState->ecs, weapon, ECS_WEAPON, &weaponTag, sizeof(WeaponTag));

    transform.position = glm ::vec3(200.0f, 200.0f, 0.0f);
    transform.scale = glm ::vec3(1.0f, 1.0f , 0.0f);
    transform.rotation = glm ::vec3(0.0f, 0.0f, 0.0f);
    Entity tree = createEntity(gameState->ecs, "tree", ECS_TRANSFORM, &transform, sizeof(TransformComponent));
    collider = {.type = Box2DCollider::STATIC, .offset = {20, 0}, .size = {30, 10}};
    sprite.pivot = SpriteComponent::PIVOT_CENTER;
    sprite.texture = treeSprite;
    sprite.index = {0,0};
    sprite.size = {treeSprite->width, treeSprite->height};
    sprite.layer = 1.0f;
    sprite.ySort = true;
    sprite.visible = true;
    pushComponent(gameState->ecs, tree, ECS_SPRITE, &sprite, sizeof(SpriteComponent));
    pushComponent(gameState->ecs, tree, ECS_2D_BOX_COLLIDER, &collider, sizeof(Box2DCollider));

    {   //Animatioin Monster
        registryAnimation(&gameState->animationManager, "monsterIdle", 6, 0, true);
        registryAnimation(&gameState->animationManager, "monsterWalk", 12, 1, true);
        registryAnimation(&gameState->animationManager, "monsterAttack", 15, 2, true);
        registryAnimation(&gameState->animationManager, "monsterHit", 5, 3, false);
        registryAnimation(&gameState->animationManager, "monsterDeath", 22, 4, false);
    }
    srand(time(NULL));

    for(int i = 0; i < 30; i++){
        transform.position = glm::vec3(rand() % 800 + 32, rand() % 800 + 32, 0.0f);
        //transform.scale = glm ::vec3(0.05f, 0.05f , 0.0f);
        transform.scale = glm ::vec3(1.0f, 1.0f, 1.0f);
        transform.rotation = glm ::vec3(0.0f, 0.0f, 0.0f);
        Entity enemy = createEntity(gameState->ecs, "enemy", ECS_TRANSFORM, (void*)&transform, sizeof(TransformComponent));
        sprite.texture = demonSprite;
        sprite.index = {0,0};
        sprite.size = {288, 160};
        sprite.offset = {0, 0};
        sprite.ySort = true;
        sprite.visible = true;
        sprite.layer = 1.0f;
        sprite.pivot = SpriteComponent::PIVOT_CENTER;
        Box2DCollider collider = {.type = Box2DCollider::DYNAMIC, .offset = {110, 0}, .size = {60, 20}};
        HurtBox hurtBox = {.area = {.offset = {110, 30}, .size = {60, 40}}};
        HitBox hitBox = {.area = {.offset = {50, 0}, .size = {60, 40}}};
        velocity.vel = {15.0f, 15.0f};
        DirectionComponent direction = {.dir = {0,0}};
        EnemyTag enemyTag = {.dmg = 10.0f};
        pushComponent(gameState->ecs, enemy, ECS_SPRITE, &sprite, sizeof(SpriteComponent));
        pushComponent(gameState->ecs, enemy, ECS_DIRECTION, &direction, sizeof(DirectionComponent));
        pushComponent(gameState->ecs, enemy, ECS_VELOCITY, &velocity, sizeof(VelocityComponent));
        pushComponent(gameState->ecs, enemy, ECS_ENEMY_TAG, &enemyTag, sizeof(EnemyTag));
        pushComponent(gameState->ecs, enemy, ECS_2D_BOX_COLLIDER, &collider, sizeof(Box2DCollider));
        pushComponent(gameState->ecs, enemy, ECS_HURTBOX, &hurtBox, sizeof(HurtBox));
        pushComponent(gameState->ecs, enemy, ECS_HITBOX, &hitBox, sizeof(HitBox));
        pushComponent(gameState->ecs, enemy, ECS_ANIMATION, &anim, sizeof(AnimationComponent));
        pushComponent(gameState->ecs, enemy, ECS_HEALTH, &health, sizeof(HealthComponent));
    }
    //removeEntity(gameState->ecs, player);
    PROFILER_END();
    return gameState;
}

GAME_API void gameUpdate(GameState* gameState, Input* input, float dt){
    PROFILER_START();
    //-------------------Physics----------------
    //systemPlayerEnemyCollision(gameState, gameState->ecs, dt);
    systemCollision(gameState, gameState->ecs, dt);
    //------------------------------------------

    inputSystem(gameState, gameState->ecs, input, {ECS_SPRITE, ECS_VELOCITY, ECS_INPUT, ECS_DIRECTION}, dt);
    inputSystemWeapon(gameState, gameState->ecs, input, {ECS_HITBOX, ECS_INPUT, ECS_ATTACHED_ENTITY}, dt);
    enemyFollowPlayerSystem(gameState->ecs, gameState->player, {ECS_TRANSFORM, ECS_DIRECTION, ECS_ENEMY_TAG}, dt);
    moveSystem(gameState->ecs, {ECS_TRANSFORM, ECS_VELOCITY}, dt);
    cameraFollowSystem(gameState->ecs, &gameState->camera, gameState->player);
    animationSystem(gameState, gameState->ecs, {ECS_SPRITE, ECS_ANIMATION}, dt);
    systemUpdateAttachedEntity(gameState->ecs, {ECS_TRANSFORM, ECS_SPRITE, ECS_ATTACHED_ENTITY});
    PROFILER_END();
}


GAME_API void gameRender(GameState* gameState, Renderer* renderer, float dt){
    if (!gladLoadGL()) {
        LOGERROR("GLAD not loaded properly in DLL.");
        return;
    }
    PROFILER_START();
    renderTileMap(renderer, gameState->bgMap, gameState->camera, 0.0f, false);
    systemRenderSprites(gameState, gameState->ecs, renderer, {ECS_TRANSFORM, ECS_SPRITE}, dt);
    renderTileMap(renderer, gameState->fgMap, gameState->camera, 2.0f, false);
    systemRenderColliders(gameState, gameState->ecs, renderer, {ECS_2D_BOX_COLLIDER}, dt);
    systemRenderHitBox(gameState, gameState->ecs, renderer, {ECS_HITBOX}, dt);
    systemRenderHurtBox(gameState, gameState->ecs, renderer, {ECS_HURTBOX}, dt);
    //TODO: do attached component that store the id of the entity to which is attached and calculate position relative to it
    PROFILER_END();
}

GAME_API void gameStop(){
    PROFILER_CLEANUP();
}

Box2DCollider calculateWorldAABB(TransformComponent* transform, Box2DCollider* box){
    Box2DCollider newBox;
    newBox.offset.x = transform->position.x + box->offset.x;
    newBox.offset.y = transform->position.y + box->offset.y;
    newBox.size = box->size;
    return newBox;
}