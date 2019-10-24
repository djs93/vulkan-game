#include "linmath.h"
#include "gfc_matrix.h"
#include "gf3d_entity.h"
#include "simple_logger.h"
#include "local.h"
#include "..\include\physics.h"

// movement parameters
float	pm_stopspeed = 100;
float	pm_maxspeed = 300;
float	pm_duckspeed = 100;
float	pm_accelerate = 10;
float	pm_airaccelerate = 0;
float	pm_friction = 6;

void run_entity(Entity_T* ent)
{
	if (!ent) {
		slog("No entity passed into run_entity!");
		return;
	}
	if (!ent->_inuse) {
		return;
	}

	if (ent->prethink) {
		ent->prethink(ent);
	}
	/**
	switch ((int)ent->movetype)
	{
		case MOVETYPE_PUSH:
		case MOVETYPE_STOP:
			//physics_pusher(ent);
			break;
		case MOVETYPE_NONE:
			physics_none(ent);
			break;
		case MOVETYPE_NOCLIP:
			//physics_noclip(ent);
			break;
		case MOVETYPE_STEP:
			//physics_step(ent);
			break;
		case MOVETYPE_TOSS:
		case MOVETYPE_BOUNCE:
		case MOVETYPE_FLY:
		case MOVETYPE_FLYMISSILE:
			//physics_toss(ent);
			break;
		default:
			slog("Bad movetype %i", (int)ent->movetype);
	}
	*/
}

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

void physics_pusher(Entity_T* ent) {
	//push physics
	run_think(ent);
}

void physics_none(Entity_T* ent) {
	// regular thinking
	run_think(ent);
}

