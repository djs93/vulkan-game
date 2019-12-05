#include <gf3d_model.h>
#include <gf3d_pipeline.h>
#ifndef __GF3D_UI_H__
#define __GF3D_UI_H__

typedef struct
{
	VkImage* image;
	VkImageLayout layout;
	uint32_t regionCount;
	VkImageBlit* regions;
	VkFilter filter;
	Bool inuse;
}UILayer;

typedef struct
{
	UILayer* layer_list;
	Uint32   max_layers;
}UIManager;

static UIManager gf3d_ui = { 0, 0 };

void gf3d_ui_draw(UILayer layer, VkCommandBuffer commandBuffer, int bufferFrame);
void gf3d_ui_manager_init(Uint32 max_layers);
void gf3d_ui_manager_close();
void gf3d_ui_delete(UILayer* layer);

#endif