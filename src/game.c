#include <SDL.h>            

#include "simple_logger.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"
#include "simple_json.h"

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
#include "gf3d_ui.h"
#include "gf3d_sprite.h"
#include "window_methods.h"
#include "SDL_ttf.h"

level_locals level;
Entity_T* entity_list;
Entity_T* player;
int window_width;
int window_height;
void draw_normal_entities(Uint32 bufferFrame, VkCommandBuffer commandBuffer, int frame);
void draw_place_entities(Uint32 bufferFrame, VkCommandBuffer commandBuffer, int frame);
void sync_camera(Entity_T * ent, Vector3D offset);
void setupMainMenu();
void TestThink(Entity_T* self);
void check_death();
void setupLevelOne();
void setupContentEditor();

int main(int argc,char *argv[])
{
    int done = 0;
    int a;
    Uint8 validate = 0;
	float frame = 0;
	level.time = 0;
	level.modelTime = 0;
	Uint8 dump = 0;
	Uint8 trace = 0;
    const Uint8 * keys;
    Uint32 bufferFrame = 0;
    VkCommandBuffer commandBuffer;
    Model *model;
    Model *model2;
	Vector3D testvec = vector3d(1.f, 0.f, 0.f);
	SDL_Event event;
	window_width = 1600;
	window_height = 900;
	float cameraSpeed = 0.7f;
    
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
    

    init_logger("gf3d.log");    
    slog("gf3d begin");
    gf3d_vgraphics_init(
        "gf3d",                 //program name
		window_width,           //screen width
		window_height,          //screen height
        vector4d(0.51,0.75,1,1),//background color
        0,                      //fullscreen
        0,               //validation
		1,					//vktrace validation layer
		1					//api dump validation layer
    );
    
    // main game loop
    slog("gf3d main loop begin");

	state = GS_MainMenu;

	// set up entities
	#pragma region set up entities
	gf3d_entity_manager_init(ENTITY_MAX);
	gf3d_ui_manager_init(UI_MAX);
	
	Sprite* mouse = NULL;
	int mousex, mousey;
	mouse = gf3d_sprite_load("images/pointer.png", 32, 32, 16);
	UIElement* mouseEle = gf3d_ui_new();
	mouseEle->sprite = mouse;
	mouseEle->name = "mouse";

	Vector3D contentOffset = vector3d(150,150,80);

	/*
	UIElement* testBox = gf3d_ui_new();
	testBox->sprite = gf3d_sprite_load("images/ground.png", -1, -1, 1);
	testBox->onClick = testClick;
	testBox->position = vector2d(300, 300);
	*/

	#pragma endregion
	float accel = 15.0f;
	TTF_Init();
	setupMainMenu();
    while(!done)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
		SDL_GetMouseState(&mousex, &mousey);
		keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
		if (state == GS_MainMenu) {
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_KEYDOWN:
					switch (event.key.keysym.scancode) {
					case(SDL_SCANCODE_ESCAPE):
						done = 1;
						break;
					default:
						break;
					}
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == SDL_BUTTON_LEFT) {
						gf3d_ui_doClick(mousex, mousey, mouseEle);
					}
					break;
				default:
					break;
				}
			}
			mouseEle->position = vector2d(mousex, mousey);
		}
		else if (state == GS_InGameMenu) {
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == SDL_BUTTON_LEFT) {
						gf3d_ui_doClick(mousex, mousey, mouseEle);
					}
					break;
				default:
					break;
				}
			}
			mouseEle->position = vector2d(mousex, mousey);
		}
		else if (state == GS_InGame) {
			if (get_type_count("mushroom") == 0) {
				if (loadLevel(level.nextmap)) {
					toMainMenu();
				}
			}
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_KEYDOWN:
					switch (event.key.keysym.scancode) {
					case(SDL_SCANCODE_ESCAPE):
						toMainMenu();
						break;
					case(SDL_SCANCODE_D):
						if (abs(player->acceleration.x - accel) <= accel) {
							player->acceleration.x -= accel;
						}
						break;
					case(SDL_SCANCODE_A):
						if (abs(player->acceleration.x + accel) <= accel) {
							player->acceleration.x += accel;
						}
						break;
					case(SDL_SCANCODE_W):
						if (abs(player->acceleration.y - accel) <= accel) {
							player->acceleration.y -= accel;
						}
						break;
					case(SDL_SCANCODE_S):
						if (abs(player->acceleration.y + accel) <= accel) {
							player->acceleration.y += accel;
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
						if (abs(player->acceleration.x + accel) <= accel) {
							player->acceleration.x += accel;
						}
						break;
					case(SDL_SCANCODE_A):
						if (abs(player->acceleration.x - accel) <= accel) {
							player->acceleration.x -= accel;
						}
						break;
					case(SDL_SCANCODE_W):
						if (abs(player->acceleration.y + accel) <= accel) {
							player->acceleration.y += accel;
						}
						break;
					case(SDL_SCANCODE_S):
						if (abs(player->acceleration.y - accel) <= accel) {
							player->acceleration.y -= accel;
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
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == SDL_BUTTON_LEFT) {
						gf3d_ui_doClick(mousex, mousey, mouseEle);
					}
					break;
				default:
					break;
				}
			}

			if (keys[SDL_SCANCODE_D] && keys[SDL_SCANCODE_A]) {
				player->acceleration.x = 0.0f;
			}
			if (keys[SDL_SCANCODE_W] && keys[SDL_SCANCODE_S]) {
				player->acceleration.y = 0.0f;
			}
			if (keys[SDL_SCANCODE_SPACE] && !(player->flags & FL_JUMPING)) {
				player->velocity.z = player->specFloat1;
				int combo = player->flags | FL_JUMPING;
				player->flags = combo;
			}
			if (keys[SDL_SCANCODE_RIGHT]) {
				rotate_entity(player, -0.01f, vector3d(0, 0, 1));
				gfc_matrix_slog(player->modelMat);
			}

			update_physics_positions();
			//update_physics_positions();
			//update_physics_positions();
			//update_physics_positions();
			check_death();
			//check_death();
			//check_death();
			//check_death();
		}
		else if (state == GS_InContentEditor) {
			Entity_T* axes = find_entity("axes");
			Entity_T* axesAttach = find_entity("axes_attach");
			if (axes && axesAttach) {
				Bool positionChanged = false;
				Vector3D pos = axes->position;
				Bool placeEntity = false;
				Bool deleteEntity = false;
				while (SDL_PollEvent(&event)) {
					switch (event.type) {
					case SDL_KEYDOWN:
						switch (event.key.keysym.scancode) {
						case(SDL_SCANCODE_ESCAPE):
							toMainMenu();
							break;
						case(SDL_SCANCODE_D):
							positionChanged = true;
							vector3d_add(pos, pos, vector3d(0.2, 0, 0));
							if (keys[SDL_SCANCODE_LSHIFT]) {
								vector3d_add(pos, pos, vector3d(0.2, 0, 0));
							}
							teleport_entity(axes, pos);
							teleport_entity(axesAttach, pos);
							break;
						case(SDL_SCANCODE_A):
							positionChanged = true;
							vector3d_add(pos, pos, vector3d(-0.2, 0, 0));
							if (keys[SDL_SCANCODE_LSHIFT]) {
								vector3d_add(pos, pos, vector3d(-0.2, 0, 0));
							}
							teleport_entity(axes, pos);
							teleport_entity(axesAttach, pos);
							break;
						case(SDL_SCANCODE_W):
							positionChanged = true;
							vector3d_add(pos, pos, vector3d(0, 0.2, 0));
							if (keys[SDL_SCANCODE_LSHIFT]) {
								vector3d_add(pos, pos, vector3d(0, 0.2, 0));
							}
							teleport_entity(axes, pos);
							teleport_entity(axesAttach, pos);
							break;
						case(SDL_SCANCODE_S):
							positionChanged = true;
							vector3d_add(pos, pos, vector3d(0, -0.2, 0));
							if (keys[SDL_SCANCODE_LSHIFT]) {
								vector3d_add(pos, pos, vector3d(0, -0.2, 0));
							}
							teleport_entity(axes, pos);
							teleport_entity(axesAttach, pos);
							break;
						case(SDL_SCANCODE_Q):
							positionChanged = true;
							vector3d_add(pos, pos, vector3d(0, 0, -0.2));
							if (keys[SDL_SCANCODE_LSHIFT]) {
								vector3d_add(pos, pos, vector3d(0, 0, -0.2));
							}
							teleport_entity(axes, pos);
							teleport_entity(axesAttach, pos);
							break;
						case(SDL_SCANCODE_E):
							positionChanged = true;
							vector3d_add(pos, pos, vector3d(0, 0, 0.2));
							if (keys[SDL_SCANCODE_LSHIFT]) {
								vector3d_add(pos, pos, vector3d(0, 0, 0.2));
							}
							teleport_entity(axes, pos);
							teleport_entity(axesAttach, pos);
							break;
						case(SDL_SCANCODE_SPACE):
							placeEntity = true;
							break;
						case(SDL_SCANCODE_Z):
							deleteEntity = true;
							break;
						case(SDL_SCANCODE_J):
							contentOffset.x -= cameraSpeed*2;
							if (keys[SDL_SCANCODE_LSHIFT]) {
								contentOffset.x -= cameraSpeed * 2;
							}
							break;
						case(SDL_SCANCODE_L):
							contentOffset.x += cameraSpeed * 2;
							if (keys[SDL_SCANCODE_LSHIFT]) {
								contentOffset.x += cameraSpeed * 2;
							}
							break;
						case(SDL_SCANCODE_I):
							contentOffset.y += cameraSpeed * 2;
							if (keys[SDL_SCANCODE_LSHIFT]) {
								contentOffset.y += cameraSpeed * 2;
							}
							break;
						case(SDL_SCANCODE_K):
							contentOffset.y -= cameraSpeed * 2;
							if (keys[SDL_SCANCODE_LSHIFT]) {
								contentOffset.y -= cameraSpeed * 2;
							}
							break;
						case(SDL_SCANCODE_U):
							contentOffset.z += cameraSpeed;
							if (keys[SDL_SCANCODE_LSHIFT]) {
								contentOffset.z += cameraSpeed;
							}
							break;
						case(SDL_SCANCODE_O):
							contentOffset.z -= cameraSpeed;
							if (keys[SDL_SCANCODE_LSHIFT]) {
								contentOffset.z -= cameraSpeed;
							}
							break;
						default:
							break;
						}
					case SDL_MOUSEBUTTONDOWN:
						if (event.button.button == SDL_BUTTON_LEFT) {
							gf3d_ui_doClick(mousex, mousey, mouseEle);
						}
						break;
					default:
						break;
					}
				}
				if (positionChanged) {
					UIElement* positionUI = gf3d_ui_find("position");
					char str[50];
					sprintf(str, "Pozycja: %f, %f, %f", player->position.x, player->position.y, player->position.z);
					gf3d_sprite_free(positionUI->sprite);
					positionUI->sprite = gf3d_ui_getTextSprite(str, vector4d(200, 200, 200, 200), 75);
				}
				if (placeEntity) {
					if (axesAttach->type) {
						if (!find_entity("player")||strcmp(axesAttach->type, "player") != 0) {
							Entity_T* placedEnt = gf3d_nonanimated_entity_copy(axesAttach);
							teleport_entity(placedEnt, axesAttach->position);
						}
					}
				}
				if (deleteEntity) {
					Entity_T* ent = get_last_entity();
					if (ent->name && strcmp(ent->name, "axes")!=0 && strcmp(ent->name, "axes_attach") != 0) {
						ent->_inuse = 0;
					}
				}
			}
			mouseEle->position = vector2d(mousex, mousey);
		}

        // configure render command for graphics command pool
        // for each mesh, get a command and configure it from the pool
        bufferFrame = gf3d_vgraphics_render_begin();
		gf3d_pipeline_reset_frame(gf3d_vgraphics_get_graphics_overlay_pipeline(), bufferFrame);
		gf3d_pipeline_reset_frame(gf3d_vgraphics_get_graphics_model_pipeline(), bufferFrame);
		gf3d_pipeline_reset_frame(gf3d_vgraphics_get_graphics_place_pipeline(), bufferFrame);

		if (state != GS_MainMenu) {
			commandBuffer = gf3d_command_rendering_begin(bufferFrame, gf3d_vgraphics_get_graphics_model_pipeline());

			draw_normal_entities(bufferFrame, commandBuffer, (int)level.modelTime);
			if(player && state ==GS_InGame){
				sync_camera(player, vector3d(10, 70, 30));
			}
			else if(player && state==GS_InContentEditor) {
				sync_camera(player, contentOffset);
			}
			
			gf3d_command_rendering_end(commandBuffer);

			//do place shader pipeline render here
			if (state == GS_InContentEditor) {
				commandBuffer = gf3d_command_rendering_begin(bufferFrame, gf3d_vgraphics_get_graphics_place_pipeline());
				draw_place_entities(bufferFrame, commandBuffer, (int)level.modelTime);
				gf3d_command_rendering_end(commandBuffer);
			}
			//frame = frame + 0.1;
			//level.framenum++;
			level.time += 0.1f;
			if (state != GS_InGameMenu && state != GS_InContentEditor) {
				level.modelTime += 0.1f;
			}

		}
		else { //blank out the background (need to render nothing in model pipeline)
			commandBuffer = gf3d_command_rendering_begin(bufferFrame, gf3d_vgraphics_get_graphics_model_pipeline());
			gf3d_command_rendering_end(commandBuffer);
			level.time += 0.1f;
		}
		// 2D overlay rendering

		commandBuffer = gf3d_command_rendering_begin(bufferFrame, gf3d_vgraphics_get_graphics_overlay_pipeline());

		gf3d_ui_draw_all((int)level.time, bufferFrame, commandBuffer);

		gf3d_command_rendering_end(commandBuffer);
		gf3d_vgraphics_render_end(bufferFrame);
    }    
    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    slog_sync();
    return 0;
}

