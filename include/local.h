#define FRAMETIME 0.1

#define FL_FLY 0x00000001

#include "game.h"

#ifndef __LOCAL_H__
#define __LOCAL_H__

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

	Entity* sight_client;
	Entity* sight_entity;
	int sight_entity_framenum;

	Entity* current_entity;
} level_locals;

extern level_locals level;

#endif
#pragma once
