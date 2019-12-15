#ifndef __GF3D_ENTITY_H__
#define __GF3D_ENTITY_H__

#include "gf3d_model.h"
#include "local.h"
#include "Geometry3D.h"

typedef enum
{
    ES_Idle = 0,
    ES_Dying = 1,
    ES_Dead = 2
}EntityState;

typedef struct Entity_S
{
	char*			name;			/**<name of the entity*/
	char*			type;			/**Type of the entitiy*/
    int           _inuse;         /**<flag to keep track if this isntance is in use and should not be reassigned*/
    Model          *model;          /**<the 3d model for this entity*/
	Matrix4			modelMat;		/**<the model matrix*/
    Vector3D         position;       /**<DO NOT DIRECTLY MODIFY - position of the entity in 3d space*/
    Vector3D         velocity;       /**<velocity of the entity in 3d space*/
    Vector3D         acceleration;   /**<acceleration of the entity in 3d space*/
    Vector3D         rotation;       /**<yaw, pitch, and roll of the entity*/
    Vector3D         scale;          /**<*please default to 1,1,1*/
    EntityState     state;          /**<current state of the entity*/
    void (*prethink)(struct Entity_S* self);   /**<function called before entity think*/
    void (*think)(struct Entity_S* self);   /**<function called on entity think*/
    void (*touch)(struct Entity_S* self,struct Entity_S* other);   /**<function called when an entity touches another*/
    void (*die)(struct Entity_S* self);   /**<function called when an entity dies*/
    float           health;
    float           healthmax;
	int movetype;					/**<type of movement*/
	float nextthink;
	int frame;
	Vector3D origin;
	Vector3D old_origin;
	Entity_T* groundentity;
	int linkcount;
	int groundentity_linkcount;

	int flags;
	int svflags;

	AABB boundingBox;
	Vector3D maxspeed;

	float specFloat1;		/**<used for jumpheight in player*/
    void *data;                     /**<additional entity specific data*/
    void *data2;                     /**<additional entity specific data*/
    
}Entity;

typedef struct
{
	Entity_T* entity_list;
	Uint32  entity_max;
	Uint32	num_ents;
}EntityManager;

static EntityManager gf3d_entity_manager = { 0, ENTITY_MAX };
/**
 * @brief initializes the entity subsystem
 * @param entity_max maximum number of simultaneous entities you wish to support
 */
void gf3d_entity_manager_init(Uint32 entity_max);

/**
 * @brief get an empty entity from the system
 * @return NULL on out of space or a pointer to an entity otherwise
 */
Entity *gf3d_entity_new();

/**
 * @brief free an active entity
 * @param self the entity to free
 */
void    gf3d_entity_free(Entity_T *self);

/**
* @brief get the index of the named entity
* @param name the name of the entity to find
* @return the index of the entity in the entity list
*/
Entity_T* find_entity(char* name);
Entity_T* get_last_entity();

void rotate_entity(Entity_T* entity, float radians, Vector3D axis);

Entity_T* modeled_entity_animated(char* modelName, char* entityName, int startFrame, int numFrames);
Entity_T* modeled_entity(char* modelName, char* entityName);

Vector3D getAngles(Matrix4 mat);

Entity_T* load_entity_json(char* entityName);

void save_entity_layout_json(Entity_T* entity);

Entity_T* gf3d_nonanimated_entity_copy(Entity_T* entity);

#endif
