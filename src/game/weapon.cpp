#include "weapon.hpp"
#include "projectile.hpp"
#include "lifetime.hpp"

#include "components.hpp"

ECS_DECLARE_COMPONENT(GunComponent)
ECS_DECLARE_COMPONENT(OrbitingWeaponComponent)
ECS_DECLARE_COMPONENT(OrbitingProjectile)
ECS_DECLARE_COMPONENT(GranadeComponent)
ECS_DECLARE_COMPONENT(ExplosionComponent)
ECS_DECLARE_COMPONENT(ExplosionTag)
ECS_DECLARE_COMPONENT(HasWeaponComponent)
ECS_DECLARE_COMPONENT(CooldownComponent)

Entity createGun(Ecs* ecs){
    Entity weapon = createEntity(ecs);
    GunComponent gun;// = {.dmg = 1, .attackSpeed = 0.2, .radius = 5, .piercing = false};
    gun.automatic = true;
    pushComponent(ecs, weapon, GunComponent, &gun);
    CooldownComponent cooldown = {.timeRemaining = 0.5};
    pushComponent(ecs, weapon, CooldownComponent, &cooldown);
    WeaponTag weaponTag = {};
    pushComponent(ecs, weapon, WeaponTag, &weaponTag);
    DamageComponent dmg = {};
    dmg.dmg = 1;
    pushComponent(ecs, weapon, DamageComponent, &dmg);
    return weapon;
}

Entity createGranade(Ecs* ecs){
    Entity weapon = createEntity(ecs);
    GranadeComponent granade;
    pushComponent(ecs, weapon, GranadeComponent, &granade);
    CooldownComponent cooldown = {.timeRemaining = granade.attackSpeed};
    pushComponent(ecs, weapon, CooldownComponent, &cooldown);
    WeaponTag weaponTag = {};
    pushComponent(ecs, weapon, WeaponTag, &weaponTag);
    return weapon;
}

Entity createOrbitWeapon(Ecs* ecs){
    EntityArray players = view(ecs, ECS_TYPE(PlayerTag));
    HasWeaponComponent* hasWeapon = (HasWeaponComponent*)getComponent(ecs, players.entities[0], HasWeaponComponent);
    Entity orbit = createEntity(ecs);
    hasWeapon->weaponId[hasWeapon->weaponCount] = orbit;
    hasWeapon->weaponType[hasWeapon->weaponCount] = WeaponType::WEAPON_ORBIT;
    hasWeapon->weaponCount++;
    OrbitingWeaponComponent orbitComponent = {};
    orbitComponent.target = players.entities[0];
    pushComponent(ecs, orbit, OrbitingWeaponComponent, &orbitComponent);
    return orbit;
}

void addOrbitProjectile(Ecs* ecs, Entity weaponId){
    EntityArray players = view(ecs, ECS_TYPE(PlayerTag));
    Entity orbit = createEntity(ecs);
    TransformComponent* t = (TransformComponent*)getComponent(ecs, players.entities[0], TransformComponent);
    t->scale = {1,1,1};
    t->rotation = {0,0,0};
    pushComponent(ecs, orbit, TransformComponent, t);

    SpriteComponent sprite = {
        .texture = getTextureByName("default"),
        .size = {16, 16},
        .color = {1,0,0,0.5},
        .ySort = false,
        .layer = 1.0f,
        .visible = true
    };
    pushComponent(ecs, orbit, SpriteComponent, &sprite);
    OrbitingProjectile projectile = {};
    OrbitingWeaponComponent* weapon = (OrbitingWeaponComponent*)getComponent(ecs, weaponId, OrbitingWeaponComponent);
    projectile.slotIndex = weapon->slotCount;
    weapon->slotCount++;
    pushComponent(ecs, orbit, OrbitingProjectile, &projectile);
    DamageComponent dmg = {};
    dmg.dmg = 1;
    pushComponent(ecs, orbit, DamageComponent, &dmg);

    Entity hitbox = createEntity(ecs);
    Box2DCollider hitboxCollider = {.type = Box2DCollider::DYNAMIC, .offset = {1,0}, .size {16,16}, .isTrigger = true};
    pushComponent(ecs, hitbox, Box2DCollider, &hitboxCollider);
    pushComponent(ecs, hitbox, TransformComponent, &t);
    Parent parent = {.entity = orbit};
    pushComponent(ecs, hitbox, Parent, &parent);
    HitboxTag hitboxTag = {};
    pushComponent(ecs, hitbox, HitboxTag, &hitboxTag);
}

