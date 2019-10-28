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

void pacer_think(Entity_T* self) {
	if (self->flags & FL_PUSHED) {
		Vector3D originalAccel = *(Vector3D*)self->data2;
		if (vector3d_equal(originalAccel, self->acceleration)) {
			self->flags & ~FL_PUSHED;
		}
		Vector3D difference;
		difference.x = ((Vector3D*)self->data2)->x - ((Vector3D*)self->data)->x*300;
		difference.y = ((Vector3D*)self->data2)->y - ((Vector3D*)self->data)->y*300;
		self->acceleration.x += difference.x/60.0f;
		self->acceleration.y += difference.y/60.0f;
	}
	if (self->groundentity) {
		//check for no ground on next move (use velocity, frametime, and origin to ray cast down from projected move origin)
		Point3D projectedOrigin;
		vector3d_scale(projectedOrigin, self->velocity, FRAMETIME*5);
		vector3d_add(projectedOrigin, self->boundingBox.position, projectedOrigin);
		Ray raycast = ray(projectedOrigin, vector3d(0,0,-10));
		NormalizeDirection(raycast);
		if (RaycastAABB(self->groundentity->boundingBox, raycast)<0.0f) {
			//rotate model 180 degrees and invert velocity and acceleration
			rotate_entity(self, GFC_PI, vector3d(0, 0, 1));
			self->velocity.x *= -1;
			self->velocity.y *= -1;
			self->acceleration.x *= -1;
			self->acceleration.y *= -1;
		}
	}
	self->nextthink = 0.1f;
}
void pacer_touch(Entity_T* self, Entity_T* other) {
	if (other == player) {
		slog("thouch");
		if (other->boundingBox.position.z - other->boundingBox.size.z + 0.1f > self->boundingBox.position.z + self->boundingBox.size.z) {
			self->health = 0.0f;
			self->movetype = MOVETYPE_NONE;
			return;
		}
		float zRot = getAngles(self->modelMat).z;
		if (self->acceleration.x != 0.0f) {
		self->acceleration.x *= -1;
			if (zRot <= 0.0f) {
				self->velocity.x = -self->maxspeed.x;
			}
			else {
				self->velocity.x = self->maxspeed.x;
			}
		}
		if (self->acceleration.y != 0.0f) {
		self->acceleration.y *= -1; 
			if (zRot < 180.0f && zRot >= -180.0f) {
				self->velocity.y = -self->maxspeed.y;
			}
			else {
				self->velocity.y = self->maxspeed.y;
			}
		}
		rotate_entity(self, GFC_PI, vector3d(0, 0, 1));
	}
	/**
	if (other == player && !(self->flags & FL_PUSHED)) {
		//accelerate sharply away
		slog("in here");
		self->flags = self->flags | FL_PUSHED;
		self->data2 = &self->acceleration; //store normal acceleration before push
		Vector3D push, forward, side, u;
		forward.x = self->boundingBox.position.x - other->boundingBox.position.x;
		forward.y = self->boundingBox.position.y - other->boundingBox.position.y;
		forward.z = 0;
		//vector3d_normalize(&forward);

		vector3d_cross_product(&side, forward, vector3d(0,0,1));
		//vector3d_normalize(&side);

		vector3d_add(push, forward, side);
		//vector3d_add(push, push, vector3d(0, 0, 1));
		self->data = &push;
		vector3d_scale(self->acceleration, push, 300);
	}
	*/
}
void pacer_die(Entity_T* self) {
	speed_up_die(self);
}

void player_think(Entity_T* self) {
	self->nextthink = 0.1f;
}
