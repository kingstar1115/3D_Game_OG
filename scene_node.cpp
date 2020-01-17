#include <stdexcept>
#define GLM_FORCE_RADIANS
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <time.h>

#include "scene_node.h"

namespace game {

	SceneNode::SceneNode(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture, const Resource *envmap) {

		// Set name of scene node
		name_ = name;

		// Set geometry
		if (geometry) {
			if (geometry->GetType() == PointSet) {
				mode_ = GL_POINTS;
			}
			else if (geometry->GetType() == Mesh) {
				mode_ = GL_TRIANGLES;
			}
			else {
				throw(std::invalid_argument(std::string("Invalid type of geometry")));
			}
			array_buffer_ = geometry->GetArrayBuffer();
			element_array_buffer_ = geometry->GetElementArrayBuffer();
			size_ = geometry->GetSize();
		}

		if (material) {
			// Set material (shader program)
			if (material->GetType() != Material) {
				throw(std::invalid_argument(std::string("Invalid type of material")));
			}

			material_ = material->GetResource();
		}
					   
		// Set texture
		if (texture) {
			texture_ = texture->GetResource();
		}
		else {
			texture_ = 0;
		}

		// Set environment map texture
		if (envmap) {
			envmap_ = envmap->GetResource();
		}
		else {
			envmap_ = 0;
		}

		// Other attributes
		scale_ = glm::vec3(1.0, 1.0, 1.0);
		blending_ = false;
		children = new std::vector<SceneNode*>();
	}



	SceneNode* SceneNode::FindIt(std::string node_name) {
		for (int i = 0; i < children->size(); i++) {
			if ((*children)[i]->GetName() == node_name) {
				return (*children)[i];
			}
			else if ((*children)[i]->GetChildren()->size() > 0) {
				SceneNode * r = (*children)[i]->FindIt(node_name);
				if (r != NULL) {
					return r;
				}
			}
		}
		return NULL;
	}

SceneNode::~SceneNode(){
}


std::string SceneNode::GetName(void) {

    return name_;
}


glm::vec3 SceneNode::GetPosition(void) const {

    return position_;
}


glm::quat SceneNode::GetOrientation(void) const {

    return orientation_;
}


glm::vec3 SceneNode::GetScale(void) const {

    return scale_;
}

glm::mat4 SceneNode::GetTransFMat()
{
	return transfMatrix;
}


void SceneNode::SetPosition(glm::vec3 position){

    position_ = position;
}


void SceneNode::SetOrientation(glm::quat orientation){

    orientation_ = orientation;
}


void SceneNode::SetScale(glm::vec3 scale){

    scale_ = scale;
}

void SceneNode::SetParent(SceneNode * p)
{
	p->AddChild(this);
	parent = p; 
}

void SceneNode::AddChild(SceneNode * c)
{
	children->push_back(c);
}


void SceneNode::Translate(glm::vec3 trans){

    position_ += trans;
}


void SceneNode::Rotate(glm::quat rot){

    orientation_ *= rot;
    orientation_ = glm::normalize(orientation_);
}


void SceneNode::Scale(glm::vec3 scale){

    scale_ *= scale;
}


GLenum SceneNode::GetMode(void) const {

    return mode_;
}


GLuint SceneNode::GetArrayBuffer(void) const {

    return array_buffer_;
}


GLuint SceneNode::GetElementArrayBuffer(void) const {

    return element_array_buffer_;
}


GLsizei SceneNode::GetSize(void) const {

    return size_;
}


GLuint SceneNode::GetMaterial(void) const {

    return material_;
}


void SceneNode::Draw(Camera *camera){

	if (blending_) {
		// Disable z-buffer
		glDisable(GL_DEPTH_TEST);

		// Enable blending
		glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Simpler form
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquationSeparate(GL_FUNC_ADD, GL_MAX);
	}
	else {
		// Enable z-buffer
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
	}

	// Select proper material (shader program)
	glUseProgram(material_);

	// Set geometry to draw
	glBindBuffer(GL_ARRAY_BUFFER, array_buffer_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer_);

	// Set globals for camera
	camera->SetupShader(material_);

	// Set world matrix and other shader input variables
	SetupShader(material_,camera);

	// Draw geometry
	if (mode_ == GL_POINTS) {
		glDrawArrays(mode_, 0, size_);
	}
	else {
		glDrawElements(mode_, size_, GL_UNSIGNED_INT, 0);
	}

	if (children->size() > 0) {
		for (int i = 0; i < children->size(); i++) {
			(*children)[i]->Draw(camera);
		}
	}
}


void SceneNode::UpdateNodeInfo(void){

	if (!constRender) { 
		renderTime += 0.01; 
		if (renderTime >= lifeTime)shouldBeDestoried = true;
	}
	float theSpeed = 0;
	stun--;
	if (stun <= 0) {
		speed = speed + acceleration_ > maxSpeed ?
			maxSpeed : speed + acceleration_;

		speed = speed + acceleration_ <= 0 ?
			0 : speed + acceleration_;

		if (speed >= 0) {
			speed = speed > fictionFactor ?
				speed - fictionFactor : 0;
		}
		theSpeed = speed;
	}
	else {
		theSpeed = 0;
	}

	glm::vec3 velocity = theSpeed * glm::normalize(orientation_*forward_);

	if (name_ == "Camera") {
		glm::vec3 nextPos = position_ + velocity;
		if (nextPos.y <= -23.5) {
			velocity.y = 0;
		}
		if(glm::length(nextPos - glm::vec3(0, -25, 0)) > 150) {
			velocity = glm::vec3(0);
		}
	}
	position_ += velocity;

	scalingMatrix = glm::scale(glm::mat4(1.0), scale_);
	glm::mat4 rotation = glm::mat4_cast(orientation_);
	glm::mat4 translation = glm::translate(glm::mat4(1.0), position_);
	glm::mat4 origin = glm::translate(glm::mat4(1.0), -rotOrigin);
	
	if (parent == NULL) { transfMatrix = translation * rotation * origin * scalingMatrix; }
	else { transfMatrix = (parent->GetTransFMat()) *  translation *  rotation * origin * scalingMatrix; }


}


void SceneNode::SetupShader(GLuint program, Camera* camera){

    // Set attributes for shaders
    GLint vertex_att = glGetAttribLocation(program, "vertex");
    glVertexAttribPointer(vertex_att, 3, GL_FLOAT, GL_FALSE, 11*sizeof(GLfloat), 0);
    glEnableVertexAttribArray(vertex_att);

    GLint normal_att = glGetAttribLocation(program, "normal");
    glVertexAttribPointer(normal_att, 3, GL_FLOAT, GL_FALSE, 11*sizeof(GLfloat), (void *) (3*sizeof(GLfloat)));
    glEnableVertexAttribArray(normal_att);

    GLint color_att = glGetAttribLocation(program, "color");
    glVertexAttribPointer(color_att, 3, GL_FLOAT, GL_FALSE, 11*sizeof(GLfloat), (void *) (6*sizeof(GLfloat)));
    glEnableVertexAttribArray(color_att);

    GLint tex_att = glGetAttribLocation(program, "uv");
    glVertexAttribPointer(tex_att, 2, GL_FLOAT, GL_FALSE, 11*sizeof(GLfloat), (void *) (9*sizeof(GLfloat)));
    glEnableVertexAttribArray(tex_att);



    // World transformation
    

    glm::mat4 transf = transfMatrix;

	GLint world_mat = glGetUniformLocation(program, "world_mat");
	glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(transf));


