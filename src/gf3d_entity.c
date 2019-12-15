#include <stdlib.h>
#include <string.h>

#include "simple_logger.h"
#include "simple_json.h"

#include "gf3d_entity.h"


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
		return NULL;
	}
	for (; from < &entity_list[gf3d_entity_manager.num_ents]; from++)
	{
		if (!from->_inuse)
			continue;
		if (!strcmp(from->name, name))
			return from;
	}
	return NULL;
}

Entity_T* get_last_entity() {
	Entity_T* ent = NULL;
	int i;
	for (i = 0; i < gf3d_entity_manager.entity_max; i++)
	{
		if (!entity_list[i]._inuse)continue;
		ent = &entity_list[i];
	}
	return ent;
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
		res.x = atan2(mat[2][1], mat[2][2]);
		res.y = atan2(-mat[2][0], sy);
		res.z = atan2(mat[1][0], mat[0][0]);
	}
	else {
		res.x = atan2(-mat[1][2], mat[1][1]);
		res.y = atan2(-mat[2][0], sy);
		res.z = 0;
	}
	if (res.x < 0.0f) {
		res.x += GFC_2PI;
	}
	if (res.y < 0.0f) {
		res.y += GFC_2PI;
	}
	if (res.z < 0.0f) {
		res.z += GFC_2PI;
	}
	return res;
}

Entity_T* load_entity_json(char* entityName)
{
	return NULL;
}

