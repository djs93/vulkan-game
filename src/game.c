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
#include "local.h"
#include "mob_methods.h"

level_locals level;
Entity_T* entity_list;
Entity_T* player;
void draw_entities();
void sync_camera();
void TestThink(Entity_T* self);
void check_death();

int main(int argc,char *argv[])
{
    int done = 0;
    int a;
    Uint8 validate = 1;
	float frame = 0;
	Uint8 dump = 0;
	Uint8 trace = 0;
    const Uint8 * keys;
    Uint32 bufferFrame = 0;
    VkCommandBuffer commandBuffer;
    Model *model;
    Model *model2;
	Vector3D testvec = vector3d(1.f, 0.f, 0.f);
	SDL_Event event;

    
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
	//model = gf3d_model_load("ezreal");
	//model = gf3d_model_load_animated("ezreal",0,1);
	//model2 = gf3d_model_load_animated("ezreal", 0, 1);
	//model3 = gf3d_model_load("dino");
	gf3d_entity_manager_init(ENTITY_MAX);
	Entity_T* ent1 = modeled_entity_animated("teemo", "Ezreal1", 0, 17);
	player = ent1;
    //ent1->model = model;
	//ent1->name = "Ezreal1";
	ent1->movetype = MOVETYPE_STEP;
	ent1->healthmax = 100.0f;
	ent1->health = ent1->healthmax;
	ent1->boundingBox.size.z *= 0.75f;
	ent1->boundingBox.size.x *= 0.50f;
	ent1->boundingBox.size.y *= 0.50f;
	ent1->maxspeed = vector3d(1000.0f,1000.0f,10000.0f);
	ent1->specFloat1 = 2000.0f;
	//ent1->velocity = vector3d(200, 0, 0);
	//slog("Entities now: %i", gf3d_entity_manager.num_ents);
	//gfc_matrix_identity(modelMat);
	//gfc_matrix_copy(ent1->modelMat, modelMat);
	//gfc_matrix_rotate(ent1->modelMat, ent1->modelMat, 3.14f, vector3d(0,0,1));
	//ent1->modelMat = modelMat;
	Entity_T* ent2 = modeled_entity_animated("ezreal", "Ezreal2", 0, 1);
	//ent2->model = model2;
	//ent2->name = "Ezreal2";
	ent2->movetype = MOVETYPE_STEP;
	ent2->think = TestThink;
	ent2->nextthink = 0.1f;
	ent2->maxspeed = vector3d(0.0f, 0.0f, 10000.0f);
	//gfc_matrix_identity(modelMat2);
	//gfc_matrix_copy(ent2->modelMat, modelMat2);
	//ent2->modelMat = modelMat2;
	Entity_T* ent3 = modeled_entity("ground", "ground");
	Entity_T* ent4 = modeled_entity("platform_one", "plat1");
	Entity_T* ent5 = modeled_entity("platform_one", "plat2");
	Entity_T* ent6 = modeled_entity("platform_one", "plat3");
	teleport_entity(ent2, vector3d(20, 0, 0));
	teleport_entity(ent3, vector3d(0, 0, -19));
	teleport_entity(ent4, vector3d(20, -40, -10));
	teleport_entity(ent5, vector3d(40, -40, 0));
	teleport_entity(ent6, vector3d(60, -40, 10));

	Entity_T* shroom1 = modeled_entity("shroom", "shroom1");
	teleport_entity(shroom1, vector3d(0, -40, -10));
	shroom1->movetype = MOVETYPE_NOCLIP;
	shroom1->think = mushroom_think;
	shroom1->nextthink = 0.1f;
	shroom1->touch = mushroom_touch;
	shroom1->die = mushroom_die;
	shroom1->healthmax = 1.0f;
	shroom1->health = shroom1->healthmax;

	Entity_T* spring1 = modeled_entity("spring", "spring1");
	teleport_entity(spring1, vector3d(-20, -40, -10));
	spring1->movetype = MOVETYPE_NOCLIP;
	spring1->think = spring_think;
	spring1->nextthink = 0.1f;
	spring1->touch = spring_touch;
	spring1->die = spring_die;
	spring1->healthmax = 1.0f;
	spring1->health = spring1->healthmax;

	Entity_T* speedup1 = modeled_entity("speed_up", "speedup1");
	teleport_entity(speedup1, vector3d(-40, -40, -10));
	speedup1->movetype = MOVETYPE_NOCLIP;
	speedup1->think = speed_up_think;
	speedup1->nextthink = 0.1f;
	speedup1->touch = speed_up_touch;
	speedup1->die = speed_up_die;
	speedup1->healthmax = 1.0f;
	speedup1->health = speedup1->healthmax;

	#pragma endregion
	float x, y, z, m = 0.;
	Vector3D forward;
	float accel = 15.0f;
    while(!done)
    {
		//slog("%f", ent1->velocity.x);
        SDL_PumpEvents();   // update SDL's internal event structures
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_KEYDOWN:
					switch (event.key.keysym.scancode) {
						case(SDL_SCANCODE_D):
							if (abs(ent1->acceleration.x - accel) <= accel) {
								ent1->acceleration.x -= accel;
							}
							break;
						case(SDL_SCANCODE_A):
							if (abs(ent1->acceleration.x + accel) <= accel) {
								ent1->acceleration.x += accel;
							}
							break;
						case(SDL_SCANCODE_W):
							if (abs(ent1->acceleration.y - accel) <= accel) {
								ent1->acceleration.y -= accel;
							}
							break;
						case(SDL_SCANCODE_S):
							if (abs(ent1->acceleration.y + accel) <= accel) {
								ent1->acceleration.y += accel;
							}
							break;
						case(SDL_SCANCODE_LSHIFT):
							if (!(player->flags & FL_SPRINTING)) {
								slog("Set sprint");
								player->maxspeed.x = player->maxspeed.x * 2;
								player->maxspeed.y = player->maxspeed.y * 2;
								int combo = player->flags | FL_SPRINTING;
								player->flags = combo;
							}
							break;
						default:
							break;
					}
					break;
				case SDL_KEYUP:
					switch (event.key.keysym.scancode) {
					case(SDL_SCANCODE_D):
						if (abs(ent1->acceleration.x + accel) <= accel) {
							ent1->acceleration.x += accel;
						}
						break;
					case(SDL_SCANCODE_A):
						if (abs(ent1->acceleration.x - accel) <= accel) {
							ent1->acceleration.x -= accel;
						}
						break;
					case(SDL_SCANCODE_W):
						if (abs(ent1->acceleration.y + accel) <= accel) {
							ent1->acceleration.y += accel;
						}
						break;
					case(SDL_SCANCODE_S):
						if (abs(ent1->acceleration.y - accel) <= accel) {
							ent1->acceleration.y -= accel;
						}
						break;
					case(SDL_SCANCODE_LSHIFT):
						if (player->flags & FL_SPRINTING) {
							slog("Unset sprint");
							player->maxspeed.x = player->maxspeed.x / 2;
							player->maxspeed.y = player->maxspeed.y / 2;
							int combo = player->flags & ~FL_SPRINTING;
							player->flags = combo;
						}
						break;
					default:
						break;
					}
					break;
				default:
					break;
			}
		}
		keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        //update game things here
        
		//Update physics for each entity
		//vector4d_rotate(&ent1->rotation, 0.02, vector3d(0, 0, 1));
		//vector3d_rotate_about_vector(&ent1->rotation, vector3d(0, 0, 1), ent1->rotation, 0.02);
		if (keys[SDL_SCANCODE_D] && keys[SDL_SCANCODE_A]) {
			ent1->acceleration.x = 0.0f;
		}
		if (keys[SDL_SCANCODE_W] && keys[SDL_SCANCODE_S]) {
			ent1->acceleration.y = 0.0f;
		}
		if (keys[SDL_SCANCODE_SPACE]&& !(ent1->flags & FL_JUMPING)) {
			ent1->velocity.z = ent1->specFloat1;
			int combo = ent1->flags | FL_JUMPING;
			ent1->flags = combo;
		}
		if (keys[SDL_SCANCODE_RIGHT]) { 
			rotate_entity(ent1, 0.02, vector3d(0, 0, 1));
		}
		if (keys[SDL_SCANCODE_LEFT]) {
			rotate_entity(ent1, -0.02, vector3d(0, 0, 1));
		}
		if (keys[SDL_SCANCODE_UP]) {
			rotate_entity(ent1, 0.02, vector3d(1, 0, 0));
		}
		if (keys[SDL_SCANCODE_DOWN]) {
			rotate_entity(ent1, -0.02, vector3d(1, 0, 0));
		}
		if (keys[SDL_SCANCODE_X]) {
			rotate_entity(ent1, 0.02, vector3d(1, 0, 0));
		}
		if (keys[SDL_SCANCODE_Y]) {
			rotate_entity(ent1, 0.02, vector3d(0, 1, 0));
		}
		if (keys[SDL_SCANCODE_Z]) {
			rotate_entity(ent1, 0.02, vector3d(0, 0, 1));
		}
		/**
		if (keys[SDL_SCANCODE_L]) {
			float sy = sqrt(pow(ent1->modelMat[0][0], 2) + pow(ent1->modelMat[1][0], 2));
			
			Bool singular = sy < 0.000001f;
			if (!singular) {
				x = atan2(ent1->modelMat[2][1], ent1->modelMat[2][2]) * GFC_RADTODEG;
				y = atan2(-ent1->modelMat[2][0], sy) * GFC_RADTODEG;
				z = atan2(ent1->modelMat[1][0], ent1->modelMat[0][0]) * GFC_RADTODEG;
			}
			else {
				x = atan2(-ent1->modelMat[1][2], ent1->modelMat[1][1]) * GFC_RADTODEG;
				y = atan2(-ent1->modelMat[2][0], sy)* GFC_RADTODEG;
				z = 0;
			}
			vector3d_angle_vectors(vector3d(x, y, z), &forward, NULL, NULL);
			rotate_entity(ent2, 0.02, forward);
		}
		if (keys[SDL_SCANCODE_KP_0]) {
			gfc_matrix_identity(ent1->modelMat);
			gfc_matrix_identity(ent2->modelMat);
		}
		*/
		if (keys[SDL_SCANCODE_KP_PERIOD]) {
			//slog("\nx:%f\ny:%f\nz:%f", x, y, z);
			gfc_matrix_slog(ent1->modelMat);
			//slog("\nx:%f\ny:%f\nz:%f", forward.x, forward.y, forward.z);
		}
		/**
		if (keys[SDL_SCANCODE_D]) {
			//gfc_matrix_make_translation(ent1->modelMat, vector3d(m, 0, 0));
			//m++;
			ent1->acceleration = vector3d(10,0,0);
		}
		if (!keys[SDL_SCANCODE_D] && !vector3d_equal(ent1->acceleration, vector3d(0, 0, 0))) {
			ent1->acceleration = vector3d(0, 0, 0);
		}
		
		if (keys[SDL_SCANCODE_A]) {
			//gfc_matrix_make_translation(ent1->modelMat, vector3d(m, 0, 0));
			//m++;
			ent1->acceleration = vector3d(-10,0,0);
		}
		if (!keys[SDL_SCANCODE_A] && !vector3d_equal(ent1->acceleration, vector3d(0, 0, 0))) {
			ent1->acceleration = vector3d(0, 0, 0);
		}
		*/
		update_physics_positions();
		check_death();
		/**
		for (int i = 0; i < gf3d_entity_manager.entity_max; i++) {
			run_entity(&entity_list[i]);
		}
		*/
		//slog("\nx:%f\ny:%f\nz:%f", ent1->rotation.x, ent1->rotation.y, ent1->rotation.z);
		//Multiplied each default speed by a factor of 10 because it was moving too slowly (Dale)
        //gf3d_vgraphics_rotate_camera(0.01);
        //gfc_matrix_rotate(modelMat, modelMat, 0.02, vector3d(1,0,0));
        //gfc_matrix_rotate(modelMat2, modelMat2, 0.02, vector3d(0,0,1));

        // configure render command for graphics command pool
        // for each mesh, get a command and configure it from the pool
        bufferFrame = gf3d_vgraphics_render_begin();
        gf3d_pipeline_reset_frame(gf3d_vgraphics_get_graphics_pipeline(),bufferFrame);
            commandBuffer = gf3d_command_rendering_begin(bufferFrame);
				//Make a "draw entities" function that does all this for each entity
                //gf3d_model_draw(ent1->model,bufferFrame,commandBuffer, ent1->modelMat, 0);
                //gf3d_model_draw(ent2->model,bufferFrame,commandBuffer, ent2->modelMat, 0);
				
				draw_entities(bufferFrame, commandBuffer, (int)frame);
				sync_camera(ent1);
				
				frame = frame + 0.1; 
				level.framenum++;
				level.time = level.framenum * 0.1f;
				//slog("Frame now %f", frame);
				//if (frame >= 1)frame = 0;

				//ent1->boundingBox.size = *ent1->model->extents[(Uint32)frame];
				//ent2->boundingBox.size = *ent2->model->extents[(Uint32)frame];

				//slog("BB1 size x:%f, y:%f, z:%f", ent1->boundingBox.size.x, ent1->boundingBox.size.y, ent1->boundingBox.size.z);
				//slog("BB2 size x:%f, y:%f, z:%f", ent2->boundingBox.size.x, ent2->boundingBox.size.y, ent2->boundingBox.size.z);

            gf3d_command_rendering_end(commandBuffer);
            
        gf3d_vgraphics_render_end(bufferFrame);

        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
		//if (keys[SDL_SCANCODE_SEMICOLON])ent1->model = model3;
    }    
    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    slog_sync();
    return 0;
}

