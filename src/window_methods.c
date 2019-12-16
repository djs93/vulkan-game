#include "..\include\window_methods.h"
#include "simple_logger.h"
#include "game.h"
#include "gf3d_ui.h"
#include "gf3d_entity.h"

void testClick(UIElement* self, UIElement* mouseEle)
{
	slog("Text box clicked!");
	mouseEle->hidden = !(mouseEle->hidden);
	self->hidden = !(self->hidden);
}

void startButtonClick(UIElement* self, UIElement* mouseEle)
{
	//mainMenuToLevelOne();
	gf3d_ui_free_all_but_mouse();
	loadLevel("level1.json");
	mouseEle->hidden = true;
}


void contentEditorButtonClick(UIElement* self, UIElement* mouseEle)
{
	mainMenuToContentEditor();
}

void saveClick(UIElement* self, UIElement* mouseEle)
{
	save_all_content_editor();
}

void platformClick(UIElement* self, UIElement* mouseEle)
{
	Entity_T* attachEnt = find_entity("axes_attach");
	if (attachEnt) {
		attachEnt->model = gf3d_model_load("platform_one");
		attachEnt->type = "platform";
	}
}

void spawnClick(UIElement* self, UIElement* mouseEle)
{
	if (find_entity("player")) { return; }
	Entity_T* attachEnt = find_entity("axes_attach");
	if (attachEnt) {
		attachEnt->model = gf3d_model_load("teemo_000000");
		attachEnt->model->texture = gf3d_texture_load("images/teemo.png");
		attachEnt->type = "player";
	}
}

void groundClick(UIElement* self, UIElement* mouseEle)
{
	Entity_T* attachEnt = find_entity("axes_attach");
	if (attachEnt) {
		attachEnt->model = gf3d_model_load("ground");
		attachEnt->type = "ground";
	}
}

void jumperClick(UIElement* self, UIElement* mouseEle)
{
	Entity_T* attachEnt = find_entity("axes_attach");
	if (attachEnt) {
		attachEnt->model = gf3d_model_load("robot_000000");
		attachEnt->model->texture = gf3d_texture_load("images/robot.png");
		attachEnt->type = "jumper";
	}
}

void pacerClick(UIElement* self, UIElement* mouseEle)
{
	Entity_T* attachEnt = find_entity("axes_attach");
	if (attachEnt) {
		attachEnt->model = gf3d_model_load("penguin_000000");
		attachEnt->model->texture = gf3d_texture_load("images/penguin.png");
		attachEnt->type = "pacer";
	}
}

void circlerClick(UIElement* self, UIElement* mouseEle)
{
	Entity_T* attachEnt = find_entity("axes_attach");
	if (attachEnt) {
		attachEnt->model = gf3d_model_load("circler_000000");
		attachEnt->model->texture = gf3d_texture_load("images/circler.png");
		attachEnt->type = "circler";
	}
}

void mushroomClick(UIElement* self, UIElement* mouseEle)
{
	Entity_T* attachEnt = find_entity("axes_attach");
	if (attachEnt) {
		attachEnt->model = gf3d_model_load("shroom");
		attachEnt->type = "mushroom";
	}
}

void springClick(UIElement* self, UIElement* mouseEle)
{
	Entity_T* attachEnt = find_entity("axes_attach");
	if (attachEnt) {
		attachEnt->model = gf3d_model_load("spring");
		attachEnt->type = "spring";
	}
}

void speedClick(UIElement* self, UIElement* mouseEle)
{
	Entity_T* attachEnt = find_entity("axes_attach");
	if (attachEnt) {
		attachEnt->model = gf3d_model_load("speed_up");
		attachEnt->type = "speed";
	}
}