//Helper function to save entity data into a loadable format
void save_entity_layout_json(Entity_T* entity)
{
	SJson* file = sj_object_new();
	if (entity->name) {
		SJson* name = sj_new_str(entity->name);
		sj_object_insert(file, "defaultname", name);
	}

	if (entity->type) {
		SJson* type = sj_new_str(entity->type);
		sj_object_insert(file, "type", type);
	}

	/*
	SJson* inuse = sj_new_int(entity->_inuse);
	sj_object_insert(file, "_inuse", inuse);
	*/

	if (entity->model) {
		SJson* modelName = sj_new_str(entity->model->filename);
		sj_object_insert(file, "modelFile", modelName);
	}

	/*
	SJson* matArray = sj_array_new();
	int i,j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			sj_array_append(matArray, sj_new_float(entity->modelMat[i][j]));
		}
	}
	sj_object_insert(file, "modelMat", matArray);
	

	SJson* position = sj_array_new();
	sj_array_append(position, sj_new_float(entity->position.x));
	sj_array_append(position, sj_new_float(entity->position.y));
	sj_array_append(position, sj_new_float(entity->position.z));
	sj_object_insert(file, "position", position);

	SJson* velocity = sj_array_new();
	sj_array_append(velocity, sj_new_float(entity->velocity.x));
	sj_array_append(velocity, sj_new_float(entity->velocity.y));
	sj_array_append(velocity, sj_new_float(entity->velocity.z));
	sj_object_insert(file, "velocity", velocity);

	SJson* acceleration = sj_array_new();
	sj_array_append(acceleration, sj_new_float(entity->acceleration.x));
	sj_array_append(acceleration, sj_new_float(entity->acceleration.y));
	sj_array_append(acceleration, sj_new_float(entity->acceleration.z));
	sj_object_insert(file, "acceleration", acceleration);

	SJson* rotation = sj_array_new();
	sj_array_append(rotation, sj_new_float(entity->rotation.x));
	sj_array_append(rotation, sj_new_float(entity->rotation.y));
	sj_array_append(rotation, sj_new_float(entity->rotation.z));
	sj_object_insert(file, "rotation", rotation);

	SJson* scale = sj_array_new();
	sj_array_append(scale, sj_new_float(entity->scale.x));
	sj_array_append(scale, sj_new_float(entity->scale.y));
	sj_array_append(scale, sj_new_float(entity->scale.z));
	sj_object_insert(file, "scale", scale);

	SJson* state = sj_new_int(entity->state);
	sj_object_insert(file, "state", state);
	*/

	SJson* health = sj_new_float(entity->health);
	sj_object_insert(file, "health", health);

	SJson* healthMax = sj_new_float(entity->healthmax);
	sj_object_insert(file, "healthmax", healthMax);

	SJson* movetype = sj_new_int(entity->movetype);
	sj_object_insert(file, "movetype", movetype);

	SJson* nextthink = sj_new_float(entity->nextthink);
	sj_object_insert(file, "nextthink", nextthink);

	SJson* frame = sj_new_int(entity->frame);
	sj_object_insert(file, "frame", frame);

	/*
	SJson* origin = sj_array_new();
	sj_array_append(origin, sj_new_float(entity->origin.x));
	sj_array_append(origin, sj_new_float(entity->origin.y));
	sj_array_append(origin, sj_new_float(entity->origin.z));
	sj_object_insert(file, "origin", origin);

	SJson* old_origin = sj_array_new();
	sj_array_append(old_origin, sj_new_float(entity->old_origin.x));
	sj_array_append(old_origin, sj_new_float(entity->old_origin.y));
	sj_array_append(old_origin, sj_new_float(entity->old_origin.z));
	sj_object_insert(file, "oldorigin", old_origin);
	*/

	SJson* flags = sj_new_int(entity->flags);
	sj_object_insert(file, "flags", flags);

	SJson* svflags = sj_new_int(entity->svflags);
	sj_object_insert(file, "svflags", svflags);

	/*
	SJson* AABBPos = sj_array_new();
	sj_array_append(AABBPos, sj_new_float(entity->boundingBox.position.x));
	sj_array_append(AABBPos, sj_new_float(entity->boundingBox.position.y));
	sj_array_append(AABBPos, sj_new_float(entity->boundingBox.position.z));
	sj_object_insert(file, "AABBPos", AABBPos);
	*/

	SJson* AABBSize = sj_array_new();
	sj_array_append(AABBSize, sj_new_float(entity->boundingBox.size.x));
	sj_array_append(AABBSize, sj_new_float(entity->boundingBox.size.y));
	sj_array_append(AABBSize, sj_new_float(entity->boundingBox.size.z));
	sj_object_insert(file, "AABBSize", AABBSize);

	SJson* AABBAdjustments = sj_array_new();
	sj_array_append(AABBAdjustments, sj_new_float(entity->model->boudningAdjustment.x));
	sj_array_append(AABBAdjustments, sj_new_float(entity->model->boudningAdjustment.y));
	sj_array_append(AABBAdjustments, sj_new_float(entity->model->boudningAdjustment.z));
	sj_object_insert(file, "AABBAdjustments", AABBAdjustments);

	SJson* maxspeed = sj_array_new();
	sj_array_append(maxspeed, sj_new_float(entity->maxspeed.x));
	sj_array_append(maxspeed, sj_new_float(entity->maxspeed.y));
	sj_array_append(maxspeed, sj_new_float(entity->maxspeed.z));
	sj_object_insert(file, "maxspeed", maxspeed);

	SJson* specFloat1 = sj_new_float(entity->specFloat1);
	sj_object_insert(file, "specFloat1", specFloat1);

	//be sure to get special cases like think functions and additional data per ent-specific load!
	char* fileName = malloc(strlen("mobs/")+strlen(entity->type)+ strlen(".json") +1);
	strcpy(fileName, "mobs/");
	strcat(fileName, entity->type);
	strcat(fileName, ".json");
	sj_save(file, fileName);
	free(fileName);
}

Entity_T* gf3d_nonanimated_entity_copy(Entity_T* entity)
{
	Entity_T* res = gf3d_entity_new();
	res->model = gf3d_model_load(entity->model->filename);
	res->model->texture = entity->model->texture;
	res->type = entity->type;
	if (strcmp(res->type, "player") == 0) {
		res->name = "player";
	}
	else {
		res->name = "copied entity";
	}
	Matrix4 modelMat;
	gfc_matrix_identity(modelMat);
	gfc_matrix_copy(res->modelMat, modelMat);
	return res;
}




/*eol@eof*/
