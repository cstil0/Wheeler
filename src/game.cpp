#include "game.h"
//#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "fbo.h"
#include "shader.h"
#include "input.h"
#include "animation.h"
//#include "entity.h"
#include "world.h"
#include "stage.h"

#include <cmath>

//some globals
Animation* anim = NULL;
float mouse_speed = 100.0f;

Game* Game::instance = NULL;
World world;

IntroStage* intro_stage = NULL;
PlayStage* play_stage = NULL;
EndStage* end_stage = NULL;
Stage* current_stage = NULL;

Game::Game(int window_width, int window_height, SDL_Window* window)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	instance = this;
	must_exit = false;

	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;
	mouse_locked = false;

	intro_stage = new IntroStage();
	play_stage = new PlayStage();
	end_stage = new EndStage();

	current_stage = (Stage*)intro_stage;
	current_stage->type = Stage::TYPE::INTRO;

	current_stage->loadStage("data/entities.txt", world, "entity");
	current_stage->loadStage("data/selecting_entities.txt", world, "selecting_entity");

	//OpenGL flags
	glEnable( GL_CULL_FACE ); //render both sides of every triangle
	glEnable( GL_DEPTH_TEST ); //check the occlusions using the Z buffer

	world.load_character();
	world.load_sky();
	world.load_floor();

	//world.addEntity("data/Character/color-atlas-new.png", "data/Character/palm-round_73.obj", Vector3(6.5f, 3.893f, 0.0f));
	/*world.addEntity("data/Character/color-atlas-new.png", "data/Character/palm-small_74.obj", Vector3(6.5f, 0.0f, -3.893f));
	world.addEntity("data/Character/color-atlas-new.png", "data/Character/penguin_20.obj", Vector3(6.0f, 0.0f, 0.0f));
	world.addEntity("data/Character/color-atlas-new.png", "data/Character/bus-school_6.obj", Vector3(0.0f, 0.0f, 0.0f));*/


	//Camera* camera = world.camera->camera;
	//world.camera->camera = new Camera();
	world.camera->cam->setPerspective(70.f, window_width / (float)window_height, 0.1f, 1000000.f);

	//Matrix44* char_model = &world.character->model;
	//Vector3 eye = world.character->model * Vector3(0.0f, 10.0f, 5.0f);
	//Vector3 center = world.character->model * Vector3(0.0f, 0.0f, -2.0f);
	//Vector3 up = world.character->model.rotateVector(Vector3(0.0f, 1.0f, 0.0f));
	//world.camera->camera->lookAt(eye, center, up);

	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse
}

