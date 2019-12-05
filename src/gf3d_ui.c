#include "gf3d_ui.h"
#include "gf3d_swapchain.h"
#include "simple_logger.h"


void gf3d_ui_manager_close()
{
	int i;
	for (i = 0; i < gf3d_ui.max_layers; i++)
	{
		gf3d_ui_delete(&gf3d_ui.layer_list[i]);
	}
	if (gf3d_ui.layer_list)
	{
		free(gf3d_ui.layer_list);
	}
	memset(&gf3d_ui, 0, sizeof(UIManager));
	slog("model manager closed");
}

void gf3d_ui_manager_init(Uint32 max_layers)
{
	if (max_layers == 0)
	{
		slog("cannot intilizat model manager for 0 models");
		return;
	}
	gf3d_ui.layer_list = (Model*)gfc_allocate_array(sizeof(Model), max_layers);
	gf3d_ui.max_layers = max_layers;

	slog("model manager initiliazed");
	atexit(gf3d_ui_manager_close);
}

void gf3d_ui_draw(UILayer layer, VkCommandBuffer commandBuffer, int bufferFrame) {
	int i = 0;
	//grab swapchain image to blit to
	VkImage blitImage = gf3d_swapchain_getImages()[bufferFrame];
	//blit for each region defined in the layer
	vkCmdBlitImage(commandBuffer, &layer.image, layer.layout, blitImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, layer.regionCount, layer.regions, layer.filter);
}

void gf3d_ui_delete(UILayer* layer) {
	layer->inuse = false;
	memset(layer, 0, sizeof(UILayer));
}
