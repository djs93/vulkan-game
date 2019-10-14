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
void run_entity(Entity* ent);
#endif
