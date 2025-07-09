#include "weapon.hpp"
#include "projectile.hpp"
#include "components.hpp"

Entity createGun(Ecs* ecs){
    Entity weapon = createEntity(ecs);
    GunComponent gun = {.dmg = 1, .attackSpeed = 0.2, .radius = 5};
    pushComponent(ecs, weapon, GunComponent, &gun);
    CooldownComponent cooldown = {.timeRemaining = 0.2};
    pushComponent(ecs, weapon, CooldownComponent, &cooldown);
    return weapon;
}

Entity createShotgun(Ecs* ecs){
    Entity weapon = createEntity(ecs);
    ShotgunComponent gun = {.dmg = 1, .attackSpeed = 0.4, .radius = 5};
    pushComponent(ecs, weapon, ShotgunComponent, &gun);
    CooldownComponent cooldown = {.timeRemaining = 0.4};
    pushComponent(ecs, weapon, CooldownComponent, &cooldown);
    return weapon;
}

void fireGun(Ecs* ecs, Entity weaponId, const glm::vec3 spawnPosition, const glm::vec2 direction){
    GunComponent* gun = getComponent(ecs, weaponId, GunComponent);
    if(!gun) return;
    CooldownComponent* cooldown = getComponent(ecs, weaponId, CooldownComponent);
    if(!cooldown) return;
    if(cooldown->timeRemaining > 0.0f) return;
    createProjectile(ecs, spawnPosition, direction, gun->dmg, gun->radius);
}

void fireShotgun(Ecs* ecs, Entity weaponId, const glm::vec3 spawnPosition, const glm::vec2 direction){
    ShotgunComponent* gun = getComponent(ecs, weaponId, ShotgunComponent);
    if(!gun) return;
    CooldownComponent* cooldown = getComponent(ecs, weaponId, CooldownComponent);
    if(!cooldown) return;
    if(cooldown->timeRemaining > 0.0f) return;
    if(fabs(direction.x) > 0){
        createProjectile(ecs, spawnPosition - glm::vec3(0.0f, 5.0f, 0.0f), direction, gun->dmg, gun->radius);
        createProjectile(ecs, spawnPosition + glm::vec3(0.0f, 5.0f, 0.0f), direction, gun->dmg, gun->radius);
    }else if (fabs(direction.y) > 0){
        createProjectile(ecs, spawnPosition - glm::vec3(5.0f, 0.0f, 0.0f), direction, gun->dmg, gun->radius);
        createProjectile(ecs, spawnPosition + glm::vec3(5.0f, 0.0f, 0.0f), direction, gun->dmg, gun->radius);
    }
}

void weaponFireSystem(Ecs* ecs){
    std::vector<Entity> entities = view(ecs, HasWeaponComponent);
    for(Entity e : entities){
        HasWeaponComponent* hasWeapon = getComponent(ecs, e, HasWeaponComponent);
        TransformComponent* t = getComponent(ecs, e, TransformComponent);
        Box2DCollider* b = getComponent(ecs, e, Box2DCollider);
        CooldownComponent* cooldown = getComponent(ecs, hasWeapon->weaponId, CooldownComponent);
        InputComponent* inputComponent = getComponent(ecs, e, InputComponent);
        if(cooldown->timeRemaining > 0.0f) continue;

        glm::vec3 center = t->position + glm::vec3(getBoxCenter(b), t->position.z);
        if(hasComponent(ecs, hasWeapon->weaponId, GunComponent) && inputComponent->fire){
            fireGun(ecs, hasWeapon->weaponId, center, inputComponent->direction);
            GunComponent* gun = getComponent(ecs, hasWeapon->weaponId, GunComponent);
            cooldown->timeRemaining = gun->attackSpeed;
        }
        if(hasComponent(ecs, hasWeapon->weaponId, ShotgunComponent) && inputComponent->fire){
            fireShotgun(ecs, hasWeapon->weaponId, center, inputComponent->direction);
            ShotgunComponent* gun = getComponent(ecs, hasWeapon->weaponId, ShotgunComponent);
            cooldown->timeRemaining = gun->attackSpeed;
        }
    }
}

void cooldownSystem(Ecs* ecs, float dt){
    std::vector<Entity> entities = view(ecs, CooldownComponent);
    for(Entity e : entities){
        CooldownComponent* cooldown = getComponent(ecs, e, CooldownComponent);
        cooldown->timeRemaining -= dt;
    }
}