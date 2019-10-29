#include "linmath.h"
#include "gfc_matrix.h"
#include "gf3d_entity.h"
#include "simple_logger.h"
#include "local.h"
#include "..\include\physics.h"

Bool run_think(Entity_T* ent) {
	float	thinktime;

	thinktime = ent->nextthink;
	if (thinktime <= 0)
		return true;
	if (thinktime > level.time + 0.001)
		return true;

	ent->nextthink = 0;
	if (!ent->think) {
		slog("NULL ent->think");
		return true;
	}
	ent->think(ent);

	return false;
}

void physics_none(Entity_T* ent) {
	// regular thinking
	run_think(ent);
}

void physics_step(Entity_T* ent) {
	float speed;
	Vector3D tempAccel, tempVel, tempOrigin, testOrigin;
	AABB tempBox, testBox;
	Bool clippingx =false,clippingy =false,clippingz = false, clipping = false;
	Entity_T* other;
	Vector3D temp;
	int j = 0;
	//Set velocity
	if (ent->acceleration.x == 0.0f) {
		ent->velocity.x *= 0.96f;
		if (abs(ent->velocity.x) < 0.5f) {
			ent->velocity.x = 0.0f;
		}
	}
	if (ent->acceleration.y == 0.0f) {
		ent->velocity.y *= 0.96f;
		if (abs(ent->velocity.y) < 0.5f) {
			ent->velocity.y = 0.0f;
		}
	}
	vector3d_copy(tempAccel, ent->acceleration);
	vector3d_add(ent->velocity, ent->velocity, tempAccel);
	if (ent->velocity.x < -ent->maxspeed.x) {
		ent->velocity.x = -ent->maxspeed.x;
	}
	else if (ent->velocity.x > ent->maxspeed.x) {
		ent->velocity.x = ent->maxspeed.x;
	}
	if (ent->velocity.y < -ent->maxspeed.y) {
		ent->velocity.y = -ent->maxspeed.y;
	}
	else if (ent->velocity.y > ent->maxspeed.y) {
		ent->velocity.y = ent->maxspeed.y;
	}
	if (ent->velocity.z < -ent->maxspeed.z) {
		ent->velocity.z = -ent->maxspeed.z;
	}
	else if (ent->velocity.z > ent->maxspeed.z) {
		ent->velocity.z = ent->maxspeed.z;
	}
	if (ent->movetype == MOVETYPE_STEP) { //apply gravity
		ent->velocity.z -= 15.0f;		
	}
	if (vector3d_equal(ent->velocity, vector3d(0.0f, 0.0f, 0.0f))) {
		j = gf3d_entity_manager.entity_max;
	}
	//Simulate move
	vector3d_scale(tempVel, ent->velocity, FRAMETIME);
	vector3d_add(tempOrigin, ent->boundingBox.position, tempVel);
	tempBox = aabb(tempOrigin, ent->boundingBox.size);
	//loop through each entity and make sure its box isn't clipping this one's
	while (j < gf3d_entity_manager.entity_max) {
		other = &entity_list[j];
		if (ent->_inuse == 0) {
			return;
		}
		if (ent == other) {
			j++;
			continue;
		}
		clipping = other->_inuse!=0&&AABBAABB(tempBox, other->boundingBox);
		if (clipping) {
			//if top of other is below bottom of entity, set groundentity
			float otherZTop = other->position.z;
			float entZBottom = ent->position.z;
			if (!ent->groundentity && otherZTop < entZBottom) {
				ent->groundentity = other;
			}
			//Test in x direction for clip
			vector3d_set(testOrigin, tempOrigin.x, ent->boundingBox.position.y, ent->boundingBox.position.z);
			testBox = aabb(testOrigin, ent->boundingBox.size);
			clippingx = clippingx || (other->movetype != MOVETYPE_NOCLIP && AABBAABB(testBox, other->boundingBox));
			//Test in y direction for clip
			vector3d_set(testOrigin, ent->boundingBox.position.x, tempOrigin.y, ent->boundingBox.position.z);
			testBox = aabb(testOrigin, ent->boundingBox.size);
			clippingy = clippingy || (other->movetype != MOVETYPE_NOCLIP && AABBAABB(testBox, other->boundingBox));
			//Test in z direction for clip
			vector3d_set(testOrigin, ent->boundingBox.position.x, ent->boundingBox.position.y, tempOrigin.z);
			testBox = aabb(testOrigin, ent->boundingBox.size);
			clippingz = clippingz || (other->movetype != MOVETYPE_NOCLIP && AABBAABB(testBox, other->boundingBox));

			Impact(ent, other);
		}
		j++;
	}
	//Verify not clipping
	if (!clippingx) {
		//Update position
		ent->position.x += tempVel.x;
		//sync matrix and bbox positions
		ent->boundingBox.position.x = ent->position.x;
		ent->boundingBox.position.x += ent->model->boudningAdjustment.x;
		if (ent == player) {
			ent->boundingBox.position.x += 0.5f;
		}
		ent->modelMat[3][0] = ent->position.x;
	}
	else {
		ent->velocity.x = 0.0f;
	}
	if (!clippingy) {
		//Update position
		ent->position.y += tempVel.y;
		//sync matrix and bbox positions
		ent->boundingBox.position.y = ent->position.y;
		ent->boundingBox.position.y += ent->model->boudningAdjustment.y;
		ent->modelMat[3][1] = ent->position.y;
	}
	else {
		ent->velocity.y = 0.0f;
	}
	if (!clippingz) {
		//Update position
		ent->position.z += tempVel.z;
		//sync matrix and bbox positions
		ent->boundingBox.position.z = ent->position.z;
		ent->boundingBox.position.z += ent->model->boudningAdjustment.z;
		ent->modelMat[3][2] = ent->position.z;
		if (ent->groundentity) { 
			ent->groundentity = NULL; 
		}
	}
	else {
		if (ent->velocity.z <= 0.0f) {

			int combo = ent->flags & ~FL_JUMPING;
			ent->flags = combo;
		}
		ent->velocity.z = 0.0f;
	}
	run_think(ent);
}

void update_physics_positions() {
	int i = 0;
	Entity_T* ent;
	if (!entity_list) {
		slog("Tried to find entity before initializing entity list!");
	}
	while (i < gf3d_entity_manager.entity_max) {
		ent = &entity_list[i];
		if (ent->_inuse == 0) {
			i++;
			continue;
		}
		if (ent->prethink) {
			ent->prethink(ent);
		}
		if (ent->movetype == MOVETYPE_NONE || ent->movetype == MOVETYPE_NOCLIP) {
			physics_none(ent);
		}
		else if (ent->movetype == MOVETYPE_STEP) {
			physics_step(ent);
		}
		i++;
	}
}

void Impact(Entity_T* e1, Entity_T* e2)
{
	if (e1->touch)
		e1->touch(e1, e2);

	if (e2->touch)
		e2->touch(e2, e1);
}

void teleport_entity(Entity_T* ent, Vector3D position)
{
	//matrix translate
	ent->modelMat[3][0] = position.x;
	ent->modelMat[3][1] = position.y;
	ent->modelMat[3][2] = position.z;
	//gfc_matrix_make_translation(ent->modelMat,position);
	//position translate
	vector3d_copy(ent->position, position);
	//bounding box translate
	ent->boundingBox.position = ent->position;
}