//what to do when the image has to be draw
void Game::render(void) {

	//set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	current_stage->render(world);

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{
	float speed = seconds_elapsed * mouse_speed; //the speed is defined by the seconds_elapsed so it goes constant

	//mouse input to rotate the cam
	if ((Input::mouse_state & SDL_BUTTON_LEFT) || mouse_locked) //is left button pressed?
	{
		world.camera->cam->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
		world.camera->cam->rotate(Input::mouse_delta.y * 0.005f, world.camera->cam->getLocalVector(Vector3(-1.0f, 0.0f, 0.0f)));
	}

	if (world.free_camera) {
		//async input to move the camera around
		if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift
		if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) world.camera->cam->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) world.camera->cam->move(Vector3(0.0f, 0.0f, -1.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) world.camera->cam->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) world.camera->cam->move(Vector3(-1.0f, 0.0f, 0.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_E)) world.camera->cam->move(Vector3(0.0f, -1.0f, 0.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_Q)) world.camera->cam->move(Vector3(-1.0f, 1.0f, 0.0f) * speed);
	}
	else {
		float dist = world.char_speed * seconds_elapsed;
		EntityMesh* character = world.character;
		Vector3 curr_dist = Vector3(0,0,0);
		Vector3 curr_pos = character->pos;
		// Calculamos el target
		Vector3 target_pos = curr_pos;
		//int tupac = 0;

		if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) {
			//tupac += 1;
			target_pos.z = curr_pos.z - dist;
			// Miramos si ese target está disponible y si lo está esta misma función cambia la pos del personaje
			world.detectCollisions(seconds_elapsed, target_pos);

			// Calculamos cuanto nos hemos desplazado (no se si esto es muy eficiente, pero es lo mejor que se me ha ocurrido por ahora)
			curr_dist.z += target_pos.z - curr_pos.z;
			curr_dist.x += target_pos.x - curr_pos.x;

			// Trasladamos el modelo
			character->model.translate(curr_dist.x, 0.0f, curr_dist.z);
			curr_dist.x = 0;
			curr_dist.z = 0;
		}

		else if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) {
			//tupac += 1;

			target_pos.z = curr_pos.z + dist;
			world.detectCollisions(seconds_elapsed, target_pos);
			curr_dist.z += target_pos.z - curr_pos.z;
			curr_dist.x += target_pos.x - curr_pos.x;

			// Trasladamos el modelo

			character->model.translate(curr_dist.x, 0.0f, curr_dist.z);
			curr_dist.x = 0;
			curr_dist.z = 0;
		}

		else if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) {
			//tupac += 1;

			target_pos.x = curr_pos.x - dist;
			world.detectCollisions(seconds_elapsed, target_pos);
			curr_dist.z += target_pos.z - curr_pos.z;
			curr_dist.x += target_pos.x - curr_pos.x;

			// Trasladamos el modelo

			character->model.translate(curr_dist.x, 0.0f, curr_dist.z);
			curr_dist.x = 0;
			curr_dist.z = 0;
		}

		else if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) {
			//tupac += 1;

			target_pos.x = curr_pos.x + dist;
			world.detectCollisions(seconds_elapsed, target_pos);

			curr_dist.z += target_pos.z - curr_pos.z;
			curr_dist.x += target_pos.x - curr_pos.x;

			// Trasladamos el modelo

			character->model.translate(curr_dist.x, 0.0f, curr_dist.z);
			curr_dist.x = 0;
			curr_dist.z = 0;
		}

		if (Input::isKeyPressed(SDL_SCANCODE_E)) {
			character->model.rotate(30.0f * seconds_elapsed * DEG2RAD, Vector3(0.0f, 1.0f, 0.0f));
		}
		if (Input::isKeyPressed(SDL_SCANCODE_Q)) {
			character->model.rotate(-30.0f * seconds_elapsed * DEG2RAD, Vector3(0.0f, 1.0f, 0.0f));
		}

		//if (tupac > 1) character->model.translate(curr_dist.x/2, 0.0f, curr_dist.z/2);
		//else character->model.translate(curr_dist.x, 0.0f, curr_dist.z);

		curr_dist.x = 0;
		curr_dist.z = 0;

		//std::cout << character->pos.x;
		//std::cout << " ";
		//std::cout << character->pos.z;
		//std::cout << "\n";

		}


		if (Input::wasKeyPressed(SDL_SCANCODE_TAB)) {
			world.free_camera = !world.free_camera;
		}

		// Key to go directly to the play stage
		if (Input::wasKeyPressed(SDL_SCANCODE_P)) {
			current_stage = (Stage*)play_stage;
			current_stage->type = Stage::TYPE::PLAY;
		}

		if (Input::wasKeyPressed(SDL_SCANCODE_B)) {
			world.render_bounding = true;
		}

		////async input to move the camera around
		//if(Input::isKeyPressed(SDL_SCANCODE_LSHIFT) ) speed *= 10; //move faster with left shift
		//if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
		//if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) camera->move(Vector3(0.0f, 0.0f,-1.0f) * speed);
		//if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
		//if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) camera->move(Vector3(-1.0f,0.0f, 0.0f) * speed);

		//to navigate with the mouse fixed in the middle
		if (mouse_locked)
			Input::centerMouse();
	}


//Keyboard event handler (sync input)
void Game::onKeyDown( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: must_exit = true; break; //ESC key, kill the app
		case SDLK_F1: Shader::ReloadAll(); break; 
		case SDLK_h: world.selectEntity(Input::mouse_position, window_width, window_height); break;
	}
}

void Game::onKeyUp(SDL_KeyboardEvent event)
{
}

void Game::onGamepadButtonDown(SDL_JoyButtonEvent event)
{

}

void Game::onGamepadButtonUp(SDL_JoyButtonEvent event)
{

}

void Game::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_MIDDLE) //middle mouse
	{
		mouse_locked = !mouse_locked;
		SDL_ShowCursor(!mouse_locked);
	}
	if (event.button == SDL_BUTTON_LEFT) {
		world.selectEntity(Input::mouse_position, window_width, window_height);
	}
}

void Game::onMouseButtonUp(SDL_MouseButtonEvent event)
{
}

void Game::onMouseWheel(SDL_MouseWheelEvent event)
{
	mouse_speed *= event.y > 0 ? 1.1 : 0.9;
}

void Game::onResize(int width, int height)
{
    std::cout << "window resized: " << width << "," << height << std::endl;
	glViewport( 0,0, width, height );
	camera->aspect =  width / (float)height;
	window_width = width;
	window_height = height;
}

