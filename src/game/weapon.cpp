#include "weapon.hpp"
#include "projectile.hpp"
#include "components.hpp"
#include "lifetime.hpp"
#include "componentIds.hpp"

Entity createGun(Ecs* ecs){
    Entity weapon = createEntity(ecs);
    GunComponent gun;// = {.dmg = 1, .attackSpeed = 0.2, .radius = 5, .piercing = false};
    pushComponent(ecs, weapon, gunComponentId, &gun);
    CooldownComponent cooldown = {.timeRemaining = 0.5};
    pushComponent(ecs, weapon, cooldownComponentId, &cooldown);
    WeaponTag weaponTag = {};
    pushComponent(ecs, weapon, weaponTagId, &weaponTag);
    return weapon;
}

Entity createShotgun(Ecs* ecs){
    Entity weapon = createEntity(ecs);
    ShotgunComponent gun;// = {.dmg = 1, .attackSpeed = 0.4, .radius = 5, .piercing = false};
    pushComponent(ecs, weapon, shotgunComponentId, &gun);
    CooldownComponent cooldown = {.timeRemaining = 1.0};
    pushComponent(ecs, weapon, cooldownComponentId, &cooldown);
    WeaponTag weaponTag = {};
    pushComponent(ecs, weapon, weaponTagId, &weaponTag);
    return weapon;
}

Entity createSniper(Ecs* ecs){
    Entity weapon = createEntity(ecs);
    SniperComponent gun;// = {.dmg = 1, .attackSpeed = 0.6, .radius = 5, .piercing = true};
    pushComponent(ecs, weapon, sniperComponentId, &gun);
    CooldownComponent cooldown = {.timeRemaining = 1.5};
    pushComponent(ecs, weapon, cooldownComponentId, &cooldown);
    WeaponTag weaponTag = {};
    pushComponent(ecs, weapon, weaponTagId, &weaponTag);
    return weapon;
}

Entity createGranade(Ecs* ecs){
    Entity weapon = createEntity(ecs);
    GranadeComponent granade;
    pushComponent(ecs, weapon, granadeComponentId, &granade);
    CooldownComponent cooldown = {.timeRemaining = granade.attackSpeed};
    pushComponent(ecs, weapon, cooldownComponentId, &granade);
    WeaponTag weaponTag = {};
    pushComponent(ecs, weapon, weaponTagId, &weaponTag);
    return weapon;
}

Entity createOrbitWeapon(Ecs* ecs){
    EntityArray players = view(ecs, (size_t[]){playerTagId}, 1);
    HasWeaponComponent* hasWeapon = (HasWeaponComponent*)getComponent(ecs, players.entities[0], hasWeaponComponentId);
    Entity orbit = createEntity(ecs);
    hasWeapon->weaponId[hasWeapon->weaponCount] = orbit;
    hasWeapon->weaponType[hasWeapon->weaponCount] = WeaponType::WEAPON_ORBIT;
    hasWeapon->weaponCount++;
    OrbitingWeaponComponent orbitComponent = {};
    orbitComponent.target = players.entities[0];
    pushComponent(ecs, orbit, orbitingWeaponComponentId, &orbitComponent);
    return orbit;
}

void addOrbitProjectile(Ecs* ecs, Entity weaponId){
    EntityArray players = view(ecs, (size_t[]){playerTagId}, 1);
    Entity orbit = createEntity(ecs);
    TransformComponent* t = (TransformComponent*)getComponent(ecs, players.entities[0], transformComponentId);
    t->scale = {1,1,1};
    t->rotation = {0,0,0};
    pushComponent(ecs, orbit, transformComponentId, &t);

    SpriteComponent sprite = {
        .texture = getTexture("default"),
        .index = {0,0},
        .size = {16, 16},
        .ySort = true,
        .layer = 1.0f,
        .color = {1,0,0,0.5}
    };
    pushComponent(ecs, orbit, spriteComponentId, &sprite);
    OrbitingProjectile projectile = {};
    OrbitingWeaponComponent* weapon = (OrbitingWeaponComponent*)getComponent(ecs, weaponId, orbitingWeaponComponentId);
    projectile.slotIndex = weapon->slotCount;
    weapon->slotCount++;
    pushComponent(ecs, orbit, orbitingProjectileId, &projectile);
    HitBox hitbox = {.dmg = weapon->dmg, .offset = {0,0}, .size = {16, 16}};
    pushComponent(ecs, orbit, hitBoxId, &hitbox);
}

