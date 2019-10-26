#ifndef __MOB_METHODS_H__
#define __MOB_METHODS_H__
#include "gf3d_entity.h"
#include "physics.h"

void mushroom_think(Entity_T* self);
void mushroom_touch(Entity_T* self, Entity_T* other);
void mushroom_die(Entity_T* self);
void spring_think(Entity_T* self);
void spring_touch(Entity_T* self, Entity_T* other);
void spring_die(Entity_T* self);
void speed_up_think(Entity_T* self);
void speed_up_touch(Entity_T* self, Entity_T* other);
void speed_up_die(Entity_T* self);
#endif
