#ifndef WORLD_H
#define WORLD_H

#include "entity.h"
#include <vector>

//#include <iostream>
//#include <fstream>
//#include <string>
//using namespace std;

class World
{
public:
	float char_speed;
	const float low_lod_distance = 2.0f;
	const float no_render_distance = 200.0f;
	static World* instance;
	std::vector<EntityMesh*> entities;
	std::vector<EntityMeshSel*> selecting_entities;
	EntityMesh* character;
	EntityMesh* floor;
	EntityMesh* sky;

	EntityMesh* selected_entity;
	float min_sel_dist;

	bool free_camera;
	bool render_bounding;
	EntityCamera* camera;

	World();
	//vector<string> split(string str, const char* pattern);
	//void loadStage(const char* file_path);
	void load_floor();
	void load_sky();
	void load_character();
	void addEntity(const char* texture_path, const char* mesh_path, Vector3 position, const char* name, const char* type);
	void selectEntity(Vector2 mouse_position, int window_width, int window_height);
	void render();
	void detectCollisions(float elapsed_time, Vector3& target_pos);
};


#endif