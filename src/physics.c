#include "linmath.h"
#include "gfc_matrix.h"
#include "gf3d_entity.h"
#include "simple_logger.h"

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