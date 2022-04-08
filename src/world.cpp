#include "world.h"
#include "camera.h"

World* World::instance = NULL;

World::World()
{
	instance = this;
	char_speed = 10.0f;

	character = new EntityMesh();
	sky = new EntityMesh();
	floor = new EntityMesh();

	selected_entity = NULL;
	min_sel_dist = 30.0f;

	free_camera = false;
	render_bounding = false;
	camera = new EntityCamera();

	//Matrix44 model;
	//EntityMesh* entity = new EntityMesh();
	//entity->mesh = Mesh::Get("data/Character/boy-beach_0.obj");
	//entity->texture = Texture::Get("data/Character/color-atlas-new.png");
	//entity->shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");

	//entity->model = model;
	////entity->model.translate(position.x, position.y, position.z);

	//entities.push_back(entity);

	//addEntity("data/Character/color-atlas-new.png", "data/Character/test.obj", Vector3(0.0f, 0.0f, 0.0f));
}

void World::load_floor() {
	floor->texture = Texture::Get("data/rock_texture.png");
	floor->mesh = Mesh::Get("data/floor.obj");
	floor->shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	Matrix44 model_char;
	floor->model = model_char;
	floor->tiling = 250.0f;
	floor->model.translate(camera->cam->eye.x, camera->cam->eye.y, camera->cam->eye.z);
}

void World::load_sky() {
	sky->texture = Texture::Get("data/cielo.tga");
	sky->mesh = Mesh::Get("data/cielo.ASE");
	sky->shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	Matrix44 model_char;
	sky->model = model_char;
	sky->model.translate(camera->cam->eye.x, camera->cam->eye.y, camera->cam->eye.z);
}

void World::load_character() {
	character->texture = Texture::Get("data/color-atlas-new.png");
	character->mesh = Mesh::Get("data/Character/man-golf.obj");
	character->shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	Matrix44 model_char;
	character->model = model_char;
	character->pos = Vector3(0.0f, 0.0f, 0.0f);
	character->model.translate(0.0f, 0.0f, 0.0f);
	//character->model.rotate(180.0f * DEG2RAD, Vector3(0.0f, 1.0f, 0.0f));

	//character->model.rotate(-PI/2, Vector3(0.0f, 1.0f, 0.0f));

	//int count = 0;
	//for (int i = 0; i < tiles_width; i++) {
	//	for (int j = 0; j < tiles_height; j++) {
	//		Matrix44 model;
	//		tiles[count].texture = Texture::Get("data/color-atlas-new.png");
	//		tiles[count].model = model;
	//		tiles[count].mesh = Mesh::Get("data/Tiles/tile-plain_sand.obj");
	//		tiles[count].shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	//		tiles[count].model.translate(tiles_distance * (float)j, 0.0f, tiles_distance * (float)i);
	//		//entities.push_back(&tiles[count]);
	//		count++;
	//	}
	//}
}

void World::addEntity(const char* texture_path, const char* mesh_path, Vector3 position, const char* name, const char* type) {
	if (type == "entity"){
		Matrix44 model;
		EntityMesh* entity = new EntityMesh();
		entity->texture = Texture::Get(texture_path);
		entity->mesh = Mesh::Get(mesh_path);
		entity->shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");

		entity->model = model;
		entity->model.translate(position.x, position.y, position.z);
		entity->pos = Vector3(position.x, position.y, position.z);
		entity->name = name;
		entities.push_back(entity);
	}
	else {
		Matrix44 model;
		EntityMeshSel* entity_sel= new EntityMeshSel();
		entity_sel->texture = Texture::Get(texture_path);
		entity_sel->mesh = Mesh::Get(mesh_path);
		entity_sel->shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");

		entity_sel->model = model;
		entity_sel->model.translate(position.x, position.y, position.z);
		entity_sel->pos = Vector3(position.x, position.y, position.z);
		entity_sel->name = name;
		selecting_entities.push_back(entity_sel);
	}
}

