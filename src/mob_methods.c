#include "mob_methods.h"
#include "simple_logger.h"

void mushroom_think(Entity_T* self)
{
	rotate_entity(self, 0.01, vector3d(0, 0, 1));
	self->nextthink = 1.0f;
}

void mushroom_touch(Entity_T* self, Entity_T* other)
{
	if (self->health != 0.0f && other == player) {
		player->health += 10.0f;
		slog("Player health now %f", player->health);
		self->health = 0.0f;
	}
}

void mushroom_die(Entity_T* self)
{
	if (self->position.z > -30) {
		teleport_entity(self, vector3d(self->position.x, self->position.y, self->position.z - 0.06f));
	}
	else {
		self->_inuse = 0;
	}
}

void spring_think(Entity_T* self)
{
	rotate_entity(self, 0.01, vector3d(0, 0, 1));
	self->nextthink = 1.0f;
}

void spring_touch(Entity_T* self, Entity_T* other)
{
	if (self->health != 0.0f && other == player) {
		player->specFloat1 += 500.0f;
		slog("Player jump height now %f", player->specFloat1);
		self->health = 0.0f;
	}
}

void spring_die(Entity_T* self)
{
	if (self->position.z > -30) {
		teleport_entity(self, vector3d(self->position.x, self->position.y, self->position.z - 0.06f));
	}
	else {
		self->_inuse = 0;
	}
}

void speed_up_think(Entity_T* self)
{
	rotate_entity(self, 0.01, vector3d(0, 0, 1));
	self->nextthink = 1.0f;
}

void speed_up_touch(Entity_T* self, Entity_T* other)
{
	if (self->health != 0.0f && other == player) {
		player->maxspeed.x += 200.0f;
		player->maxspeed.y += 200.0f;
		slog("Player max speed now %f", player->maxspeed.x);
		self->health = 0.0f;
	}
}

void speed_up_die(Entity_T* self)
{
	if (self->position.z > -30) {
		teleport_entity(self, vector3d(self->position.x, self->position.y, self->position.z - 0.06f));
	}
	else {
		self->_inuse = 0;
	}
}
