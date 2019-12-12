#include <gf3d_model.h>
#include <gf3d_pipeline.h>
#include <gf3d_sprite.h>
#ifndef __GF3D_UI_H__
#define __GF3D_UI_H__

typedef struct UIElement_S
{
	Bool inuse;
	Bool hidden;
	Sprite* sprite;
	void (*onClick)(struct UIElement_S* self, struct UIElement_S* clicker);
	Vector2D position;
}UIElement;

typedef struct
{
	UIElement* element_list;
	Uint32   max_elements;
}UIManager;

static UIManager gf3d_ui = { 0, 0 };

void gf3d_ui_draw(UIElement* element, int frame, Vector2D position, Uint32 bufferFrame, VkCommandBuffer commandBuffer);
void gf3d_ui_manager_init(Uint32 max_elements);
void gf3d_ui_manager_close();
void gf3d_ui_delete(UIElement* layer);
void gf3d_ui_draw_all(int frame, Uint32 bufferFrame, VkCommandBuffer commandBuffer);
UIElement* gf3d_ui_new();
void gf3d_ui_doClick(int mouseX, int mouseY, UIElement* clicker);
UIElement* gf3d_ui_placeText(char* text, int posX, int posY, Vector4D color, int fontSize);

#endif