void World::selectEntity(Vector2 mouse_position, int window_width, int window_height)
{
	Vector3 origin = camera->cam->eye;
	// Con esto lanzamos exactamente donde esta el mouse
	// Esto también puede ser muy util para funcionalidades de seleccionar cosas con el mouse
	Vector3 dir = camera->cam->getRayDirection(mouse_position.x, mouse_position.y, window_width, window_height);

	// Recorremos todas las entidades
	for (size_t i = 0; i < selecting_entities.size(); i++) {
		EntityMeshSel* current = selecting_entities[i];
		Vector3 col;
		Vector3 normal;
		if (current->selected) continue;
		if (!current->mesh->testRayCollision(current->model, origin, dir, col, normal)) continue;
		// Si no estamos a una distancia minima del objeto no podemos seleccionarlo
		float dist = abs(current->pos.z - character->pos.z);
		if (dist > min_sel_dist) continue;
		// Si colisionamos con esa entidad, la seleccionamos
		selected_entity = current;
		current->selected = true;
		current->name += "  *";
		break;
	}

}

void World::render()
{
	sky->render();
	floor->render();
	character->render();

	Matrix44 char_model = character->model;
	if (!free_camera) {
		//this->camera->camera->rotate(PI, Vector3(0.0f, 1.0f, 0.0f));
		this->camera->cam->eye = char_model * Vector3(0.0f, 3.0f, 3.0f);
		this->camera->cam->up = char_model.rotateVector(Vector3(0.0f, 1.0f, 0.0f));
		this->camera->cam->center = char_model * Vector3(0.0f, 0.0f, -5.0f);
		// Estaria bien limpiar este carro de punteros
		this->camera->cam->lookAt(camera->cam->eye, camera->cam->center, camera->cam->up);
	}

	for (int i = 0; i < entities.size(); i++) {
		Vector3 current_pos_world = entities[i]->model * entities[i]->mesh->box.center;
		if (!camera->cam->testSphereInFrustum(current_pos_world, entities[i]->mesh->radius)) continue;
		float dist = current_pos_world.distance(camera->cam->eye);

		if (dist > no_render_distance) continue;
		if (dist > low_lod_distance) {
			entities[i]->render();
		}

		if (render_bounding) {
			entities[i]->mesh->renderBounding(entities[i]->model);
		}
	}
	for (int i = 0; i < selecting_entities.size(); i++) {
		drawText(10, 50 + i * 5, selecting_entities[i]->name, Vector3(1, 1, 1), 2);
		// Estaria bien guardar el current entity para no acceder cada vez
		if (selecting_entities[i]->selected) continue;
		Vector3 current_pos_world = selecting_entities[i]->model * selecting_entities[i]->mesh->box.center;
		if (!camera->cam->testSphereInFrustum(current_pos_world, selecting_entities[i]->mesh->radius)) continue;
		float dist = current_pos_world.distance(camera->cam->eye);

		if (dist > no_render_distance) continue;
		if (dist > low_lod_distance) {
			selecting_entities[i]->render();
		}
		if (render_bounding) {
			selecting_entities[i]->mesh->renderBounding(entities[i]->model);
		}
	}
}

void World::detectCollisions(float elapsed_time, Vector3& target_pos)
{
	// Esto se podria meter en un operador Vector3 + float
	//Vector3 target_pos = Vector3(character->pos.x + char_speed, character->pos.y + char_speed, character->pos.z);
	Vector3 character_target_center = target_pos + Vector3(0, 1, 0);

	//std::cout << (character->pos.z);
	//std::cout << "\n";
	//std::cout << "\n";

	for (size_t i = 0; i < entities.size(); i++) {
		EntityMesh* curr_entity = entities[i];

		Vector3 coll;
		Vector3 collnorm;
		if (!curr_entity->mesh->testSphereCollision(curr_entity->model, character_target_center, 0.7f, coll, collnorm))
			continue;

		// Esto nos devuelve el vector hacia donde se tiene que mover el personaje
		Vector3 push_away = normalize(coll - character_target_center) * elapsed_time;
		//if (curr_entity->name == "bus") std::cout << curr_entity->pos.z;
		std::cout << curr_entity->name + "\n";
		//target_pos = character->pos - push_away;
		target_pos = character->pos;

		std::cout << (character->pos.x);
		std::cout << "\n";
		std::cout << (target_pos.x);

		std::cout << "\n";


		// Este código es para hacer alguna funcionalidad especifica cuando el player colisiona con algo
		//if (curr_entity->testCollision(character)) {
		//	curr_entity->OnCollision(character);
		//	character->OnCollision(curr_entity);
		//}
	}

	character->pos = target_pos;

}

