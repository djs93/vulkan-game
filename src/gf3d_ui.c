#include "gf3d_ui.h"
#include "gf3d_swapchain.h"
#include "simple_logger.h"


void gf3d_ui_manager_close()
{
	int i;
	for (i = 0; i < gf3d_ui.max_elements; i++)
	{
		gf3d_ui_delete(&gf3d_ui.element_list[i]);
	}
	if (gf3d_ui.element_list)
	{
		free(gf3d_ui.element_list);
	}
	memset(&gf3d_ui, 0, sizeof(UIManager));
	slog("model manager closed");
}

void gf3d_ui_manager_init(Uint32 max_elements)
{
	if (max_elements == 0)
	{
		slog("cannot intilizat model manager for 0 models");
		return;
	}
	gf3d_ui.element_list = (Model*)gfc_allocate_array(sizeof(Model), max_elements);
	gf3d_ui.max_elements = max_elements;

	slog("model manager initiliazed");
	atexit(gf3d_ui_manager_close);
}

void gf3d_ui_draw(UIElement* element, int frame, Vector2D position, Uint32 bufferFrame, VkCommandBuffer commandBuffer) {
	gf3d_sprite_draw(element->sprite, element->position, (Uint32)frame % element->sprite->frameCount, bufferFrame, commandBuffer);
}

void gf3d_ui_delete(UIElement* element) {
	element->inuse = false;
	memset(element, 0, sizeof(UIElement));
}

void gf3d_ui_draw_all(int frame, Uint32 bufferFrame, VkCommandBuffer commandBuffer)
{
	int i;
	/* first in, farthest back method
	for (i = 0; i < gf3d_ui.max_elements; i++) {
		if (gf3d_ui.element_list[i].inuse) {
			gf3d_ui_draw(&gf3d_ui.element_list[i], frame, gf3d_ui.element_list[i].position, bufferFrame, commandBuffer);
		}
	}
	*/
	//first in, furthest front method (aka the one we're using)
	for (i = gf3d_ui.max_elements-1; i >= 0; i--) {
		if (gf3d_ui.element_list[i].inuse && !gf3d_ui.element_list[i].hidden) {
			gf3d_ui_draw(&gf3d_ui.element_list[i], frame, gf3d_ui.element_list[i].position, bufferFrame, commandBuffer);
		}
	}
}

UIElement* gf3d_ui_new()
{
	UIElement* ele = NULL;
	int i;
	for (i = 0; i < gf3d_ui.max_elements; i++)
	{
		if (gf3d_ui.element_list[i].inuse)continue;
		//. found a free entity
		memset(&gf3d_ui.element_list[i], 0, sizeof(UIElement));
		ele = &gf3d_ui.element_list[i];
		ele->inuse = 1;
		ele->onClick = NULL;
		return &gf3d_ui.element_list[i];
	}
	slog("request for entity failed: all full up");
	return NULL;
}

void gf3d_ui_doClick(int mouseX, int mouseY, UIElement* clicker)
{
	int i;
	UIElement* clickyEle = NULL; //The element clicked
	for (i = 0; i < gf3d_ui.max_elements; i++)
	{
		UIElement* currentEle = &gf3d_ui.element_list[i];
		if (!currentEle->inuse)continue;
		if (!currentEle->onClick)continue;
		if (mouseX > currentEle->position.x && mouseX < currentEle->position.x + currentEle->sprite->texture->width) {
			if (mouseY > currentEle->position.y && mouseY < currentEle->position.y + currentEle->sprite->texture->height) {
				clickyEle = currentEle;
			}
		}
	}
	if (clickyEle) {
		clickyEle->onClick(clickyEle, clicker);
	}
}

