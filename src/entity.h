#ifndef ENTITY_H
#define ENTITY_H

//#include "game.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
//#include "fbo.h"
#include "shader.h"
//#include "input.h"
//#include "animation.h"
#include "camera.h"
//#include <cmath>

class Entity
{
public:
    std::string name;
    Matrix44 model;
    Vector3 pos;

    virtual void render();
    virtual void update(float elapsed_time);

    Entity();
    virtual ~Entity();
};

class EntityMesh : public Entity
{
public:    
    Mesh* mesh;
    Texture* texture;
    Shader* shader;
    Vector4 color;
    float tiling = 1.0f;
    EntityMesh() : Entity() {};
   
    void render();
    void OnCollision(EntityMesh* other);
    // void update(float dt);
    //void detectColisions();
};

class EntityMeshSel : public EntityMesh
{
public:
    bool selected = false;
    EntityMeshSel() : EntityMesh() {};

    // void update(float dt);
    //void detectColisions();
};
class EntityCamera : public Entity
{
public:
    // Cambiar a cam para diferenciar mejor
    Camera* cam = new Camera();

    EntityCamera() : Entity() {};
};
#endif