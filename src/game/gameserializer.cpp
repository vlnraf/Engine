#include "gameserializer.hpp"

void serializeTransformComponent(SerializationState* serializer, const TransformComponent* component){
    serializeObjectStart(serializer, "TransformComponent");
    serializeVec3(serializer, "position", &component->position);
    serializeVec3(serializer, "scale", &component->scale);
    serializeVec3(serializer, "rotation", &component->rotation);
    serializeObjectEnd(serializer);

}

void serialiazeSpriteComponent(SerializationState* serializer, const SpriteComponent* component){
    serializeObjectStart(serializer, "SpriteComponent");
    serializeInt(serializer, "pivot", (int)component->pivot);
    serializeString(serializer, "path", component->texturePath);
    serializeVec2(serializer, "index", &component->index);
    serializeVec2(serializer, "size", &component->size);
    serializeVec2(serializer, "offset", &component->offset);
    serializeBool(serializer, "flipX", component->flipX);
    serializeBool(serializer, "flipY", component->flipY);
    serializeBool(serializer, "ySort", component->ySort);
    serializeFloat(serializer, "layer", component->layer);
    serializeBool(serializer, "visible", component->visible);
    serializeObjectEnd(serializer);
}

void serializeBox2DComponent(SerializationState* serializer, const Box2DCollider* component){
    serializeObjectStart(serializer, "box2DComponent");
    serializeInt(serializer, "type", (int)component->type);
    serializeBool(serializer, "active", component->active);
    serializeVec2(serializer, "offset", &component->offset);
    serializeVec2(serializer, "size", &component->size);
    serializeObjectEnd(serializer);
}

void serializeHitBox(SerializationState* serializer, const HitBox* component){
    serializeObjectStart(serializer, "HitBox");
    serializeInt(serializer, "dmg", component->dmg);
    serializeBox2DComponent(serializer, &component->area);
    serializeObjectEnd(serializer);
}

void serializeHurtBox(SerializationState* serializer, const HurtBox* component){
    serializeObjectStart(serializer, "HurtBox");
    serializeInt(serializer, "health", component->health);
    serializeBox2DComponent(serializer, &component->area);
    serializeObjectEnd(serializer);
}

void serializeDebugNameComponent(SerializationState* serializer, DebugNameComponent* comp){
    serializeObjectStart(serializer, "DebugNameComponent");
    serializeString(serializer, "name", comp->name.c_str());
    serializeObjectEnd(serializer);
}

void serializeDirectionComponent(SerializationState* serializer, DirectionComponent* comp){
    serializeObjectStart(serializer, "DirectionComponent");
    serializeVec2(serializer, "dir", &comp->dir);
    serializeObjectEnd(serializer);

}

void serializeVelocityComponent(SerializationState* serializer, VelocityComponent* comp){
    serializeObjectStart(serializer, "VelocityComponent");
    serializeVec2(serializer, "dir", &comp->vel);
    serializeObjectEnd(serializer);
}

void serializeEnmeyTag(SerializationState* serializer, EnemyTag* comp){
    serializeObjectStart(serializer, "EnemyTag");
    serializeInt(serializer, "dmg", comp->dmg);
    serializeInt(serializer, "toFollow", comp->toFollow);
    serializeObjectEnd(serializer);
}

void serializeAnimationComponent(SerializationState* serializer, AnimationComponent* comp){
    serializeObjectStart(serializer, "AnimationComponent");
    serializeString(serializer, "id", comp->id.c_str());
    serializeString(serializer, "previousId", comp->previousId.c_str());
    serializeInt(serializer, "currentFrame", comp->currentFrame);
    serializeFloat(serializer, "frameCount", comp->frameCount);
    serializeInt(serializer, "frames", comp->frames);
    serializeBool(serializer, "loop", comp->loop);
    serializeObjectEnd(serializer);

}

void serializeInputComponent(SerializationState* serializer, InputComponent* comp){
    serializeObjectStart(serializer, "InputComponent");
    serializeObjectEnd(serializer);
}

void serializePlayerTag(SerializationState* serializer, PlayerTag* comp){
    serializeObjectStart(serializer, "PlayerTag");
    serializeObjectEnd(serializer);
}

void serializeWeaponTag(SerializationState* serializer, WeaponTag* comp){
    serializeObjectStart(serializer, "WeaponTag");
    serializeObjectEnd(serializer);
}

void serializeAttachedEntity(SerializationState* serializer, AttachedEntity* comp){
    serializeObjectStart(serializer, "AttachedEntity");
    serializeInt(serializer, "Entity", (int)comp->entity);
    serializeVec2(serializer, "offset", &comp->offset);
    serializeObjectEnd(serializer);

}

