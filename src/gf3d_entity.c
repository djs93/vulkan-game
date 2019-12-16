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

void gf3d_entity_free_all()
{
	int i;
	for (i = 0; i < gf3d_entity_manager.entity_max; i++)
	{
		memset(&entity_list[i], 0, sizeof(Entity_T));
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
		if (from->_inuse != 1 || !from->type || !from->name)
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

Entity_T* load_entity_json(char * entityType)
{
	Entity_T* result; 
	char* fileName = malloc(strlen("mobs/") + strlen(entityType) + strlen(".json") + 1);
	strcpy(fileName, "mobs/");
	strcat(fileName, entityType);
	strcat(fileName, ".json");
	SJson* file = sj_load(fileName);
	if (!file)return NULL;
	SJson* modelFileObj = sj_object_get_value(file, "modelFile");
	char* modelFileChar = sj_get_string_value(modelFileObj);
	SJson* animatedObj = sj_object_get_value(file, "animated");
	char* animatedChar = sj_get_string_value(animatedObj);
	if (strcmp(animatedChar, "true")==0) {
		SJson* startFrameObj = sj_object_get_value(file, "startFrame");
		int startFrame;
		sj_get_integer_value(startFrameObj, &startFrame);
		SJson* endFrameObj = sj_object_get_value(file, "endFrame");
		int endFrame;
		sj_get_integer_value(endFrameObj, &endFrame);
		result = modeled_entity_animated(modelFileChar, NULL, startFrame, endFrame);
	}
	else {
		result = modeled_entity(modelFileChar, NULL);
	}
	if (!result) {
		slog("could not load %s from json!", modelFileChar);
		return NULL;
	}
	result->name = sj_get_string_value(sj_object_get_value(file, "defaultname"));
	result->type = sj_get_string_value(sj_object_get_value(file, "type"));

	float* health = malloc(sizeof(float));
	
	sj_get_float_value(sj_object_get_value(file, "health"), health);
	if (health)result->health = *health;
	else result->health = 1.0f;

	float* maxHealth = malloc(sizeof(float));
	sj_get_float_value(sj_object_get_value(file, "healthmax"), maxHealth);
	if (maxHealth)result->healthmax = *maxHealth;
	else result->healthmax = 1.0f;

	int* moveType = malloc(sizeof(int));
	sj_get_integer_value(sj_object_get_value(file, "movetype"), moveType);
	if (moveType)result->movetype = *moveType;
	else result->movetype = MOVETYPE_NONE;

	float* nextthink = malloc(sizeof(float));
	sj_get_float_value(sj_object_get_value(file, "nextthink"), nextthink);
	if (nextthink)result->nextthink = *nextthink;
	else result->nextthink = 0.0f;

	int* frame = malloc(sizeof(int));
	sj_get_integer_value(sj_object_get_value(file, "frame"), frame);
	if (frame)result->frame = *frame;
	else result->frame = 1;

	int* flags = malloc(sizeof(int));
	sj_get_integer_value(sj_object_get_value(file, "flags"), flags);
	if (flags)result->flags = *flags;
	else result->flags = 0;

	int* svflags = malloc(sizeof(int));
	sj_get_integer_value(sj_object_get_value(file, "svflags"), svflags);
	if (svflags)result->svflags = *svflags;
	else result->svflags = 0;

	SJson* aabbSizeArray = sj_object_get_value(file, "AABBSize");
	float* x = malloc(sizeof(float));
	float* y = malloc(sizeof(float));
	float* z = malloc(sizeof(float));
	sj_get_float_value(sj_array_get_nth(aabbSizeArray, 0), x);
	sj_get_float_value(sj_array_get_nth(aabbSizeArray, 1), y);
	sj_get_float_value(sj_array_get_nth(aabbSizeArray, 2), z);
	result->boundingBox.size.x = *x;
	result->boundingBox.size.y = *y;
	result->boundingBox.size.z = *z;

	SJson* aabbOffsetArray = sj_object_get_value(file, "AABBAdjustments");
	sj_get_float_value(sj_array_get_nth(aabbOffsetArray, 0), x);
	sj_get_float_value(sj_array_get_nth(aabbOffsetArray, 1), y);
	sj_get_float_value(sj_array_get_nth(aabbOffsetArray, 2), z);
	result->model->boudningAdjustment.x = *x;
	result->model->boudningAdjustment.y = *y;
	result->model->boudningAdjustment.z = *z;

	SJson* maxSpeedArray = sj_object_get_value(file, "maxspeed");
	sj_get_float_value(sj_array_get_nth(maxSpeedArray, 0), x);
	sj_get_float_value(sj_array_get_nth(maxSpeedArray, 1), y);
	sj_get_float_value(sj_array_get_nth(maxSpeedArray, 2), z);
	result->maxspeed.x = *x;
	result->maxspeed.y = *y;
	result->maxspeed.z = *z;

	float* specfloat = malloc(sizeof(float));
	sj_get_float_value(sj_object_get_value(file, "specFloat1"), specfloat);
	if (specfloat)result->specFloat1 = *specfloat;
	else result->specFloat1 = 0.0f;

	Entity_T* test = find_entity("player");
	if (strcmp(entityType, "player")==0&&test) {
		result->data = test->data;
		result->data2 = test->data2;
		result->specFloat1 = test->specFloat1;
		result->health = test->health;
		result->healthmax = test->healthmax;
	}
	else if (strcmp(entityType, "pacer")==0) {
		Vector3D temp = vector3d(0, -10, 0);
		result->data2 = &temp;
		result->acceleration = temp;
	}

	free(health);
	free(maxHealth);
	free(moveType);
	free(nextthink);
	free(frame);
	free(flags);
	free(svflags);
	free(x);
	free(y);
	free(z);
	free(specfloat);

	return result;
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
		SJson* animated = NULL;
		SJson* startFrame = NULL;
		SJson* endFrame = NULL;
		if (entity->model->frameCount > 1) {
			animated = sj_new_bool(1);
			startFrame = sj_new_int(entity->model->startFrame);
			endFrame = sj_new_int(entity->model->endFrame);
		}
		else {
			animated = sj_new_bool(0);
		}
		sj_object_insert(file, "animated", animated);
		if (startFrame) {
			sj_object_insert(file, "startFrame", startFrame);
		}
		if (endFrame) {
			sj_object_insert(file, "endFrame", endFrame);
		}
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

SJson* save_entity_content_editor_json(Entity_T* entity) {
	SJson* entJson = sj_object_new();
	SJson* positionArray = sj_array_new();
	sj_object_insert(entJson, "type", sj_string_to_value(sj_string_new_text(entity->type)));
	sj_array_append(positionArray, sj_new_float(entity->position.x));
	sj_array_append(positionArray, sj_new_float(entity->position.y));
	sj_array_append(positionArray, sj_new_float(entity->position.z));
	sj_object_insert(entJson, "position", positionArray);

	return entJson;
}

void save_all_content_editor() {
	Entity_T* ent;
	int i;
	SJson* saveFile = sj_object_new();
	SJson* entArray = sj_array_new();
	for (i = 0; i < gf3d_entity_manager.entity_max; i++)
	{
		if (!entity_list[i]._inuse)continue;
		ent = &entity_list[i];
		if (!ent->name || (strcmp(ent->name, "copied entity") != 0 && strcmp(ent->name, "player") != 0))continue;
		sj_array_append(entArray, save_entity_content_editor_json(ent));
	}
	sj_object_insert(saveFile, "entities", entArray);
	sj_object_insert(saveFile, "loadNext", sj_new_str("save.json"));
	sj_save(saveFile, "save.json");
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

int get_type_count(char* type) {
	Entity_T* ent = NULL;
	int i;
	int count = 0;
	for (i = 0; i < gf3d_entity_manager.entity_max; i++)
	{
		if (!entity_list[i]._inuse)continue;
		ent = &entity_list[i];
		if (strcmp(ent->type, type) == 0) {
			count++;
		}
	}
	return count;
}

float getLowestPoint() {
	float result = 100000.0f;
	int i;
	for (i = 0; i < gf3d_entity_manager.entity_max; i++)
	{
		Entity_T* ent = &entity_list[i];
		if (!ent->_inuse)continue;
		if (!(strcmp(ent->type, "ground") == 0 || strcmp(ent->type, "platform") == 0))continue;
		if (ent->position.z < result) {
			result = ent->position.z;
		}
	}
	return result;
}
/*eol@eof*/
