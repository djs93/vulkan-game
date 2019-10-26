#ifndef __PHYSICS_H__
#define __PHYSICS_H__

/**
* @brief updates all entities' positions
*/
void update_physics_positions();

void physics_none(Entity_T* ent);
void physics_step(Entity_T* ent);

void Impact(Entity_T* e1, Entity_T* e2);

void teleport_entity(Entity_T* ent, Vector3D position);
#endif
