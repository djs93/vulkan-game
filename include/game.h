#ifndef __GAME_H__
#define __GAME_H__

#define ENTITY_MAX 12
#define UI_MAX 15
typedef struct Entity_S Entity_T;

typedef enum
{
	GS_MainMenu = 0,
	GS_InGameMenu = 1,
	GS_InGame = 2,
	GS_InContentEditor = 3
}GameState;

GameState state;
void mainMenuToLevelOne();

#endif