void fireGun(Ecs* ecs, Entity weaponId, const glm::vec3 spawnPosition, const glm::vec2 direction){
    GunComponent* gun = (GunComponent*)getComponent(ecs, weaponId, gunComponentId);
    if(!gun) return;
    CooldownComponent* cooldown = (CooldownComponent*)getComponent(ecs, weaponId, cooldownComponentId);
    if(!cooldown) return;
    if(cooldown->timeRemaining > 0.0f) return;
    createProjectile(ecs, spawnPosition, direction, gun->dmg, gun->range, gun->radius, gun->piercing);
}

void fireShotgun(Ecs* ecs, Entity weaponId, const glm::vec3 spawnPosition, const glm::vec2 direction){
    ShotgunComponent* gun = (ShotgunComponent*)getComponent(ecs, weaponId, shotgunComponentId);
    if(!gun) return;
    CooldownComponent* cooldown = (CooldownComponent*)getComponent(ecs, weaponId, cooldownComponentId);
    if(!cooldown) return;
    if(cooldown->timeRemaining > 0.0f) return;
    if(fabs(direction.x) > 0){
        float top = glm::cos(glm::radians(30.0f));
        float bottom = glm::sin(glm::radians(30.0f));
        glm::vec2 dir = {top, bottom};
        dir.x = dir.x * direction.x;
        dir = glm::normalize(dir);
        createProjectile(ecs, spawnPosition, dir, gun->dmg, gun->range, gun->radius, gun->piercing);
        top = glm::cos(glm::radians(-30.0f));
        bottom = glm::sin(glm::radians(-30.0f));
        dir = {top, bottom};
        dir.x = dir.x * direction.x;
        dir = glm::normalize(dir);
        createProjectile(ecs, spawnPosition, dir, gun->dmg, gun->range, gun->radius, gun->piercing);
        top = glm::cos(glm::radians(15.0f));
        bottom = glm::sin(glm::radians(15.0f));
        dir = {top, bottom};
        dir.x = dir.x * direction.x;
        dir = glm::normalize(dir);
        createProjectile(ecs, spawnPosition, dir, gun->dmg, gun->range, gun->radius, gun->piercing);
        top = glm::cos(glm::radians(-15.0f));
        bottom = glm::sin(glm::radians(-15.0f));
        dir = {top, bottom};
        dir.x = dir.x * direction.x;
        dir = glm::normalize(dir);
        createProjectile(ecs, spawnPosition, dir, gun->dmg, gun->range, gun->radius, gun->piercing);
    }else if (fabs(direction.y) > 0){
        //createProjectile(ecs, spawnPosition - glm::vec3(5.0f, 0.0f, 0.0f), direction, gun->dmg, gun->range, gun->radius, gun->piercing);
        //createProjectile(ecs, spawnPosition + glm::vec3(5.0f, 0.0f, 0.0f), direction, gun->dmg, gun->range, gun->radius, gun->piercing);
        float right = glm::cos(glm::radians(30.0f));
        float left = glm::sin(glm::radians(30.0f));
        glm::vec2 dir = {left, right};
        dir.y = dir.y * direction.y;
        dir = glm::normalize(dir);
        createProjectile(ecs, spawnPosition, dir, gun->dmg, gun->range, gun->radius, gun->piercing);
        right = glm::cos(glm::radians(-30.0f));
        left = glm::sin(glm::radians(-30.0f));
        dir = {left, right};
        dir.y = dir.y * direction.y;
        dir = glm::normalize(dir);
        createProjectile(ecs, spawnPosition, dir, gun->dmg, gun->range, gun->radius, gun->piercing);
        right = glm::cos(glm::radians(15.0f));
        left = glm::sin(glm::radians(15.0f));
        dir = {left, right};
        dir.y = dir.y * direction.y;
        dir = glm::normalize(dir);
        createProjectile(ecs, spawnPosition, dir, gun->dmg, gun->range, gun->radius, gun->piercing);
        right = glm::cos(glm::radians(-15.0f));
        left = glm::sin(glm::radians(-15.0f));
        dir = {left, right};
        dir.y = dir.y * direction.y;
        dir = glm::normalize(dir);
        createProjectile(ecs, spawnPosition, dir, gun->dmg, gun->range, gun->radius, gun->piercing);
    }
}

