#include <SDL.h>            

#include "simple_logger.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"

#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"
#include "gf3d_model.h"
#include "gf3d_camera.h"
#include "gf3d_texture.h"
#include "gf3d_entity.h"
#include "physics.h"

int main(int argc,char *argv[])
{
    int done = 0;
    int a;
    Uint8 validate = 1;
	Uint8 dump = 0;
	Uint8 trace = 0;
    const Uint8 * keys;
    Uint32 bufferFrame = 0;
    VkCommandBuffer commandBuffer;
    Model *model;
    Matrix4 *modelMat;
    Model *model2;
    Matrix4 *modelMat2;
	Model *model3;
    Matrix4 *modelMat3;
    
    for (a = 1; a < argc;a++)
    {
        if (strcmp(argv[a],"-disable_validate") == 0)
        {
            validate = 0;
        }
		else if (strcmp(argv[a], "-disable_vktrace") == 0) 
		{
			trace = 1;
		}
		else if (strcmp(argv[a], "-disable_api_dump") == 0)
		{
			dump = 1;
		}
    }
	//validate = 0;
    
    init_logger("gf3d.log");    
    slog("gf3d begin");
    gf3d_vgraphics_init(
        "gf3d",                 //program name
        1200,                   //screen width
        700,                    //screen height
        vector4d(0.51,0.75,1,1),//background color
        0,                      //fullscreen
        validate,               //validation
		trace,					//vktrace validation layer
		dump					//api dump validation layer
    );
    
    // main game loop
    slog("gf3d main loop begin");

	// set up models
	#pragma region set up models
	model = gf3d_model_load("ezreal");
	model2 = gf3d_model_load("ezreal");
	model3 = gf3d_model_load("dino");
	gf3d_entity_manager_init(2);
	Entity* ent1 = gf3d_entity_new();
    ent1->model = model;
	gfc_matrix_identity(modelMat);
	ent1->modelMat = modelMat;
	Entity* ent2 = gf3d_entity_new();
	ent2->model = model2;
	gfc_matrix_identity(modelMat2);
	ent2->modelMat = modelMat2;
    gfc_matrix_make_translation(
            modelMat2,
            vector3d(10,0,0)
        );
	gfc_matrix_identity(modelMat3);
	#pragma endregion

    while(!done)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        //update game things here
        
		//Update physics for each entity
		vector4d_rotate(&ent1->rotation, 0.02, vector3d(0, 0, 1));

		update_physics_positions();

		//Multiplied each default speed by a factor of 10 because it was moving too slowly (Dale)
        //gf3d_vgraphics_rotate_camera(0.01);
        //gfc_matrix_rotate(modelMat, modelMat, 0.02, vector3d(1,0,0));
        //gfc_matrix_rotate(modelMat2, modelMat2, 0.02, vector3d(0,0,1));

        // configure render command for graphics command pool
        // for each mesh, get a command and configure it from the pool
        bufferFrame = gf3d_vgraphics_render_begin();
        gf3d_pipeline_reset_frame(gf3d_vgraphics_get_graphics_pipeline(),bufferFrame);
            commandBuffer = gf3d_command_rendering_begin(bufferFrame);

                gf3d_model_draw(ent1->model,bufferFrame,commandBuffer,modelMat);
                gf3d_model_draw(ent2->model,bufferFrame,commandBuffer,modelMat2);
                
            gf3d_command_rendering_end(commandBuffer);
            
        gf3d_vgraphics_render_end(bufferFrame);

        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
		if (keys[SDL_SCANCODE_SEMICOLON])ent1->model = model3;
    }    
    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    slog_sync();
    return 0;
}

/*eol@eof*/