void RunFrame() {
	int i;
	Entity_T* ent;

	level.framenum++;
	level.time = level.framenum * 0.1f;

	ent = &entity_list[0];
	for (i = 0; i < gf3d_entity_manager.entity_max; i++, ent++) {
		if (!ent->_inuse) {
			continue;
		}

		level.current_entity = ent;

		vector3d_copy(ent->old_origin, ent->origin);

		//if the ground entity moved, make sure we're still on it
		if ((ent->groundentity) && (ent->groundentity->linkcount != ent->groundentity_linkcount))
		{
			ent->groundentity = NULL;
			//if (!(ent->flags & FL_FLY) && (ent->svflags & SVF_MONSTER))
			//{
			//	//Implement later:
			//	//M_CheckGround(ent);
			//}
		}

		run_entity(ent);
	}
}

void TestThink(Entity_T* self) {
	//slog("My name is %s", self->name);
	rotate_entity(self, 0.01, vector3d(0, 0, 1));
	self->nextthink = 0.1f;
}

void draw_entities(Uint32 bufferFrame, VkCommandBuffer commandBuffer, int frame) {
	int i = 0;
	Entity_T* ent;
	while (i < gf3d_entity_manager.entity_max) {
		ent = &entity_list[i];
		if (ent->_inuse == 0) {
			i++;
			continue;
		}

		if (ent->model) {
			if (strcmp(ent->name, "Ezreal1")) {
				//slog("Frame int is %i", frame);
				//slog("drawing player with frame %i", (int)frame % ent->model->frameCount);
			}
			gf3d_model_draw(ent->model, bufferFrame, commandBuffer, ent->modelMat, (Uint32)frame%ent->model->frameCount);
			//ent->boundingBox.size = *ent->model->extents[(Uint32)frame % ent->model->frameCount];
		}

		i++;
	}
}

void sync_camera(Entity_T* ent) {
	Vector3D pos;
	vector3d_add(pos, ent->boundingBox.position, vector3d(5, 60, 15));
	//vector3d_add(pos, ent->boundingBox.position, vector3d(60, 0, 0));
	gf3d_vgraphics_set_camera_pos(pos, ent->boundingBox.position, vector3d(0,0,1));
}

void check_death() {
	int i = 0;
	Entity_T* ent;
	while (i < gf3d_entity_manager.entity_max) {
		ent = &entity_list[i];
		if (ent->_inuse == 0) {
			i++;
			continue;
		}

		if (ent->healthmax>0.0f && ent->die && ent->health<=0.0f) {
			ent->die(ent);
		}

		i++;
	}
}
/*eol@eof*/