	// Normal matrix
	glm::mat4 normal_matrix = glm::transpose(glm::inverse(transf));
	GLint normal_mat = glGetUniformLocation(program, "normal_mat");
	glUniformMatrix4fv(normal_mat, 1, GL_FALSE, glm::value_ptr(normal_matrix));

	// Normal matrix with view added
	glm::mat4 normal_view_matrix = glm::transpose(glm::inverse(camera->GetCurrentViewMatrix() * transf));
	GLint normal_view_mat = glGetUniformLocation(program, "normal_view_mat");
	glUniformMatrix4fv(normal_view_mat, 1, GL_FALSE, glm::value_ptr(normal_view_matrix));

	// Texture
	if (texture_) {
		GLint tex = glGetUniformLocation(program, "texture_map");
		glUniform1i(tex, 0); // Assign the first texture to the map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_); // First texture we bind
		// Define texture interpolation
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	// Environment map
	if (envmap_) {
		GLint tex = glGetUniformLocation(program, "env_map");
		glUniform1i(tex, 1); // Assign the first texture to the map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envmap_); // First texture we bind
		// Define texture interpolation
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	// Timer
	GLint timer_var = glGetUniformLocation(program, "timer");
	double current_time = glfwGetTime();
	glUniform1f(timer_var, (float)current_time);
}

void SceneNode::RemoveChild(SceneNode * child)
{
	std::vector<SceneNode*>::iterator it;
	for (it = (*children).begin(); it != (*children).end();) {
		if (child->GetName() == (*it)->GetName()) {
			it = children->erase(it);
		}
		else {
			it++;
		}
	}
}

} // namespace game;
