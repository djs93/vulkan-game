#include <stdlib.h>
#include <string.h>

#include "simple_logger.h"

#include "gf3d_entity.h"



void gf3d_entity_manager_close()
{
    if(gf3d_entity_manager.entity_list != NULL)
    {
        free(gf3d_entity_manager.entity_list);
    }
    memset(&gf3d_entity_manager,0,sizeof(EntityManager));
}

void gf3d_entity_manager_init(Uint32 entity_max)
{
	gf3d_entity_manager.entity_max = entity_max;
    gf3d_entity_manager.entity_list = (Entity*)gfc_allocate_array(sizeof(Entity),entity_max);
    if (!gf3d_entity_manager.entity_list)
    {
        slog("failed to allocate entity list");
        return;
    }
    atexit(gf3d_entity_manager_close);
}

Entity *gf3d_entity_new()
{
    Entity *ent = NULL;
    int i;
    for (i = 0; i < gf3d_entity_manager.entity_max; i++)
    {
        if (gf3d_entity_manager.entity_list[i]._inuse)continue;
        //. found a free entity
        memset(&gf3d_entity_manager.entity_list[i],0,sizeof(Entity));
        gf3d_entity_manager.entity_list[i]._inuse = 1;
        return &gf3d_entity_manager.entity_list[i];
    }
    slog("request for entity failed: all full up");
    return NULL;
}

void gf3d_entity_free(Entity *self)
{
    if (!self)
    {
        slog("self pointer is not valid");
        return;
    }
    self->_inuse = 0;
    gf3d_model_free(self->model);
    if (self->data != NULL)
    {
        slog("warning: data not freed at entity free!");
    }
}

int get_entity(char* name) {
	int i = 0;
	if (!gf3d_entity_manager.entity_list) {
		slog("Tried to find entity before initializing entity list!");
		return -1;
	}
	while (i < gf3d_entity_manager.entity_max) {
		if (gf3d_entity_manager.entity_list[i]._inuse == 0) {
			break;
		}
		if (strcmp(gf3d_entity_manager.entity_list[i].name, name)==0) {
			return i;
		}
		i++;
	}
	return -1;
}

EntityManager get_entity_manager() {
	return gf3d_entity_manager;
}

void rotate_entity(Entity* entity, float radians, Vector3D axis) {
	if (!entity->modelMat) {
		slog("No model matrix for entity %s", entity->name);
	}
	gfc_matrix_rotate(entity->modelMat, entity->modelMat, radians, axis);
}


/*eol@eof*/