void serializeGame(GameState* gameState, const char* name){
    SerializationState serializer = initSerializer(name);
    serializeString(&serializer, "GameState", "version 1");
    serializeListStart(&serializer, "Entities");
    for(const auto& entity : gameState->ecs->entityComponentMap){
        serializeInt(&serializer, "Entity", entity.first);
        serializeItemsStart(&serializer);
        for(const auto& component : entity.second){
            if(component == ECS_TRANSFORM){
                TransformComponent* comp = (TransformComponent*) getComponent(gameState->ecs, entity.first, component);
                serializeTransformComponent(&serializer, comp);
            }else if(component == ECS_SPRITE){
                SpriteComponent* comp = (SpriteComponent*) getComponent(gameState->ecs, entity.first, component);
                serialiazeSpriteComponent(&serializer, comp);
            }else if(component == ECS_DEBUG_NAME){
                DebugNameComponent* comp = (DebugNameComponent*) getComponent(gameState->ecs, entity.first, component);
                serializeDebugNameComponent(&serializer, comp);
            }else if(component == ECS_ATTACHED_ENTITY){
                AttachedEntity* comp = (AttachedEntity*) getComponent(gameState->ecs, entity.first, component);
                serializeAttachedEntity(&serializer, comp);
            }else if(component == ECS_DIRECTION){
                DirectionComponent* comp = (DirectionComponent*) getComponent(gameState->ecs, entity.first, component);
                serializeDirectionComponent(&serializer, comp);
            }else if(component == ECS_ENEMY_TAG){
                EnemyTag* comp = (EnemyTag*) getComponent(gameState->ecs, entity.first, component);
                serializeEnmeyTag(&serializer, comp);
            }else if(component == ECS_HITBOX){
                HitBox* comp = (HitBox*) getComponent(gameState->ecs, entity.first, component);
                serializeHitBox(&serializer, comp);
            }else if(component == ECS_HURTBOX){
                HurtBox* comp = (HurtBox*) getComponent(gameState->ecs, entity.first, component);
                serializeHurtBox(&serializer, comp);
            }else if(component == ECS_INPUT){
                InputComponent* comp = (InputComponent*) getComponent(gameState->ecs, entity.first, component);
                serializeInputComponent(&serializer, comp);
            }else if(component == ECS_PLAYER_TAG){
                PlayerTag* comp = (PlayerTag*) getComponent(gameState->ecs, entity.first, component);
                serializePlayerTag(&serializer, comp);
            }else if(component == ECS_VELOCITY){
                VelocityComponent* comp = (VelocityComponent*) getComponent(gameState->ecs, entity.first, component);
                serializeVelocityComponent(&serializer, comp);
            }else if(component == ECS_WEAPON){
                WeaponTag* comp = (WeaponTag*) getComponent(gameState->ecs, entity.first, component);
                serializeWeaponTag(&serializer, comp);
            }else if(component == ECS_2D_BOX_COLLIDER){
                Box2DCollider* comp = (Box2DCollider*) getComponent(gameState->ecs, entity.first, component);
                serializeBox2DComponent(&serializer, comp);
            }else if(component == ECS_ANIMATION){
                AnimationComponent* comp = (AnimationComponent*) getComponent(gameState->ecs, entity.first, component);
                serializeAnimationComponent(&serializer, comp);
            }
        }
        serializeItemsEnd(&serializer);
    }
    serializeListEnd(&serializer);
}

bool deserializeBool(std::string input){
    if(strcmp(input.c_str(), "True") == 0){
        return true;
    }else{
        return false;
    }
}

glm::vec3 deserializeVec3(std::string input){
    glm::vec3 value;
    // Remove the square brackets
    input = input.substr(1, input.size() - 2);
    // Parse the floats manually
    size_t pos1 = input.find(",");       // Find first comma
    size_t pos2 = input.find(",", pos1 + 1); // Find second comma

    value.x = std::stof(input.substr(0, pos1));
    value.y = std::stof(input.substr(pos1 + 1, pos2-pos1 - 1));
    value.z = std::stof(input.substr(pos2+1));
    return value;
}

glm::vec2 deserializeVec2(std::string input){
    glm::vec2 value;
    // Remove the square brackets
    input = input.substr(1, input.size() - 2);
    // Parse the floats manually
    size_t pos1 = input.find(",");       // Find first comma

    value.x = std::stof(input.substr(0, pos1));
    value.y = std::stof(input.substr(pos1+1));
    return value;
}

