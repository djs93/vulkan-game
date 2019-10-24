#ifndef __PHYSICS_H__
#define __PHYSICS_H__

/**
* @brief updates all entities' positions
*/
void update_physics_positions();

/**
* @brief run an entity's physics, prethink, and think functions
* @param ent the entity to run
*/
void run_entity(Entity_T* ent);

void physics_none(Entity_T* ent);
void physics_pusher(Entity_T* ent);
void physics_noclip(Entity_T* ent);
void physics_step(Entity_T* ent);
void physics_toss(Entity_T* ent);

void Friction(Entity_T* ent);
void Accelerate(Entity_T* ent, Vector3D wishdir, float wishspeed, float accel);

void Impact(Entity_T* e1, Entity_T* e2);
#endif
