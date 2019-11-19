#include "gf3d_ui.h"

typedef struct
{
	UILayer* layer_list;
	Uint32                  max_layers;
	VkDevice                device;
	Pipeline* pipe;           /**<the pipeline associated with UI rendering*/
}UIManager;

static UIManager gf3d_ui = { 0 };

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

void gf3d_ui_manager_init(Uint32 max_layers, Uint32 chain_length, VkDevice device)
{
	if (max_layers == 0)
	{
		slog("cannot intilizat model manager for 0 models");
		return;
	}
	gf3d_ui.layer_list = (Model*)gfc_allocate_array(sizeof(Model), max_layers);
	gf3d_ui.max_layers = max_layers;
	gf3d_ui.device = device;
	gf3d_ui.pipe = gf3d_vgraphics_get_graphics_pipeline_2d();

	slog("model manager initiliazed");
	atexit(gf3d_ui_manager_close);
}
