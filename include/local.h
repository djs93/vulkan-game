
#ifndef __LOCAL_H__
#define __LOCAL_H__

#define FRAMETIME 0.0001

#define FL_FLY 0x00000001
#define FLT_EPSILON 1.192092896e-07F 
#define DEG2RAD 0.01745329251994329576923690768489f

#include "game.h"

extern Entity_T* entity_list;
// edict->movetype values
typedef enum
{
	MOVETYPE_NONE,			// never moves
	MOVETYPE_NOCLIP,		// origin and angles change with no interaction
	MOVETYPE_PUSH,			// no clip to world, push on box contact
	MOVETYPE_STOP,			// no clip to world, stops on box contact
	MOVETYPE_WALK,			// gravity
	MOVETYPE_STEP,			// gravity, special edge handling
	MOVETYPE_FLY,
	MOVETYPE_TOSS,			// gravity
	MOVETYPE_FLYMISSILE,	// extra size to monsters
	MOVETYPE_BOUNCE
} movetype;

// changes as each map is entered
typedef struct {
	int framenum;
	float time;
	char level_name[32];
	char mapname[32];
	char nextmap[32];

	Entity_T* sight_client;
	Entity_T* sight_entity;
	int sight_entity_framenum;

	Entity_T* current_entity;
} level_locals;

extern level_locals level;

#endif
#pragma once