TransformComponent deserializeTransformComponent(Node* component){
    TransformComponent result;
    for(Node c : component->childrens){
        if(strcmp(c.key.c_str(), "position") == 0){
            result.position = deserializeVec3(c.value);
        }else if(strcmp(c.key.c_str(), "rotation") == 0){
            result.rotation = deserializeVec3(c.value);
        }else if(strcmp(c.key.c_str(), "scale") == 0){
            result.scale = deserializeVec3(c.value);
        }
    }
    return result;
}

SpriteComponent deserializeSpriteComponent(Node* component){
    SpriteComponent result;
    for(Node c : component->childrens){
        if(strcmp(c.key.c_str(), "pivot") == 0){
            result.pivot = (SpriteComponent::PivotType)std::stoi(c.value);
        }else if(strcmp(c.key.c_str(), "path") == 0){
            Texture* t = loadTexture(c.value.c_str());
            result.texture = t;
            std::strncpy(result.texturePath, c.value.c_str(), sizeof(result.texturePath));
            //result.texturePath = c.value.c_str();
        }else if(strcmp(c.key.c_str(), "index") == 0){
            result.index = deserializeVec2(c.value);
        }else if(strcmp(c.key.c_str(), "size") == 0){
            result.size = deserializeVec2(c.value);
        }else if(strcmp(c.key.c_str(), "offset") == 0){
            result.offset = deserializeVec2(c.value);
        }else if(strcmp(c.key.c_str(), "flipX") == 0){
            result.flipX = deserializeBool(c.value);
        }else if(strcmp(c.key.c_str(), "flipY") == 0){
            result.flipY = deserializeBool(c.value);
        }else if(strcmp(c.key.c_str(), "ySort") == 0){
            result.ySort = deserializeBool(c.value);
        }else if(strcmp(c.key.c_str(), "layer") == 0){
            result.layer = (float)(int)std::stof(c.value);
        }else if(strcmp(c.key.c_str(), "visible") == 0){
            result.visible = deserializeBool(c.value);
        }
    }
    return result;
}

DebugNameComponent deserializeDebugNameComponent(Node* component){
    DebugNameComponent result;
        for(Node c : component->childrens){
            result.name = c.value;
        }
    return result;
}

DirectionComponent deserializeDirectionComponent(Node* component){
    DirectionComponent result;
    for(Node c : component->childrens){
        result.dir = deserializeVec2(c.value);
    }
    return result;
}

InputComponent deserializeInputComponent(Node* component){
    InputComponent result;
    return result; //If present return, no data component
}

VelocityComponent deserializeVelocityComponent(Node* component){
    VelocityComponent result;
    for(Node c : component->childrens){
        result.vel = deserializeVec2(c.value);
    }
    return result;
}

PlayerTag deserializePlayerTag(Node* component){
    PlayerTag result;
    return result; //Player tag is empty so if it's present in the file we deserialize
}

EnemyTag deserializeEnemyTag(Node* component){
    EnemyTag result;
    for(Node c : component->childrens){
        if(strcmp(c.key.c_str(), "toFollow") == 0){
            result.toFollow = (Entity)std::stoi(c.value);
        }
    }
    return result;
}

Box2DCollider deserializeBox2d(Node* component){
    Box2DCollider result;
    for(Node c : component->childrens){
        if(strcmp(c.key.c_str(), "type") == 0){
            result.type = (Box2DCollider::ColliderType)std::stoi(c.value);
        }else if(strcmp(c.key.c_str(), "active") == 0){
            result.active = deserializeBool(c.value);
        }else if(strcmp(c.key.c_str(), "offset") == 0){
            result.offset = deserializeVec2(c.value);
        }else if(strcmp(c.key.c_str(), "size") == 0){
            result.size = deserializeVec2(c.value);
        }
    }
    return result;
}

HitBox deserializeHitBox(Node* component){
    HitBox result;
    for(Node c : component->childrens){
        if(strcmp(c.key.c_str(), "dmg") == 0){
            result.dmg = std::stoi(c.value);
        }else if(strcmp(c.key.c_str(), "box2DComponent") == 0){
            Box2DCollider col = deserializeBox2d(&c);
            result.area = col;
        }
    }
    return result;
}

HurtBox deserializeHurtBox(Node* component){
    HurtBox result;
    for(Node c : component->childrens){
        if(strcmp(c.key.c_str(), "health") == 0){
            result.health = std::stoi(c.value);
        }else if(strcmp(c.key.c_str(), "box2DComponent") == 0){
            Box2DCollider col = deserializeBox2d(&c);
            result.area = col;
        }
    }
    return result;
}

