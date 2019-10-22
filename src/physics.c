#include "linmath.h"
#include "gfc_matrix.h"
#include "gf3d_entity.h"
#include "simple_logger.h"
#include "local.h"

void physics_none(Entity_T* ent);

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
	//step physics
	run_think(ent);
}

void physics_toss(Entity_T* ent) {
	//toss physics
	run_think(ent);
}

void update_physics_positions() {
	int i = 0;
	if (!get_entity_manager().entity_list) {
		slog("Tried to find entity before initializing entity list!");
	}
	while (i < gf3d_entity_manager.entity_max) {
		if (entity_list[i]._inuse == 0) {
			break;
		}
		i++;
	}
}