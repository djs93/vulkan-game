#include "linmath.h"
#include "gfc_matrix.h"
#include "gf3d_entity.h"
#include "simple_logger.h"
#include "local.h"

void run_entity(Entity* ent)
{
	if (ent->prethink)
		ent->prethink(ent);

	switch ((int)ent->movetype)
	{
	case MOVETYPE_PUSH:
	case MOVETYPE_STOP:
		physics_pusher(ent);
		break;
	case MOVETYPE_NONE:
		physcis_none(ent);
		break;
	case MOVETYPE_NOCLIP:
		physics_noclip(ent);
		break;
	case MOVETYPE_STEP:
		physics_step(ent);
		break;
	case MOVETYPE_TOSS:
	case MOVETYPE_BOUNCE:
	case MOVETYPE_FLY:
	case MOVETYPE_FLYMISSILE:
		physics_toss(ent);
		break;
	default:
		slog("Bad movetype %i", (int)ent->movetype);
	}
}

void run_think(Entity* ent) {
	float	thinktime;

	thinktime = ent->nextthink;
	if (thinktime <= 0)
		return true;
	if (thinktime > level.time + 0.001)
		return true;

	ent->nextthink = 0;
	if (!ent->think)
		gi.error("NULL ent->think");
	ent->think(ent);

	return false;
}

void physics_pusher(Entity* ent) {

}

void physics_none(Entity* ent) {
	// regular thinking
	run_think(ent);
}

void physics_noclip(Entity* ent) {

}

void physics_step(Entity* ent) {

}

void physics_toss(Entity* ent) {

}

void update_physics_positions() {
	int i = 0;
	if (!get_entity_manager().entity_list) {
		slog("Tried to find entity before initializing entity list!");
	}
	while (i < gf3d_entity_manager.entity_max) {
		if (gf3d_entity_manager.entity_list[i]._inuse == 0) {
			break;
		}
		i++;
	}
}