void physics_noclip(Entity_T* ent) {
	//noclip physics
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
	if (vector3d_equal(ent->acceleration, vector3d(0.0f, 0.0f, 0.0f))) {
		//decelerate v towards stop
		ent->velocity.x *= 0.98f;
		ent->velocity.y *= 0.98f;
		ent->velocity.z -= 9.81f;

		speed = sqrt(ent->velocity.x * ent->velocity.x + ent->velocity.y * ent->velocity.y);
		if (speed < 0.5f) {
			ent->velocity.x = 0.0f;
			ent->velocity.y = 0.0f;
		}

	}
	else {
		vector3d_copy(tempAccel, ent->acceleration);
		vector3d_add(ent->velocity, ent->velocity, tempAccel);
		if (ent->movetype == MOVETYPE_STEP) { //apply gravity
			if (!ent->groundentity) {
				ent->velocity.z -= 9.81f;
			}
			else {
				if (ent->velocity.z > 0.0f) {
					ent->groundentity = NULL;
				}
			}
		}
	}
	if (vector3d_equal(ent->velocity, vector3d(0.0f, 0.0f, 0.0f))) {
		return;
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
		clipping = AABBAABB(tempBox, other->boundingBox);
		if (clipping) {
			//if top of other is below bottom of entity, set groundentity
			if ((other->position.z + other->boundingBox.size.z) < (ent->position.z - ent->boundingBox.size.z)) {
				ent->groundentity = &other;
			}
			//Test in x direction for clip
			vector3d_set(testOrigin, tempOrigin.x, ent->boundingBox.position.y, ent->boundingBox.position.z);
			testBox = aabb(testOrigin, ent->boundingBox.size);
			clippingx = AABBAABB(testBox, other->boundingBox);
			//Test in y direction for clip
			vector3d_set(testOrigin, ent->boundingBox.position.x, tempOrigin.y, ent->boundingBox.position.z);
			testBox = aabb(testOrigin, ent->boundingBox.size);
			clippingy = AABBAABB(testBox, other->boundingBox);
			//Test in z direction for clip
			vector3d_set(testOrigin, ent->boundingBox.position.x, ent->boundingBox.position.y, tempOrigin.z);
			testBox = aabb(testOrigin, ent->boundingBox.size);
			clippingz = AABBAABB(testBox, other->boundingBox);
			/**
			vector3d_copy(temp, ent->boundingBox.position);
			temp.x += ent->boundingBox.size.x;
			if (RaycastAABB(other->boundingBox, ray(temp, vector3d(1, 0, 0))) >= 0.0f) {
				ent->velocity.x = 0.0f;
				clippingx = true;
			}
			else if (RaycastAABB(other->boundingBox, ray(temp, vector3d(-1, 0, 0))) >= 0.0f) {
				ent->velocity.x = 0.0f;
				clippingx = true;
			}
			temp.x -= ent->boundingBox.size.x * 2;
			if (!clippingx && RaycastAABB(other->boundingBox, ray(temp, vector3d(1, 0, 0))) >= 0.0f) {
				ent->velocity.x = 0.0f;
				clippingx = true;
			}
			else if (!clippingx && RaycastAABB(other->boundingBox, ray(temp, vector3d(-1, 0, 0))) >= 0.0f) {
				ent->velocity.x = 0.0f;
				clippingx = true;
			}
			else if (!clippingx&& RaycastAABB(other->boundingBox, ray(ent->boundingBox.position, vector3d(1, 0, 0))) >= 0.0f) {
				ent->velocity.x = 0.0f;
				clippingx = true;
			}
			else if (RaycastAABB(other->boundingBox, ray(ent->boundingBox.position, vector3d(-1, 0, 0))) >= 0.0f) {
				ent->velocity.x = 0.0f;
				clippingx = true;
			}

			vector3d_copy(temp, ent->boundingBox.position);
			temp.y += ent->boundingBox.size.y;
			if (RaycastAABB(other->boundingBox, ray(temp, vector3d(0, 1, 0))) >= 0.0f) {
				ent->velocity.y = 0.0f;
				clippingy = true;
			}
			else if (RaycastAABB(other->boundingBox, ray(temp, vector3d(0, -1, 0))) >= 0.0f) {
				ent->velocity.y = 0.0f;
				clippingy = true;
			}
			temp.y -= ent->boundingBox.size.y * 2;
			if (!clippingy && RaycastAABB(other->boundingBox, ray(temp, vector3d(0, 1, 0))) >= 0.0f) {
				ent->velocity.y = 0.0f;
				clippingy = true;
			}
			else if (!clippingy && RaycastAABB(other->boundingBox, ray(temp, vector3d(0, -1, 0))) >= 0.0f) {
				ent->velocity.y = 0.0f;
				clippingy = true;
			}
			else if (!clippingy && RaycastAABB(other->boundingBox, ray(ent->boundingBox.position, vector3d(0, 1, 0))) >= 0.0f) {
				ent->velocity.y = 0.0f;
				clippingy = true;
			}
			else if (RaycastAABB(other->boundingBox, ray(ent->boundingBox.position, vector3d(0, -1, 0))) >= 0.0f) {
				ent->velocity.y = 0.0f;
				clippingy = true;
			}

			vector3d_copy(temp, ent->boundingBox.position);
			temp.z += ent->boundingBox.size.z;
			if (RaycastAABB(other->boundingBox, ray(temp, vector3d(0, 0, 1))) >= 0.0f) {
				ent->velocity.z = 0.0f;
				clippingz = true;
			}
			else if (RaycastAABB(other->boundingBox, ray(temp, vector3d(0, 0, -1))) >= 0.0f) {
				ent->velocity.z = 0.0f;
				clippingz = true;
			}
			temp.z -= ent->boundingBox.size.z * 2;
			if (!clippingz && RaycastAABB(other->boundingBox, ray(temp, vector3d(0, 0, 1))) >= 0.0f) {
				ent->velocity.z = 0.0f;
				clippingz = true;
			}
			else if (!clippingz && RaycastAABB(other->boundingBox, ray(temp, vector3d(0, 0, -1))) >= 0.0f) {
				ent->velocity.z = 0.0f;
				clippingz = true;
			}
			else if (!clippingz && RaycastAABB(other->boundingBox, ray(ent->boundingBox.position, vector3d(0, 0, 1))) >= 0.0f) {
				ent->velocity.z = 0.0f;
				clippingz = true;
			}
			else if (!clippingz && RaycastAABB(other->boundingBox, ray(ent->boundingBox.position, vector3d(0, 0, -1))) >= 0.0f) {
				ent->velocity.z = 0.0f;
				clippingz = true;
			}
			*/
			Impact(&ent, &other);
			//break;
		}
		j++;
	}
	//Verify not clipping
	
	if (!clippingx) {
		//Update position
		ent->position.x += tempVel.x;
		//vector3d_add(ent->position, ent->position, tempVel);
		//sync matrix and bbox positions
		ent->boundingBox.position.x = ent->position.x;
		//vector3d_copy(ent->boundingBox.position, ent->position);
		ent->modelMat[3][0] = ent->position.x;
		//ent->modelMat[3][1] = ent->position.y;
		//ent->modelMat[3][2] = ent->position.z;
	}
	else {
		ent->velocity.x = 0.0f;
	}
	if (!clippingy) {
		//Update position
		ent->position.y += tempVel.y;
		//vector3d_add(ent->position, ent->position, tempVel);
		//sync matrix and bbox positions
		ent->boundingBox.position.y = ent->position.y;
		//vector3d_copy(ent->boundingBox.position, ent->position);
		ent->modelMat[3][1] = ent->position.y;
		//ent->modelMat[3][1] = ent->position.y;
		//ent->modelMat[3][2] = ent->position.z;
		if (clipping) {
			slog("%f", RaycastAABB(other->boundingBox, ray(ent->boundingBox.position, vector3d(0, 1, 0))));
			slog("%f", RaycastAABB(other->boundingBox, ray(ent->boundingBox.position, vector3d(0, -1, 0))));
		}
	}
	else {
		ent->velocity.y = 0.0f;
	}
	if (!clippingz) {
		//Update position
		ent->position.z += tempVel.z;
		//vector3d_add(ent->position, ent->position, tempVel);
		//sync matrix and bbox positions
		ent->boundingBox.position.z = ent->position.z;
		//vector3d_copy(ent->boundingBox.position, ent->position);
		ent->modelMat[3][2] = ent->position.z;
		//ent->modelMat[3][1] = ent->position.y;
		//ent->modelMat[3][2] = ent->position.z;
	}
	else {
		ent->velocity.z = 0.0f;
	}
	run_think(ent);
}