void automaticFire(Ecs* ecs, Entity weaponId, Entity entityId, const glm::vec3 spawnPosition){
    GunComponent* gun = (GunComponent*)getComponent(ecs, weaponId, GunComponent);
    DamageComponent* dmg = getComponent(ecs, weaponId, DamageComponent);
    if(!gun) return;
    TransformComponent* transform = getComponent(ecs, entityId, TransformComponent); //player transform
    //Entity e = getNearestEntity(ecs, entityId, 3);
    EntityColliderArray* result = getNearestEntities(ecs, entityId, gun->range);
    Entity e = NULL_ENTITY;
    float nearest = gun->range;
    for(size_t i = 0; i < result->count; i++){
        Entity entity = result->item[i].entity;
        if(!hasComponent(ecs, entity, EnemyTag)) continue;
        TransformComponent* enemyTransform = getComponent(ecs, entity, TransformComponent);
        if(glm::length(transform->position - enemyTransform->position) < nearest){
            e = entity;
            nearest = glm::length(transform->position - enemyTransform->position);
        }
    }
    if(e == NULL_ENTITY) return;
    TransformComponent* t = getComponent(ecs, e, TransformComponent);
    if(!t) return;
    glm::vec3 dir = t->position -  spawnPosition; // - t->position;
    glm::vec2 direction = {dir.x, dir.y};
    direction = glm::normalize(direction);
    createProjectile(ecs, spawnPosition, direction, dmg->dmg, gun->range, gun->radius, gun->piercing);
    //playAudio("sfx/gunshot.wav", 0.1);
}

void fireGranade(Ecs* ecs, Entity weaponId, glm::vec3 targetPosition, const glm::vec2 dir){
    GranadeComponent* granade = (GranadeComponent*)getComponent(ecs, weaponId, GranadeComponent);
    if(!granade) return;
    CooldownComponent* cooldown = (CooldownComponent*)getComponent(ecs, weaponId, CooldownComponent);
    if(!cooldown) return;
    if(cooldown->timeRemaining > 0.0f) return;
    Entity projectile = createEntity(ecs);

    SpriteComponent sprite = {
        .texture = getTextureByName("granade"),
        .size = {16, 16},
        .ySort = false,
        .layer = 2.0f,
        .visible = true
    };
    TransformComponent transform = {    
        .position = {targetPosition.x - (sprite.size.x / 2), targetPosition.y + 300 - (sprite.size.y / 2), targetPosition.z},
        .scale = {1.0f, 1.0f, 0.0f},
        .rotation = {0.0f, 0.0f, 0.0f}
    };

    VelocityComponent velocity = {.vel = {100, 100}};
    DirectionComponent direction = {.dir = dir};
    glm::vec3 target = targetPosition;
    ExplosionComponent explosion = {.targetPosition = target};

    pushComponent(ecs, projectile, TransformComponent, &transform);
    pushComponent(ecs, projectile, SpriteComponent, &sprite);
    pushComponent(ecs, projectile, VelocityComponent, &velocity);
    pushComponent(ecs, projectile, DirectionComponent, &direction);
    pushComponent(ecs, projectile, ExplosionComponent, &explosion);
    pushComponent(ecs, projectile, GranadeComponent, granade);
}

