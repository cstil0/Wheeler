#ifndef STAGE_H
#define STAGE_H

#include "world.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

//Crear clases hijo para cada stage
//Cada stage se inicializa con sus entities

class Stage {
public:
	enum TYPE {
		INTRO,
		PLAY,
		END
	};

	TYPE type;
	TYPE getCurrentStage() { return type; };

	Stage() {};
	virtual void render(World& world) {};
	vector<string> split(string str, const char* pattern);
	void loadStage(const char* file_path, World& world, const char* type);
};

class IntroStage : public Stage {
public:
	IntroStage() : Stage() {};
	virtual void render(World& world);
};

class PlayStage : public Stage {
public:
	PlayStage() :Stage() {};
	virtual void render(World& world);
};

class EndStage : public Stage {
public:
	EndStage() :Stage() {};
	virtual void render(World& world);
};

#endif