void TestThink(Entity_T* self) {
	//slog("My name is %s", self->name);
	rotate_entity(self, 0.01, vector3d(0, 0, 1));
	self->nextthink = 0.1f;
}

void draw_normal_entities(Uint32 bufferFrame, VkCommandBuffer commandBuffer, int frame) {
	int i = 0;
	Entity_T* ent;
	while (i < gf3d_entity_manager.entity_max) {
		ent = &entity_list[i];
		if (ent->_inuse == 0) {
			i++;
			continue;
		}

		if (ent->model) {
			if (!ent->name || strcmp(ent->name, "axes_attach") != 0) {
				gf3d_model_draw(ent->model, bufferFrame, commandBuffer, ent->modelMat, (Uint32)frame % ent->model->frameCount);
			}
		}

		i++;
	}
}

void draw_place_entities(Uint32 bufferFrame, VkCommandBuffer commandBuffer, int frame) {
	int i = 0;
	Entity_T* ent;
	while (i < gf3d_entity_manager.entity_max) {
		ent = &entity_list[i];
		if (ent->_inuse == 0) {
			i++;
			continue;
		}

		if (ent->model) {
			if (ent->name && strcmp(ent->name, "axes_attach") == 0) {
				gf3d_model_draw(ent->model, bufferFrame, commandBuffer, ent->modelMat, (Uint32)frame % ent->model->frameCount);
			}
		}

		i++;
	}
}