void fireSniper(Ecs* ecs, Entity weaponId, const glm::vec3 spawnPosition, const glm::vec2 direction){
    SniperComponent* gun = (SniperComponent*)getComponent(ecs, weaponId, sniperComponentId);
    if(!gun) return;
    CooldownComponent* cooldown = (CooldownComponent*)getComponent(ecs, weaponId, cooldownComponentId);
    if(!cooldown) return;
    if(cooldown->timeRemaining > 0.0f) return;
    createProjectile(ecs, spawnPosition, direction, gun->dmg, gun->range, gun->radius, gun->piercing);
}

void fireGranade(Ecs* ecs, Entity weaponId, glm::vec3 targetPosition, const glm::vec2 dir){
    GranadeComponent* granade = (GranadeComponent*)getComponent(ecs, weaponId, granadeComponentId);
    if(!granade) return;
    CooldownComponent* cooldown = (CooldownComponent*)getComponent(ecs, weaponId, cooldownComponentId);
    if(!cooldown) return;
    if(cooldown->timeRemaining > 0.0f) return;
    //createProjectile(ecs, spawnPosition, direction, granade->dmg, granade->range, granade->radius, granade->piercing);
    Entity projectile = createEntity(ecs);

    SpriteComponent sprite = {
        .texture = getTexture("granade"),
        .index = {0,0},
        .size = {16, 16},
        .ySort = false,
        .layer = 2.0f
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

    pushComponent(ecs, projectile, transformComponentId, &transform);
    pushComponent(ecs, projectile, spriteComponentId, &sprite);
    pushComponent(ecs, projectile, velocityComponentId, &velocity);
    pushComponent(ecs, projectile, directionComponentId, &direction);
    pushComponent(ecs, projectile, explosionComponentId, &explosion);
    pushComponent(ecs, projectile, granadeComponentId, granade);
}

void explosionSystem(Ecs* ecs){
    EntityArray entities = view(ecs, (size_t[]){explosionComponentId}, 1);
    for(size_t i = 0; i < entities.count; i++){
        Entity e = entities.entities[i];
        ExplosionComponent* explosionComponent = (ExplosionComponent*)getComponent(ecs, e, explosionComponentId);
        GranadeComponent* granade = (GranadeComponent*)getComponent(ecs, e, granadeComponentId);
        TransformComponent* t = (TransformComponent*)getComponent(ecs, e, transformComponentId);
        if(t->position.y <= explosionComponent->targetPosition.y){
            Entity explosion = createEntity(ecs);
            LifeTime life = {.endTime = 1};
            pushComponent(ecs, explosion, lifeTimeId, &life);
            SpriteComponent sprite = {
                .texture = getTexture("default"),
                .index = {0,0},
                .size = {32, 32},
                .color = {1, 0, 0, 0.3},
                .ySort = true,
                .layer = 1.0f
            };
            TransformComponent transform = {    
                .position = {explosionComponent->targetPosition.x - (sprite.size.x / 2), explosionComponent->targetPosition.y - (sprite.size.y / 2), explosionComponent->targetPosition.z},
                .scale = {1.0f, 1.0f, 0.0f},
                .rotation = {0.0f, 0.0f, 0.0f}
            };
            pushComponent(ecs, explosion, spriteComponentId, &sprite);
            pushComponent(ecs, explosion, transformComponentId, &transform);
            HitBox hitbox = {.dmg = granade->dmg, .offset = {0,0}, .size = sprite.size};
            pushComponent(ecs, explosion, hitBoxId, &hitbox);
            removeEntity(ecs, e);
        }
    }
}

void weaponFireSystem(Ecs* ecs){
    PROFILER_START();
    EntityArray entities = view(ecs, (size_t[]){hasWeaponComponentId}, 1);
    //for(Entity e : entities){
    for(size_t i = 0; i < entities.count; i++){
        Entity e = entities.entities[i];
        HasWeaponComponent* hasWeapon = (HasWeaponComponent*)getComponent(ecs, e, hasWeaponComponentId);
        TransformComponent* t = (TransformComponent*)getComponent(ecs, e, transformComponentId);
        Box2DCollider* b = (Box2DCollider*)getComponent(ecs, e, box2DColliderId);
        if(hasWeapon->weaponCount <= 0){
            continue;
        }
        CooldownComponent* cooldown = (CooldownComponent*)getComponent(ecs, hasWeapon->weaponId[0], cooldownComponentId);
        InputComponent* inputComponent = (InputComponent*)getComponent(ecs, e, inputComponentId);
        if(!cooldown) continue;
        if(cooldown->timeRemaining > 0.0f) continue;

        glm::vec3 center = glm::vec3(getBoxCenter(b), t->position.z);
        if(hasComponent(ecs, hasWeapon->weaponId[0], gunComponentId) && inputComponent->fire){
            fireGun(ecs, hasWeapon->weaponId[0], center, inputComponent->direction);
            GunComponent* gun = (GunComponent*)getComponent(ecs, hasWeapon->weaponId[0], gunComponentId);
            cooldown->timeRemaining = gun->attackSpeed;
        }else if(hasComponent(ecs, hasWeapon->weaponId[0], shotgunComponentId) && inputComponent->fire){
            fireShotgun(ecs, hasWeapon->weaponId[0], center, inputComponent->direction);
            ShotgunComponent* gun = (ShotgunComponent*)getComponent(ecs, hasWeapon->weaponId[0], shotgunComponentId);
            cooldown->timeRemaining = gun->attackSpeed;
       }else if(hasComponent(ecs, hasWeapon->weaponId[0], sniperComponentId) && inputComponent->fire){
            fireSniper(ecs, hasWeapon->weaponId[0], center, inputComponent->direction);
            SniperComponent* gun = (SniperComponent*)getComponent(ecs, hasWeapon->weaponId[0], sniperComponentId);
            cooldown->timeRemaining = gun->attackSpeed;
        }
        for(size_t weapon = 0; weapon < hasWeapon->weaponCount; weapon++){
            CooldownComponent* cooldown = (CooldownComponent*)getComponent(ecs, hasWeapon->weaponId[weapon], cooldownComponentId);
            if(!cooldown) continue;
            if(cooldown->timeRemaining > 0.0f) continue;
            if(hasComponent(ecs, hasWeapon->weaponId[weapon], granadeComponentId)){
                int randomX = (rand() % 150) * ((rand() % 2) * 2 - 1);
                int randomY = (rand() % 150) * ((rand() % 2) * 2 - 1);// + 100;
                glm::vec3 target = {center.x + randomX, center.y + randomY, center.z};
                fireGranade(ecs, hasWeapon->weaponId[weapon], target, {0, -1});
                GranadeComponent* granade = (GranadeComponent*)getComponent(ecs, hasWeapon->weaponId[weapon], granadeComponentId);
                cooldown->timeRemaining = granade->attackSpeed;
                LOGINFO("granade launched");
            }
        }
    }
    PROFILER_END();
}

void cooldownSystem(Ecs* ecs, float dt){
    PROFILER_START();
    EntityArray entities = view(ecs, (size_t[]){cooldownComponentId}, 1);
    //for(Entity e : entities){
    for(size_t i = 0; i < entities.count; i++){
        Entity e = entities.entities[i];
        CooldownComponent* cooldown = (CooldownComponent*)getComponent(ecs, e, cooldownComponentId);
        cooldown->timeRemaining -= dt;
    }
    PROFILER_END();
}