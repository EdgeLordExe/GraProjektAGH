#ifndef HPP_HELPERS 
#define HPP_HELPERS

#include "raylib.h"
#include "entity.hpp"


Vector2 GetMousePositionInWorld();

void MoveAndSlide(EntityId id,Vector2 velocity);

void MoveAndSlide(EntityId id,Vector2 velocity, bool collide_with_entities);

void MoveAndSlide(EntityId id,Vector2 velocity, bool collide_with_entities, bool del_on_hit);

void MoveAndSlide(EntityId id,Vector2 velocity, bool collide_with_entities, bool del_on_hit, uint64_t ignore_entities_with_components);

int RandomPositionX();

int RandomPositionY();

#endif