void explosionSystem(Ecs* ecs){
    EntityArray entities = view(ecs, ECS_TYPE(ExplosionComponent));
    for(size_t i = 0; i < entities.count; i++){
        Entity e = entities.entities[i];
        ExplosionComponent* explosionComponent = (ExplosionComponent*)getComponent(ecs, e, ExplosionComponent);
        TransformComponent* t = (TransformComponent*)getComponent(ecs, e, TransformComponent);
        if(t->position.y <= explosionComponent->targetPosition.y){
            Entity explosion = createEntity(ecs);
            LifeTime life = {.endTime = 1};
            pushComponent(ecs, explosion, LifeTime, &life);
            SpriteComponent sprite = {
                .texture = getTextureByName("default"),
                .size = {32, 32},
                .color = {1, 0, 0, 0.3},
                .ySort = true,
                .layer = 1.0f,
                .visible = true
            };
            TransformComponent transform = {    
                .position = {explosionComponent->targetPosition.x , explosionComponent->targetPosition.y , explosionComponent->targetPosition.z},
                .scale = {1.0f, 1.0f, 0.0f},
                .rotation = {0.0f, 0.0f, 0.0f}
            };
            pushComponent(ecs, explosion, SpriteComponent, &sprite);
            pushComponent(ecs, explosion, TransformComponent, &transform);
            ExplosionTag explosionTag = {};
            pushComponent(ecs, explosion, ExplosionTag, &explosionTag);
            DamageComponent dmg = {};
            dmg.dmg = 5;
            pushComponent(ecs, explosion, DamageComponent, &dmg);
            Entity hitbox = createEntity(ecs);
            Box2DCollider hit = {.size = {32,32}, .isTrigger = true};
            pushComponent(ecs, hitbox, Box2DCollider, &hit);
            Parent p = {.entity = explosion};
            pushComponent(ecs, hitbox, Parent, &p);
            pushComponent(ecs, hitbox, TransformComponent, &transform);
            HitboxTag tag = {};
            pushComponent(ecs, hitbox, HitboxTag, &tag);
            removeEntity(ecs, e);
        }
    }
}

void weaponFireSystem(Ecs* ecs, float dt){
    PROFILER_START();
    EntityArray entities = view(ecs, ECS_TYPE(HasWeaponComponent));
    //for(Entity e : entities){
    static uint16_t projectileSpawned = 0;
    for(size_t i = 0; i < entities.count; i++){
        Entity e = entities.entities[i];
        HasWeaponComponent* hasWeapon = (HasWeaponComponent*)getComponent(ecs, e, HasWeaponComponent);
        TransformComponent* t = (TransformComponent*)getComponent(ecs, e, TransformComponent);
        if(hasWeapon->weaponCount <= 0){
            continue;
        }
        CooldownComponent* cooldown = (CooldownComponent*)getComponent(ecs, hasWeapon->weaponId[0], CooldownComponent);
        if(!cooldown) continue;
        if(cooldown->timeRemaining > 0.0f) continue;

        if(hasComponent(ecs, hasWeapon->weaponId[0], GunComponent) ){
            GunComponent* gun = (GunComponent*)getComponent(ecs, hasWeapon->weaponId[0], GunComponent);
            gun->delayPassed += dt;
            if(gun->automatic && gun->delayPassed > gun->delay){
                automaticFire(ecs, hasWeapon->weaponId[0], e, t->position);
                projectileSpawned++;
                gun->delayPassed = 0;
            }
            if(projectileSpawned >= gun->numProjectiles){
                projectileSpawned = 0;
                cooldown->timeRemaining = gun->attackSpeed;
            }
        }
        for(size_t weapon = 0; weapon < hasWeapon->weaponCount; weapon++){
            CooldownComponent* cooldown = (CooldownComponent*)getComponent(ecs, hasWeapon->weaponId[weapon], CooldownComponent);
            if(!cooldown) continue;
            if(cooldown->timeRemaining > 0.0f) continue;
            if(hasComponent(ecs, hasWeapon->weaponId[weapon], GranadeComponent)){
                int randomX = (rand() % 150) * ((rand() % 2) * 2 - 1);
                int randomY = (rand() % 150) * ((rand() % 2) * 2 - 1);// + 100;
                glm::vec3 target = {t->position.x + randomX, t->position.y + randomY, t->position.z};
                fireGranade(ecs, hasWeapon->weaponId[weapon], target, {0, -1});
                GranadeComponent* granade = (GranadeComponent*)getComponent(ecs, hasWeapon->weaponId[weapon], GranadeComponent);
                cooldown->timeRemaining = granade->attackSpeed;
                //LOGINFO("granade launched");
            }
        }
    }
    PROFILER_END();
}

void cooldownSystem(Ecs* ecs, float dt){
    PROFILER_START();
    EntityArray entities = view(ecs, ECS_TYPE(CooldownComponent));
    for(size_t i = 0; i < entities.count; i++){
        Entity e = entities.entities[i];
        CooldownComponent* cooldown = (CooldownComponent*)getComponent(ecs, e, CooldownComponent);
        cooldown->timeRemaining -= dt;
    }
    PROFILER_END();
}