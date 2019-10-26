#include <stdlib.h>
#include <string.h>

#include "simple_logger.h"

#include "gf3d_entity.h"


Vector3D getAngles(Matrix4 mat);
void gf3d_entity_manager_close()
{
    if(entity_list != NULL)
    {
        free(entity_list);
    }
    memset(&gf3d_entity_manager,0,sizeof(EntityManager));
}

void gf3d_entity_manager_init(Uint32 entity_max)
{
	gf3d_entity_manager.entity_max = entity_max;
    entity_list = (Entity_T*)gfc_allocate_array(sizeof(Entity_T),entity_max);
    if (!entity_list)
    {
        slog("failed to allocate entity list");
        return;
    }
    atexit(gf3d_entity_manager_close);
}

Entity_T *gf3d_entity_new()
{
    Entity_T *ent = NULL;
    int i;
    for (i = 0; i < gf3d_entity_manager.entity_max; i++)
    {
        if (entity_list[i]._inuse)continue;
        //. found a free entity
        memset(&entity_list[i],0,sizeof(Entity_T));
		ent = &entity_list[i];
        ent->_inuse = 1;
		gf3d_entity_manager.num_ents++;
		ent->movetype = MOVETYPE_NONE;
		ent->touch = NULL;
        return &entity_list[i];
    }
    slog("request for entity failed: all full up");
    return NULL;
}

void gf3d_entity_free(Entity_T *self)
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

Entity_T* find_entity(char* name) {
	Entity_T* from = entity_list;
	if (!entity_list) {
		slog("Tried to find entity before initializing entity list!");
		return -1;
	}
	for (; from < &entity_list[gf3d_entity_manager.num_ents]; from++)
	{
		if (!from->_inuse)
			continue;
		if (!strcmp(from->name, name))
			return from;
	}
	return -1;
}

EntityManager get_entity_manager() {
	return gf3d_entity_manager;
}

Entity_T* modeled_entity_animated(char* modelName, char* entityName, int startFrame, int numFrames)
{
	Entity_T* ent = gf3d_entity_new();
	if (!ent) {
		return NULL;
	}
	Model* model = gf3d_model_load_animated(modelName, startFrame, numFrames);
	if (!model) {
		slog("Could not load animated model %s", modelName);
		return NULL;
	}
	ent->model = model;
	Matrix4 modelMat;
	gfc_matrix_identity(modelMat);
	gfc_matrix_copy(ent->modelMat, modelMat);
	ent->boundingBox.size = *ent->model->extents[0];
	ent->name = entityName;
	return ent;
}

Entity_T* modeled_entity(char* modelName, char* entityName)
{
	Entity_T* ent = gf3d_entity_new();
	if (!ent) {
		return NULL;
	}
	Model* model = gf3d_model_load(modelName);
	if (!model) {
		slog("Could not load model %s", modelName);
		return NULL;
	}
	ent->model = model;
	Matrix4 modelMat;
	gfc_matrix_identity(modelMat);
	gfc_matrix_copy(ent->modelMat, modelMat);
	ent->boundingBox.size = *ent->model->extents[0];
	ent->name = entityName;
	return ent;
}

void rotate_entity(Entity_T* entity, float radians, Vector3D axis) {
	if (!entity->modelMat) {
		slog("No model matrix for entity %s", entity->name);
	}
	gfc_matrix_rotate(entity->modelMat, entity->modelMat, radians, axis);
	vector3d_copy(entity->rotation,getAngles(entity->modelMat));
}

Vector3D getAngles(Matrix4 mat) {
	Vector3D res;
	float sy = sqrt(pow(mat[0][0], 2) + pow(mat[1][0], 2));

	Bool singular = sy < 0.000001f;
	if (!singular) {
		res.x = atan2(mat[2][1], mat[2][2]) * GFC_RADTODEG;
		res.y = atan2(-mat[2][0], sy) * GFC_RADTODEG;
		res.z = atan2(mat[1][0], mat[0][0]) * GFC_RADTODEG;
	}
	else {
		res.x = atan2(-mat[1][2], mat[1][1]) * GFC_RADTODEG;
		res.y = atan2(-mat[2][0], sy) * GFC_RADTODEG;
		res.z = 0;
	}
	return res;
}


/*eol@eof*/