void sync_camera(Entity_T* ent, Vector3D offset) {
	Vector3D pos;
	vector3d_add(pos, ent->boundingBox.position, offset);
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

void setupMainMenu() {
	UIElement* titleText = gf3d_ui_placeText("Gra Vulkan", 0, 50, vector4d(255, 255, 255, 255), 250);
	titleText->position.x = window_width/2 - titleText->sprite->frameWidth / 4;

	UIElement* startButton = gf3d_ui_placeText("Zaczynac", 0, 700, vector4d(95, 100, 99, 255), 250);
	startButton->position.x = window_width / 2 - startButton->sprite->frameWidth / 4;
	startButton->onClick = startButtonClick;

	UIElement* contentButton = gf3d_ui_placeText("Edytor tresci", 0, 825, vector4d(95, 100, 99, 255), 100);
	contentButton->position.x = window_width / 2 - contentButton->sprite->frameWidth / 4;
	contentButton->onClick = contentEditorButtonClick;

	UIElement* bg1 = gf3d_ui_new();
	bg1->sprite = gf3d_sprite_load("images/title bg.png", -1, -1, 1);
	bg1->position = vector2d(0, 0);
}

void mainMenuToLevelOne()
{
	gf3d_ui_free_all_but_mouse();
	setupLevelOne();
}

void mainMenuToContentEditor() {
	gf3d_ui_free_all_but_mouse();
	setupContentEditor();
}

void setupLevelOne() {
	Entity_T* ent1 = modeled_entity_animated("teemo", "player", 0, 17);
	player = ent1;
	ent1->movetype = MOVETYPE_STEP;
	ent1->healthmax = 100.0f;
	ent1->health = ent1->healthmax;
	ent1->boundingBox.size.z *= 0.86f;
	ent1->boundingBox.size.x *= 0.70f;
	ent1->boundingBox.size.y *= 0.70f;
	ent1->maxspeed = vector3d(1000.0f, 1000.0f, 10000.0f);
	ent1->specFloat1 = 2000.0f;
	ent1->think = player_think;
	ent1->nextthink = 0.1f;
	ent1->model->boudningAdjustment.z = 1.2f;
	ent1->type = "player";
	ent1->data = 0;
	//save_entity_layout_json(player);

	//Entity_T* ent2 = modeled_entity_animated("ezreal", "Ezreal2", 0, 1);
	//ent2->movetype = MOVETYPE_STEP;
	//ent2->think = TestThink;
	//ent2->nextthink = 0.1f;
	//ent2->maxspeed = vector3d(0.0f, 0.0f, 10000.0f);
	//ent2->model->boudningAdjustment.z = -1.5f;

	Entity_T* ent3 = modeled_entity("ground", "ground");
	ent3->type = "ground";
	//save_entity_layout_json(ent3);
	Entity_T* ground2 = modeled_entity("ground", "ground2");
	Entity_T* ent4 = modeled_entity("platform_one", "platform");
	ent4->type = "platform";
	//save_entity_layout_json(ent4);
	Entity_T* ent5 = modeled_entity("platform_one", "plat2");
	Entity_T* ent6 = modeled_entity("platform_one", "plat3");
	//teleport_entity(ent2, vector3d(20, 0, 0));
	teleport_entity(ent3, vector3d(0, 0, -19));
	teleport_entity(ground2, vector3d(ground2->boundingBox.size.x*2, 0, -19));
	teleport_entity(ent4, vector3d(20, -40, -10));
	teleport_entity(ent5, vector3d(40, -40, 0));
	teleport_entity(ent6, vector3d(60, -40, 10));

	Entity_T* shroom1 = modeled_entity("shroom", "shroom1");
	shroom1->movetype = MOVETYPE_NOCLIP;
	shroom1->think = mushroom_think;
	shroom1->nextthink = 0.1f;
	shroom1->touch = mushroom_touch;
	shroom1->die = mushroom_die;
	shroom1->healthmax = 1.0f;
	shroom1->health = shroom1->healthmax;
	shroom1->type = "mushroom";
	//save_entity_layout_json(shroom1);

	teleport_entity(shroom1, vector3d(0, -40, -10));

	Entity_T* spring1 = modeled_entity("spring", "spring1");
	spring1->movetype = MOVETYPE_NOCLIP;
	spring1->think = spring_think;
	spring1->nextthink = 0.1f;
	spring1->touch = spring_touch;
	spring1->die = spring_die;
	spring1->healthmax = 1.0f;
	spring1->health = spring1->healthmax;
	spring1->type = "spring";
	//save_entity_layout_json(spring1);
	teleport_entity(spring1, vector3d(-20, -40, -10));

	Entity_T* speedup1 = modeled_entity("speed_up", "speedup1");
	speedup1->movetype = MOVETYPE_NOCLIP;
	speedup1->think = speed_up_think;
	speedup1->nextthink = 0.1f;
	speedup1->touch = speed_up_touch;
	speedup1->die = speed_up_die;
	speedup1->healthmax = 1.0f;
	speedup1->health = speedup1->healthmax;
	speedup1->type = "speed";
	//save_entity_layout_json(speedup1);
	teleport_entity(speedup1, vector3d(-40, -40, -10));

	Vector3D accelV3 = vector3d(10, 0, 0);
	Entity_T* pacer = modeled_entity_animated("penguin", "pacer", 0, 25);
	pacer->model->boudningAdjustment.z = -0.75f;
	pacer->movetype = MOVETYPE_STEP;
	pacer->nextthink = 0.1f;
	pacer->maxspeed = vector3d(900.0f, 900.0f, 10000.0f);
	pacer->healthmax = 1.0f;
	pacer->health = pacer->healthmax;
	pacer->think = pacer_think;
	pacer->nextthink = 0.1f;
	pacer->touch = pacer_touch;
	pacer->die = pacer_die;
	pacer->data2 = &accelV3;
	pacer->acceleration = accelV3;
	pacer->type = "pacer";
	//save_entity_layout_json(pacer);

	Entity_T* jumper = modeled_entity_animated("robot", "jumper", 0, 24);
	jumper->model->boudningAdjustment.z = -1.9f;
	jumper->model->boudningAdjustment.y = -1.9f;
	jumper->model->boudningAdjustment.x = -0.8f;
	jumper->boundingBox.size.z *= 0.8f;
	jumper->boundingBox.size.y *= 0.4f;
	jumper->movetype = MOVETYPE_STEP;
	jumper->nextthink = 0.1f;
	jumper->maxspeed = vector3d(800.0f, 800.0f, 10000.0f);
	jumper->healthmax = 1.0f;
	jumper->health = jumper->healthmax;
	jumper->think = jumper_think;
	jumper->nextthink = 0.1f;
	jumper->touch = jumper_touch;
	jumper->die = jumper_die;
	jumper->type = "jumper";
	//save_entity_layout_json(jumper);

	Entity_T* circler = modeled_entity_animated("circler", "circler", 0, 20);
	circler->model->boudningAdjustment.z = -0.9f;
	circler->movetype = MOVETYPE_STEP;
	circler->nextthink = 0.1f;
	circler->maxspeed = vector3d(800.0f, 800.0f, 10000.0f);
	circler->healthmax = 1.0f;
	circler->health = circler->healthmax;
	circler->think = circler_think;
	circler->nextthink = 0.1f;
	circler->touch = circler_touch;
	circler->die = circler_die;
	circler->type = "circler";
	//save_entity_layout_json(circler);

	teleport_entity(pacer, vector3d(20, 20, 0));
	teleport_entity(jumper, vector3d(60, -40, 20));
	teleport_entity(circler, vector3d(-60, 40, 0));

	rotate_entity(pacer, GFC_HALF_PI, vector3d(0, 0, 1));

	char str[14];
	char str2[14];

	sprintf(str, "Grzyby: %d", (int)player->data);
	sprintf(str2, "Zdrowie: %d", (int)player->health);

	UIElement* mushroomCount = gf3d_ui_placeText(str, 10, 10, vector4d(255, 255, 255, 255), 100);
	mushroomCount->name = "mushroom Count";

	UIElement* healthUI = gf3d_ui_placeText(str2, 10, 10, vector4d(255, 255, 255, 255), 100);
	healthUI->position.x = window_width - healthUI->sprite->frameWidth/2 - 10;
	healthUI->name = "health";

	state = GS_InGame;
}

void setupContentEditor() {
	Entity_T* axis = modeled_entity("axes", "axes");
	player = axis;
	axis->type = "axis";

	Entity_T* attach = modeled_entity("axes", "axes_attach");
	attach->model = NULL;
	attach->type = "axes_attach";

	UIElement* saveButton = gf3d_ui_placeText("Zapisac", 0, 5, vector4d(200,200,200,200), 75);
	saveButton->position.x = window_width / 2 - saveButton->sprite->frameWidth / 4;
	saveButton->name = "save button";
	saveButton->onClick = saveClick;

	char str[50];
	sprintf(str, "Pozycja: %f, %f, %f", player->position.x, player->position.y, player->position.z);
	UIElement* positionText = gf3d_ui_placeText(str, 5,5, vector4d(200,200,200,200), 75);
	positionText->name = "position";

	UIElement* platSpawnButton = gf3d_ui_placeText("Platforma", 5, 5, vector4d(200,200,200,200), 75);
	platSpawnButton->position.y = window_height - platSpawnButton->sprite->frameHeight / 2;
	platSpawnButton->onClick = platformClick;

	UIElement* playerSpawnButton = gf3d_ui_placeText("Odrodzenie gracza", 5,5, vector4d(200,200,200,200), 75);
	playerSpawnButton->position.y = platSpawnButton->position.y;
	playerSpawnButton->position.x = platSpawnButton->position.x + platSpawnButton->sprite->frameWidth / 2 + 20;
	playerSpawnButton->onClick = spawnClick;

	UIElement* groundSpawnButton = gf3d_ui_placeText("Ziemia", 5, 5, vector4d(200,200,200,200), 75);
	groundSpawnButton->position.y = platSpawnButton->position.y;
	groundSpawnButton->position.x = playerSpawnButton->position.x + playerSpawnButton->sprite->frameWidth / 2 + 20;
	groundSpawnButton->onClick = groundClick;

	UIElement* jumperSpawnButton = gf3d_ui_placeText("Skoczek", 5, 5, vector4d(200,200,200,200), 75);
	jumperSpawnButton->position.y = window_height - jumperSpawnButton->sprite->frameHeight / 2;
	jumperSpawnButton->position.x = window_width / 2 - jumperSpawnButton->sprite->frameWidth / 4;
	jumperSpawnButton->onClick = jumperClick;

	UIElement* pacerSpawnButton = gf3d_ui_placeText("Piechur", 5, 5, vector4d(200,200,200,200), 75);
	pacerSpawnButton->position.y = jumperSpawnButton->position.y;
	pacerSpawnButton->position.x = jumperSpawnButton->position.x + jumperSpawnButton->sprite->frameWidth/2 +20;
	pacerSpawnButton->onClick = pacerClick;

	UIElement* circlerSpawnButton = gf3d_ui_placeText("Krazy", 5, 5, vector4d(200,200,200,200), 75);
	circlerSpawnButton->position.y = jumperSpawnButton->position.y;
	circlerSpawnButton->position.x = jumperSpawnButton->position.x - circlerSpawnButton->sprite->frameWidth/2 - 20;
	circlerSpawnButton->onClick = circlerClick;

	UIElement* mushroomSpawnButton = gf3d_ui_placeText("Grzyb", 5, 5, vector4d(200,200,200,200), 75);
	mushroomSpawnButton->position.y = window_height - mushroomSpawnButton->sprite->frameHeight / 2;
	mushroomSpawnButton->position.x = window_width - mushroomSpawnButton->sprite->frameWidth / 2 - 5;
	mushroomSpawnButton->onClick = mushroomClick;

	UIElement* springSpawnButton = gf3d_ui_placeText("Sprezyna", 5, 5, vector4d(200,200,200,200), 75);
	springSpawnButton->position.y = mushroomSpawnButton->position.y;
	springSpawnButton->position.x = mushroomSpawnButton->position.x - springSpawnButton->sprite->frameWidth / 2 -20;
	springSpawnButton->onClick = springClick;

	UIElement* speedSpawnButton = gf3d_ui_placeText("Predkosc", 5, 5, vector4d(200,200,200,200), 75);
	speedSpawnButton->position.y = mushroomSpawnButton->position.y;
	speedSpawnButton->position.x = springSpawnButton->position.x - speedSpawnButton->sprite->frameWidth / 2 - 20;
	speedSpawnButton->onClick = speedClick;

	state = GS_InContentEditor;
}

int loadLevel(char* levelFile) {
	gf3d_entity_free_all();
	SJson* levelJson = sj_load(levelFile);
	if (!levelJson) {
		return 1;
	}
	SJson* entityArray = sj_object_get_value(levelJson, "entities");
	int i, count;
	float x, y, z;
	count = sj_array_get_count(entityArray);
	for (i = 0; i < count; i++) {
		SJson* element = sj_array_get_nth(entityArray, i);
		SJson* strObj = sj_object_get_value(element, "type");
		char* strChar = sj_get_string_value(strObj);
		Entity_T* spawnEnt = load_entity_json(strChar);

		if (strcmp(strChar, "player") == 0) {
			player = spawnEnt;
			spawnEnt->think = player_think;
			spawnEnt->die = player_die;
		}
		else if (strcmp(strChar, "mushroom") == 0) {
			spawnEnt->think = mushroom_think;
			spawnEnt->touch = mushroom_touch;
			spawnEnt->die = mushroom_die;
		}
		else if (strcmp(strChar, "spring") == 0) {
			spawnEnt->think = spring_think;
			spawnEnt->touch = spring_touch;
			spawnEnt->die = spring_die;
		}
		else if (strcmp(strChar, "speed") == 0) {
			spawnEnt->think = speed_up_think;
			spawnEnt->touch = speed_up_touch;
			spawnEnt->die = speed_up_die;
		}
		else if (strcmp(strChar, "pacer") == 0) {
			spawnEnt->think = pacer_think;
			spawnEnt->touch = pacer_touch;
			spawnEnt->die = pacer_die;
		}
		else if (strcmp(strChar, "jumper") == 0) {
			spawnEnt->think = jumper_think;
			spawnEnt->touch = jumper_touch;
			spawnEnt->die = jumper_die;
		}
		else if (strcmp(strChar, "circler") == 0) {
			spawnEnt->think = circler_think;
			spawnEnt->touch = circler_touch;
			spawnEnt->die = circler_die;
		}

		SJson* posArray = sj_object_get_value(element, "position");
		sj_get_float_value(sj_array_get_nth(posArray, 0), &x);
		sj_get_float_value(sj_array_get_nth(posArray, 1), &y);
		sj_get_float_value(sj_array_get_nth(posArray, 2), &z);
		teleport_entity(spawnEnt, vector3d(x, y, z));
		
	}

	if (!gf3d_ui_find("mushroom Count")) {
		char str[14];
		sprintf(str, "Grzyby: %d", (int)player->data);
		UIElement* mushroomCount = gf3d_ui_placeText(str, 10, 10, vector4d(255, 255, 255, 255), 100);
		mushroomCount->name = "mushroom Count";
	}
	else {
		UIElement* count = gf3d_ui_find("mushroom Count");
		char str[14];
		sprintf(str, "Grzyby: %d", (int)player->data);
		gf3d_sprite_free(count->sprite);
		count->sprite = gf3d_ui_getTextSprite(str, vector4d(255, 255, 255, 255), 100);
	}

	if (!gf3d_ui_find("health")) {
		char str2[14];
		sprintf(str2, "Zdrowie: %d", (int)player->health);
		UIElement* healthUI = gf3d_ui_placeText(str2, 10, 10, vector4d(255, 255, 255, 255), 100);
		healthUI->position.x = window_width - healthUI->sprite->frameWidth / 2 - 10;
		healthUI->name = "health";
	}
	else {
		UIElement* healthUI = gf3d_ui_find("health");
		char str2[14];
		sprintf(str2, "Zdrowie: %d", (int)player->health);
		gf3d_sprite_free(healthUI->sprite);
		healthUI->sprite = gf3d_ui_getTextSprite(str2, vector4d(255, 255, 255, 255), 100);
	}

	state = GS_InGame;
	strcpy(level.level_name, levelFile);
	strcpy(level.nextmap, sj_get_string_value(sj_object_get_value(levelJson, "loadNext")));
	return 0;
}

void toMainMenu() {
	gf3d_ui_free_all_but_mouse();
	gf3d_entity_free_all();
	setupMainMenu();
	state = GS_MainMenu;
	gf3d_ui_find("mouse")->hidden = false;
}

/*eol@eof*/