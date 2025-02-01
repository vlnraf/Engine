#pragma once

struct HitBox{
    int dmg = 0;
    Box2DCollider area;
    Entity hittedEntity;
    bool hit = false;
    bool alreadyHitted = false;
    bool discover = false;
};

struct HurtBox{
    int health;
    Box2DCollider area;
    Entity hittedByEntity;
    //bool hit = false;
    bool hitted = false;
};