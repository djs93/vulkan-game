#include "mob_methods.h"
#include "simple_logger.h"
#include "gf3d_ui.h"

void set_rotation(Matrix4 out, Matrix4 in, Vector3D theta);
void mushroom_think(Entity_T* self)
{
	rotate_entity(self, 0.01, vector3d(0, 0, 1));
	self->nextthink = 1.0f;
}

void mushroom_touch(Entity_T* self, Entity_T* other)
{
	if (other == player && self->health != 0.0f) {
		(int)player->data += 1;
		player->health += 10;
		if (player->health > player->healthmax) {
			player->health = player->healthmax;
		}
		self->health = 0.0f;
		UIElement* count = gf3d_ui_find("mushroom Count");
		char str[14];
		sprintf(str, "Mushrooms: %d", (int)player->data);
		count->sprite = gf3d_ui_getTextSprite(str, vector4d(255,255,255,255), 100);

		UIElement* healthUI = gf3d_ui_find("health");
		char str2[12];
		sprintf(str2, "Health: %d", (int)player->health);
		healthUI->sprite = gf3d_ui_getTextSprite(str2, vector4d(255, 255, 255, 255), 100);
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
		teleport_entity(self, vector3d(self->position.x, self->position.y, self->position.z - 0.08f));
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
	if (other == player) {//should move player-specifics away from bouncing so if the ent hits a wall it can bounce
		if (other->boundingBox.position.z - other->boundingBox.size.z + 0.1f > self->boundingBox.position.z + self->boundingBox.size.z) {
			self->health = 0.0f;
			self->movetype = MOVETYPE_NOCLIP;
			teleport_entity(other, vector3d(other->position.x, other->position.y, other->position.z + 1.5f));
			other->velocity.z = other->specFloat1;
			other->groundentity = NULL;
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
}
void pacer_die(Entity_T* self) {
	speed_up_die(self);
}

void player_think(Entity_T* self) {
	//look where you're going
	Vector3D xyVel = vector3d(self->velocity.x, self->velocity.y, 0.0f);
	if (vector3d_magnitude(xyVel)>2.0f) {
		Vector3D futurePoint;
		vector3d_add(futurePoint, self->position, self->velocity);
		look_towards(self, futurePoint);
	}
	
	self->nextthink = 0.1f;
}

void jumper_think(Entity_T* self)
{
	//jump
	if (!(self->flags & FL_JUMPING)) {
		self->velocity.z = 2000.0f;
		self->flags = self->flags | FL_JUMPING;
		//rotate to face player when impacting ground
	}
	look_towards(self, player->position);
	//set xy acceleration to point at player
	Vector3D forward;
	vector3d_sub(forward, player->position, self->position);
	vector3d_normalize(&forward);
	vector3d_scale(forward, forward, 15.0f);
	forward.z = 0;
	vector3d_copy(self->acceleration, forward);
	self->nextthink = 0.1f;
}

void jumper_touch(Entity_T* self, Entity_T* other)
{
	//check for stomp
	if (other==player && other->boundingBox.position.z - other->boundingBox.size.z + 0.1f > self->boundingBox.position.z + self->boundingBox.size.z) {
		self->health = 0.0f;
		self->movetype = MOVETYPE_NOCLIP;
		teleport_entity(other, vector3d(other->position.x, other->position.y, other->position.z + 1.5f));
		other->velocity.z = other->specFloat1;
		other->groundentity = NULL;
		return;
	}
}

void jumper_die(Entity_T* self)
{
	speed_up_die(self);
}

void circler_think(Entity_T* self)
{
	//check if player is in range, if it is, chase. 
	Vector2D pos1 = vector2d(self->position.x, self->position.y);
	Vector2D pos2 = vector2d(player->position.x, player->position.y);
	if (vector2d_distance_between_less_than(pos1, pos2, 35.0f)) {
		look_towards(self, player->position);
		Vector3D forward;
		vector3d_sub(forward, player->position, self->position);
		vector3d_normalize(&forward);
		forward.z = 0;
		vector3d_scale(forward, forward, 15.0f);
		vector3d_copy(self->acceleration, forward);
	}
	else {
		rotate_entity(self, 0.01f, vector3d(0, 0, 1));
		//float angle = getAngles(self->modelMat).z;
		//Vector3D forward = vector3d(0, -1, 0);
		Vector3D angles = getAngles(self->modelMat);

		Vector3D forward;
		forward.x = -sin(angles.z);
		forward.y = -cos(angles.z);
		forward.z = 0.0f;
		//vector3d_scale(angles, angles, GFC_RADTODEG);
		//vector3d_angle_vectors(angles, &forward, NULL, NULL);
		//vector3d_rotate_about_z(&forward, angle * GFC_RADTODEG);
		vector3d_scale(forward, forward, 150.0f);
		vector3d_copy(self->acceleration, forward); 
		//Vector3D futurePoint;
		//vector3d_add(futurePoint, self->position, self->velocity);
		//look_towards(self, forward);
	}
	//else:
	//rotate slightly
	//change acceleration to point forward relative to ent
	self->nextthink = 0.1f;
}

void circler_touch(Entity_T* self, Entity_T* other)
{
	if (other == player && other->boundingBox.position.z - other->boundingBox.size.z + 0.1f > self->boundingBox.position.z + self->boundingBox.size.z) {
		self->health = 0.0f;
		self->movetype = MOVETYPE_NOCLIP;
		teleport_entity(other, vector3d(other->position.x, other->position.y, other->position.z + 1.5f));
		other->velocity.z = other->specFloat1;
		other->groundentity = NULL;
		return;
	}
}

void circler_die(Entity_T* self)
{
	speed_up_die(self);
}

void look_towards(Entity_T* self, Vector3D target) {
	Vector3D pos1 = vector3d(self->position.x, self->position.y, 0);
	Vector3D pos2 = vector3d(target.x, target.y, 0);
	Vector3D dir;
	vector3d_sub(dir, pos1, pos2);
	float lookRot = GFC_PI+GFC_HALF_PI+atan2(dir.y, dir.x);
	set_rotation(self->modelMat, self->modelMat, vector3d(0,0,lookRot));
}

void set_rotation(Matrix4 out, Matrix4 in, Vector3D theta) {
	Matrix4 Rotate;
	gfc_matrix_identity(Rotate);
	gfc_matrix_multiply(Rotate, Rotate, in);
	Matrix4 Result;
	Vector3D temp;
	Vector3D axis = vector3d(0, 0, 1);
	float a = theta.z;
	float c = cos(a);
	float s = sin(a);

	vector3d_normalize(&axis);

	vector3d_scale(temp, axis, (1 - c));

	Rotate[0][0] = c + temp.x * axis.x;
	Rotate[0][1] = temp.x * axis.y + s * axis.z;
	Rotate[0][2] = temp.x * axis.z - s * axis.y;

	Rotate[1][0] = temp.y * axis.x - s * axis.z;
	Rotate[1][1] = c + temp.y * axis.y;
	Rotate[1][2] = temp.y * axis.z + s * axis.x;

	Rotate[2][0] = temp.z * axis.x + s * axis.y;
	Rotate[2][1] = temp.z * axis.y - s * axis.x;
	Rotate[2][2] = c + temp.z * axis.z;

	Rotate[3][0] = in[3][0];
	Rotate[3][1] = in[3][1];
	Rotate[3][2] = in[3][2];
	Rotate[3][3] = in[3][3];

	gfc_matrix_copy(out, Rotate);
}