void physics_toss(Entity_T* ent) {
	//toss physics
	run_think(ent);
}

void Friction(Entity_T* ent)
{
	float* vel;
	float	speed, newspeed, control;
	float	friction;
	float	drop;

	vel = ent->velocity.asArray;

	speed = sqrt(vel[0] * vel[0] + vel[1] * vel[1] + vel[2] * vel[2]);
	if (speed < 1)
	{
		vel[0] = 0;
		vel[1] = 0;
		return;
	}

	drop = 0;

	// apply ground friction
	if (ent->groundentity)
	{
		friction = pm_friction;
		control = speed < pm_stopspeed ? pm_stopspeed : speed;
		drop += control * friction * FRAMETIME;
	}

	newspeed = speed - drop;
	if (newspeed < 0)
	{
		newspeed = 0;
	}
	newspeed /= speed;

	vel[0] = vel[0] * newspeed;
	vel[1] = vel[1] * newspeed;
	vel[2] = vel[2] * newspeed;
}

void Accelerate(Entity_T* ent, Vector3D wishdir, float wishspeed, float accel)
{
	int			i;
	float		addspeed, accelspeed, currentspeed;

	currentspeed = vector3d_dot_product(ent->velocity, wishdir);
	addspeed = wishspeed - currentspeed;
	if (addspeed <= 0)
		return;
	accelspeed = accel * FRAMETIME * wishspeed;
	if (accelspeed > addspeed)
		accelspeed = addspeed;

	for (i = 0; i < 3; i++)
		ent->velocity.asArray[i] += accelspeed * wishdir.asArray[i];
}
 /**
void move(Entity_T* ent, Vector3D wishdir, float accel) {
	int			i;
	Vector3D	wishvel;
	float		fmove, smove;
	float		wishspeed;
	float		maxspeed;

	fmove = pm->cmd.forwardmove;
	smove = pm->cmd.sidemove;

	for (i = 0; i < 2; i++)
		wishvel[i] = pml.forward[i] * fmove + pml.right[i] * smove;
	wishvel[2] = 0;

	VectorCopy(wishvel, wishdir);
	wishspeed = vector3d_normalize(&wishdir);

	//
	// clamp to server defined max speed
	//
	maxspeed = (pm->s.pm_flags & PMF_DUCKED) ? pm_duckspeed : pm_maxspeed;

	if (wishspeed > maxspeed)
	{
		VectorScale(wishvel, maxspeed / wishspeed, wishvel);
		wishspeed = maxspeed;
	}

	if (pml.ladder)
	{
		PM_Accelerate(wishdir, wishspeed, pm_accelerate);
		if (!wishvel[2])
		{
			if (pml.velocity[2] > 0)
			{
				pml.velocity[2] -= pm->s.gravity * pml.frametime;
				if (pml.velocity[2] < 0)
					pml.velocity[2] = 0;
			}
			else
			{
				pml.velocity[2] += pm->s.gravity * pml.frametime;
				if (pml.velocity[2] > 0)
					pml.velocity[2] = 0;
			}
		}
		PM_StepSlideMove();
	}
	else if (pm->groundentity)
	{	// walking on ground
		pml.velocity[2] = 0; //!!! this is before the accel
		PM_Accelerate(wishdir, wishspeed, pm_accelerate);

		// PGM	-- fix for negative trigger_gravity fields
		//		pml.velocity[2] = 0;
		if (pm->s.gravity > 0)
			pml.velocity[2] = 0;
		else
			pml.velocity[2] -= pm->s.gravity * pml.frametime;
		// PGM

		if (!pml.velocity[0] && !pml.velocity[1])
			return;
		PM_StepSlideMove();
	}
	else
	{	// not on ground, so little effect on velocity
		if (pm_airaccelerate)
			PM_AirAccelerate(wishdir, wishspeed, pm_accelerate);
		else
			PM_Accelerate(wishdir, wishspeed, 1);
		// add gravity
		pml.velocity[2] -= pm->s.gravity * pml.frametime;
		PM_StepSlideMove();
	}
}
*/

void update_physics_positions() {
	int i = 0;
	int j = 0;
	Vector3D tempAccel;
	Vector3D tempOrigin;
	Vector3D tempVel;
	AABB tempBox;
	Entity_T* ent;
	Entity_T* other;
	Bool clipping;
	float vel, speed;
	if (!entity_list) {
		slog("Tried to find entity before initializing entity list!");
	}
	while (i < gf3d_entity_manager.entity_max) {
		clipping = false;
		j = 0;
		ent = &entity_list[i];
		vector3d_clear(tempAccel);
		vector3d_clear(tempOrigin);
		vector3d_clear(tempVel);
		if (ent->_inuse == 0) {
			i++;
			continue;
		}
		if (ent->movetype == MOVETYPE_NONE) {
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
	/**
	if (e1->touch)
		e1->touch(e1, e2);

	if (e2->touch)
		e2->touch(e2, e1);
		*/
	slog("%s touched %s", e1->name, e2->name);
}

void teleport_entity(Entity_T* ent, Vector3D distance)
{
	//matrix translate
	gfc_matrix_make_translation(ent->modelMat,distance);
	//position translate
	vector3d_add(ent->position, ent->position, distance);
	//bounding box translate
	ent->boundingBox.position = ent->position;
}
