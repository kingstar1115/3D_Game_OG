#include "Particle.h"
#include <stdexcept>
#define GLM_FORCE_RADIANS
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <time.h>


namespace game {
	Particle::Particle(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture, const Resource *):SceneNode(name, geometry, material)
	{

	}
	
	void Particle::Update()
	{
		
		SceneNode::UpdateNodeInfo();

	}
	
	void Particle::SetupShader(GLuint program, Camera* camera) {
		SceneNode::SetupShader(program,camera);

		GLint flag_var = glGetUniformLocation(program, "flag");
		if (type == "Feather") {
			glUniform1i(flag_var, 1);
		}
		if (type == "Tornado") {
			glUniform1i(flag_var, 2);
		}
		if (type == "Explosion") {
			glUniform1i(flag_var, 3);
		}
		
	}

}