AttachedEntity deserializeAttachedEntity(Node* component){
    AttachedEntity result;
    for(Node c : component->childrens){
        if(strcmp(c.key.c_str(), "Entity") == 0){
            result.entity = (Entity)(std::stoi(c.value));
        }else if(strcmp(c.key.c_str(), "offset") == 0){
            result.offset = deserializeVec2(c.value);
        }

    }
    return result;
}

void deserializeGame(GameState* gameState, const char* filePath){
    Node root = serializeReadFile(filePath);
    //check if the root has some children
    if(root.childrens.size() <= 0){
        return;
    }
    Node* entities = getNode(&root, "Entities");
    if(entities){
        for(Node& e : entities->childrens){
            Entity entity = createEntity(gameState->ecs);
            Node* transform = getNode(&e, "TransformComponent");
            if(transform){
                TransformComponent tc = deserializeTransformComponent(transform);
                pushComponent(gameState->ecs, entity, ECS_TRANSFORM, &tc, sizeof(TransformComponent));
            }
            Node* debugName = getNode(&e, "DebugNameComponent");
            if(debugName){
                DebugNameComponent dbName = deserializeDebugNameComponent(debugName);
                pushComponent(gameState->ecs, entity, ECS_DEBUG_NAME, &dbName, sizeof(DebugNameComponent));
            }
            Node* dc = getNode(&e, "DirectionComponent");
            if(dc){
                DirectionComponent direction = deserializeDirectionComponent(dc);
                pushComponent(gameState->ecs, entity, ECS_DIRECTION, &direction, sizeof(DirectionComponent));
            }
            Node* ic = getNode(&e, "InputComponent");
            if(ic){
                InputComponent input = deserializeInputComponent(ic);
                pushComponent(gameState->ecs, entity, ECS_INPUT, &input, sizeof(InputComponent));
            }
            Node* vc = getNode(&e, "VelocityComponent");
            if(vc){
                VelocityComponent velocity = deserializeVelocityComponent(vc);
                pushComponent(gameState->ecs, entity, ECS_VELOCITY, &velocity, sizeof(VelocityComponent));
            }
            Node* playerTag = getNode(&e, "PlayerTag");
            if(playerTag){
                PlayerTag pt = deserializePlayerTag(playerTag);
                pushComponent(gameState->ecs, entity, ECS_PLAYER_TAG, &pt, sizeof(PlayerTag));
            }
            Node* enemyTag = getNode(&e, "EnemyTag");
            if(enemyTag){
                EnemyTag et = deserializeEnemyTag(enemyTag);
                pushComponent(gameState->ecs, entity, ECS_ENEMY_TAG, &et, sizeof(EnemyTag));
            }
            Node* col2D = getNode(&e, "box2DComponent");
            if(col2D){
                Box2DCollider box = deserializeBox2d(col2D);
                pushComponent(gameState->ecs, entity, ECS_2D_BOX_COLLIDER, &box, sizeof(Box2DCollider));
            }
            Node* sp = getNode(&e, "SpriteComponent");
            if(sp){
                SpriteComponent sprite = deserializeSpriteComponent(sp);
                pushComponent(gameState->ecs, entity, ECS_SPRITE, &sprite, sizeof(SpriteComponent));
            }
            Node* huBox = getNode(&e, "HurtBox");
            if(huBox){
                HurtBox hurtBox = deserializeHurtBox(huBox);
                pushComponent(gameState->ecs, entity, ECS_HURTBOX, &hurtBox, sizeof(HurtBox));
            }
            Node* hiBox = getNode(&e, "HitBox");
            if(hiBox){
                HitBox hitBox = deserializeHitBox(hiBox);
                pushComponent(gameState->ecs, entity, ECS_HITBOX, &hitBox, sizeof(HitBox));
            }
            Node* ac = getNode(&e, "AnimationComponent");
            if(ac){
                AnimationComponent anim;
                pushComponent(gameState->ecs, entity, ECS_ANIMATION, &anim, sizeof(AnimationComponent));
            }
            Node* att = getNode(&e, "AttachedEntity");
            if(att){
                AttachedEntity attached = deserializeAttachedEntity(att);
                pushComponent(gameState->ecs, entity, ECS_ATTACHED_ENTITY, &attached, sizeof(AttachedEntity));
            }
            Node* weap = getNode(&e, "WeaponTag");
            if(weap){
                WeaponTag weapon;
                pushComponent(gameState->ecs, entity, ECS_WEAPON, & weapon, sizeof(WeaponTag));
            }
        }
    }
}
