#include "entity.h"

Entity::Entity()
{
}

Entity::~Entity()
{
}

void Entity::render()
{
}

void Entity::update(float elapsed_time)
{
}

void EntityMesh::render() {
    //get the last camera that was activated
    Camera* camera = Camera::current;
    Matrix44 model = this->model;

    //enable shader and pass uniforms
    shader->enable();
    shader->setUniform("u_color", Vector4(1, 1, 1, 1));
    shader->setUniform("u_model", model);
    shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
    shader->setUniform("u_texture", texture, 0);
    shader->setUniform("u_texture_tiling", tiling);

    //render the mesh using the shader
    mesh->render(GL_TRIANGLES);

    //disable the shader after finishing rendering
    shader->disable();
}

// La idea de estas funciones es que si algo colisiona con la entidad hacemos algo (bajar vida por ejemplo)
// Podemos hacer que sea virtual y resolverla a algo dependiendo de que tipo sea la entidad. De momento esta es general
void EntityMesh::OnCollision(EntityMesh* other)
{

}
