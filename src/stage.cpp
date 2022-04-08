#include "stage.h"
#include "camera.h"
//#include "world.h"

vector<string> Stage::split(string str, const char* pattern) {
	int posInit = 0;
	int posFound = 0;
	string splitted;
	vector<string> results;

	while (posFound >= 0) {
		posFound = str.find(pattern, posInit);
		splitted = str.substr(posInit, posFound - posInit);
		posInit = posFound + 1;
		results.push_back(splitted);
	}
	return results;
}

void Stage::loadStage(const char* file_path, World& world, const char* type) {
	string myText;

	ifstream MyReadFile(file_path);

	bool first_line = true;
	while (getline(MyReadFile, myText)) {
		if (first_line) {
			int num_entities = std::stof(myText);
			// Reservamos el espacio de entities para que los pushbacks sean mucho más rapidos
			if (type == "entity") {
				world.entities.reserve(num_entities);
			}
			else {
				world.selecting_entities.reserve(num_entities);
			}
			first_line = false;
		}
		else {
			vector<string> splitted = split(myText, ",");

			const char* texture_path;
			string texture_str(splitted[0]);
			texture_path = texture_str.c_str();

			const char* mesh_path;
			string mesh_str(splitted[1]);
			mesh_path = mesh_str.c_str();

			//const int n_t = splitted[0].length();
			//char texture_path[splitted[0].length() + 1];
			//strcpy(texture_path, splitted[0].c_str());

			//const int n_m = splitted[1].length();
			//char mesh_path[n_m + 1];
			//strcpy(mesh_path, splitted[1].c_str());
			world.addEntity(texture_path, mesh_path, Vector3(std::stof(splitted[2]), std::stof(splitted[3]), std::stof(splitted[4])), splitted[5].c_str(), type);
		}
	}

	// Close the file
	MyReadFile.close();
}

void IntroStage::render(World& world) {

}

void PlayStage::render(World& world) {
	Camera* camera = world.camera->cam;

	//set the camera as default
	camera->enable();

	//set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	//Matrix44 char_model = world.character->model;
	//world.camera->camera->eye = char_model * Vector3(0.0f, 2.0f, 2.0f);
	//world.camera->camera->center = char_model * Vector3(0.0f, 0.0f, -2.0f);
	//world.camera->camera->up = char_model.rotateVector(Vector3(0.0f, 1.0f, 0.0f));
	//// Estaria bien limpiar este carro de punteros
	//world.camera->camera->lookAt(world.camera->camera->eye, world.camera->camera->center, world.camera->camera->up);

	world.render();

	//Draw the floor grid
	drawGrid();

	//render the FPS, Draw Calls, etc
	drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);
}

void EndStage::render(World& world) {

}