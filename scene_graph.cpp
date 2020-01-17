#include <stdexcept>
#include <iostream>
#include <fstream>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include "scene_graph.h"

namespace game {

SceneGraph::SceneGraph(void){

    background_color_ = glm::vec3(0.0, 0.0, 0.0);
}


SceneGraph::~SceneGraph(){
}


void SceneGraph::SetBackgroundColor(glm::vec3 color){

    background_color_ = color;
}


glm::vec3 SceneGraph::GetBackgroundColor(void) const {

    return background_color_;
}
 

SceneNode *SceneGraph::CreateNode(std::string node_name, Resource *geometry, Resource *material){

    // Create scene node with the specified resources
    SceneNode *scn = new SceneNode(node_name, geometry, material);

    // Add node to the scene
	AddNode(scn);

    return scn;
}

void SceneGraph::AddNode(SceneNode *node) {
	hieNodeList.push_back(node);
	return;
}


SceneNode* SceneGraph::GetNode(std::string node_name){

    // Find node with the specified name
    for (int i = 0; i < hieNodeList.size(); i++){
		if (hieNodeList[i]->GetName() == node_name) {
				return hieNodeList[i];
		}
		else if(hieNodeList[i]->GetChildren()->size()>0){
			SceneNode* r = hieNodeList[i]->FindIt(node_name);
			if (r != NULL) {
				return r;
			}
		}
    }
	std::cout << node_name << " not found" << std::endl;
    return NULL;
}


/*
std::vector<std::vector<SceneNode *>>::const_iterator SceneGraph::begin() const {

    return node_.begin();
}


std::vector<SceneNode *>::const_iterator SceneGraph::end() const {

    return node_.end();
}
*/

void SceneGraph::Draw(Camera *camera){

    // Clear background
    glClearColor(background_color_[0], 
                 background_color_[1],
                 background_color_[2], 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	for (int i = 0; i < hieNodeList.size(); i++) {
		hieNodeList[i]->Draw(camera);
	}

}

void SceneGraph::SaveTexture(char *filename) {

	unsigned char data[FRAME_BUFFER_WIDTH*FRAME_BUFFER_HEIGHT * 4];

	// Retrieve image data from texture
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_);
	glReadPixels(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, data);

	// Create file in ppm format
	// Open the file
	std::ofstream f;
	f.open(filename);
	if (f.fail()) {
		throw(std::ios_base::failure(std::string("Error opening file ") + std::string(filename)));
	}

	// Write header
	f << "P3" << std::endl;
	f << FRAME_BUFFER_WIDTH << " " << FRAME_BUFFER_HEIGHT << std::endl;
	f << "255" << std::endl;

	// Write data
	for (int i = 0; i < FRAME_BUFFER_HEIGHT; i++) {
		for (int j = 0; j < FRAME_BUFFER_WIDTH; j++) {
			for (int k = 0; k < 3; k++) {
				int dt = data[i*FRAME_BUFFER_WIDTH * 4 + j * 4 + k];
				f << dt << " ";
			}
		}
		f << std::endl;
	}

	// Close the file
	f.close();

	// Reset frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneGraph::SetupDrawToTexture(void) {

	// Set up frame buffer
	glGenFramebuffers(1, &frame_buffer_);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_);

	// Set up target texture for rendering
	glGenTextures(1, &texture_);
	glBindTexture(GL_TEXTURE_2D, texture_);

	// Set up an image for the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Set up a depth buffer for rendering
	glGenRenderbuffers(1, &depth_buffer_);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer_);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

	// Configure frame buffer (attach rendering buffers)
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer_);
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);

	// Check if frame buffer was setup successfully 
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw(std::ios_base::failure(std::string("Error setting up frame buffer")));
	}

	// Reset frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Set up quad for drawing to the screen
	static const GLfloat quad_vertex_data[] = {
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
	};

	// Create buffer for quad
	glGenBuffers(1, &quad_array_buffer_);
	glBindBuffer(GL_ARRAY_BUFFER, quad_array_buffer_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertex_data), quad_vertex_data, GL_STATIC_DRAW);
}



void SceneGraph::DrawToTexture(Camera *camera) {

	// Save current viewport
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	// Enable frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_);
	glViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

	// Clear background
	glClearColor(background_color_[0],
		background_color_[1],
		background_color_[2], 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw all scene nodes
	for (int i = 0; i < hieNodeList.size(); i++) {
		hieNodeList[i]->Draw(camera);
	}

	// Reset frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Restore viewport
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}


void SceneGraph::DisplayTexture(GLuint program, int type) {

	// Configure output to the screen
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);

	// Set up quad geometry
	glBindBuffer(GL_ARRAY_BUFFER, quad_array_buffer_);

	// Select proper material (shader program)
	glUseProgram(program);

	// Setup attributes of screen-space shader
	GLint pos_att = glGetAttribLocation(program, "position");
	glEnableVertexAttribArray(pos_att);
	glVertexAttribPointer(pos_att, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

	GLint tex_att = glGetAttribLocation(program, "uv");
	glEnableVertexAttribArray(tex_att);
	glVertexAttribPointer(tex_att, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));

	// Timer
	GLint timer_var = glGetUniformLocation(program, "timer");
	float current_time = glfwGetTime();
	glUniform1f(timer_var, current_time);

	// Effect type
	GLint type_var = glGetUniformLocation(program, "type");
	glUniform1i(type_var, type);

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_);

	// Draw geometry
	glDrawArrays(GL_TRIANGLES, 0, 6); // Quad: 6 coordinates

	// Reset current geometry
	glEnable(GL_DEPTH_TEST);
}

// handle melee, catch chicken
int SceneGraph::delicious(glm::vec3 pos) {
	int Eat = 0;
	std::vector<SceneNode*> BeenEat;
	for (int i = 0; i < hieNodeList.size(); i++) {
		SceneNode* curr = hieNodeList[i];
		if (curr->GetName() == "Hen_Body" || curr->GetName() == "CK_Body") {
			float distance = glm::length(curr->GetPosition() - pos);
			// Hen successfully protect chicken
			if (curr->GetName() == "Hen_Body" && distance <= 10 && curr->getStun()<=0) {
				return -1;
			}
			if (curr->GetName() == "CK_Body" && distance <= 10) {
				Eat += 1;
				BeenEat.push_back(curr);
				
			}
		}
	}
	
	for (int j = 0; j < BeenEat.size(); j++) {
		BeenEat[j]->SetShouldBeDestoried(true);
	}
	return Eat;
}


glm::vec3 SceneGraph::getRandomPos(float radius, glm::vec3 pos, float footvalue) {
	float x = rand() / (float)RAND_MAX*radius;
	float z = rand() / (float)RAND_MAX*radius;
	float x_flag = rand() % 2 + 1;
	float z_flag = rand() % 2 + 1;
	if (x_flag == 1) { x *= -1; }
	if (z_flag == 1) { z *= -1; }
	glm::vec3 POS = glm::vec3(pos.x + x, -footvalue, pos.z + z);
	return POS;
}

void SceneGraph::Update(void) {
	// temp position, shared from hen to chicken, the 'escape' range
	glm::vec3 escape;
	SceneNode* Bird = this->GetNode("Camera");
	SceneNode* act_bird = this->GetNode("Body");

	for (int i = 0; i < hieNodeList.size(); i++) {
		SceneNode* curr = hieNodeList[i];
		// calculate distance to target
		glm::vec3 targer_pos = curr->getTarget();
		glm::vec3 poss = curr->GetPosition();
		float dis_target = sqrt((targer_pos.x - poss.x)*(targer_pos.x - poss.x) + (targer_pos.y - poss.y)*(targer_pos.y - poss.y) + (targer_pos.z - poss.z)*(targer_pos.z - poss.z));
		// calculate distance to bird

		glm::vec3 poss_bird = Bird->GetPosition();
		float dis_bird = sqrt((poss.x - poss_bird.x)*(poss.x - poss_bird.x) + (poss.y - poss_bird.y)*(poss.y - poss_bird.y) + (poss.z - poss_bird.z)*(poss.z - poss_bird.z));

		// AI logic for drone**************************************************************************************************************
		// *********************************************************************************************************************************
		if (curr->GetName() == "Drone_Body") {
			
			if (dis_target < 2) {
				// factor: how far infront of bird the drone will chase
				float LastR = curr->getRest();
				curr->setRest(LastR - 1);
				curr->SetSpeed(0.00);
				if (curr->getRest() <= 0) {
					float factor = 5.0;
					glm::vec3 target = Bird->GetPosition() + Bird->GetForward()*Bird->GetSpeed();
					curr->setTarget(target);
					//reset rest
					float restt = rand() % 300 + 1;
					curr->setRest(restt);

				}
			}

			else {
				// set direction
				curr->SetSpeed(0.03);
				curr->SetForward(curr->getTarget() - curr->GetPosition());
			}

			// collision between drone and bird
			if (dis_bird < 3){
				Bird->setStun(90);
				curr->SetShouldBeDestoried(true);
			}
		}

		// AI logic for Hen**************************************************************************************************************
		// *********************************************************************************************************************************
		if (curr->GetName() == "Hen_Body") {
			// If Bird is far from hen, hen will move to random place
			// Else hen move to a point that bird might come
			glm::vec3 a = Bird->GetPosition();
			glm::vec3 b = curr->GetPosition();
			float distance_from_bird = sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y) + (a.z - b.z)*(a.z - b.z));
			// normal
			if (distance_from_bird >= 40) {
				curr->setMoving_Center(glm::vec3(0.0, -23.7, 0.0));
				curr->setMoving_Range_radius(150);
			}
			// warning
			else {
				glm::vec3 POS = glm::vec3(Bird->GetPosition().x, -23.7, Bird->GetPosition().z) + glm::vec3(Bird->GetForward().x * (Bird->GetPosition().y + 23.7), 0, Bird->GetForward().z * (Bird->GetPosition().y + 23.7));
				if (POS.x >= 130) { POS = glm::vec3(130, POS.y, POS.z); }
				if (POS.x <= -130) { POS = glm::vec3(-130, POS.y, POS.z); }
				if (POS.z >= 130) { POS = glm::vec3(POS.x, POS.y, 130); }
				if (POS.z <= -130) { POS = glm::vec3(POS.x, POS.y, -130); }
				escape = POS;
				curr->setMoving_Center(POS);
				curr->setMoving_Range_radius(20);
			}
			if (dis_target < 2) {
				float LastR = curr->getRest();
				curr->setRest(LastR - 1);
				curr->SetSpeed(0);
				if (curr->getRest() <= 0) {
					glm::vec3 target = getRandomPos(curr->getMoving_Range_radius(), curr->getMoving_Center(), -23.7);
					curr->setTarget(target);
					//reset rest
					float restt = rand() % 200 + 100;
					curr->setRest(restt);

					//get correct oriantation
					glm::vec3 a = curr->GetForward();
					glm::vec3 b = curr->getTarget() - curr->GetPosition();
					float angle = acos(glm::dot(a, b) / (glm::length(a)*glm::length(b)));
					float dir = a.x*b.z - a.z*b.x > 0 ? -1.0f : 1.0f;
					angle = angle * 180 / glm::pi<float>();
					glm::quat rotation = glm::angleAxis(dir*angle * glm::pi<float>() / 180.0f, glm::vec3(0, 1, 0));
					curr->Rotate(rotation);

				}
			}

			else {
				// set direction
				curr->SetSpeed(0.04);
				
			}

			float diss_suck = glm::length(suck - b);
			if (diss_suck <= 10) {
				curr->setStun(50);
			}


			// handle stun
			if (curr->getStun() >= 0) {
				curr->SetSpeed(0.0);
			}
		}

		// AI logic for chicken**************************************************************************************************************
		// *********************************************************************************************************************************
		if (curr->GetName() == "CK_Body") {
			glm::vec3 a = curr->GetPosition();
			float distance_from_escape = sqrt((a.x - escape.x)*(a.x - escape.x) + (a.y - escape.y)*(a.y - escape.y) + (a.z - escape.z)*(a.z - escape.z));
			// warned
			if (distance_from_escape < 7) {
				float factor = 20;
				glm::vec3 targe = curr->GetPosition() + (curr->GetPosition() - escape) * factor;
				curr->setTarget(targe);
				//get correct oriantation
				glm::vec3 a = curr->GetForward();
				glm::vec3 b = curr->getTarget() - curr->GetPosition();
				float angle = acos(glm::dot(a, b) / (glm::length(a)*glm::length(b)));
				float dir = a.x*b.z - a.z*b.x > 0 ? -1.0f : 1.0f;
				angle = angle * 180 / glm::pi<float>();
				glm::quat rotation = glm::angleAxis(dir*angle * glm::pi<float>() / 180.0f, glm::vec3(0, 1, 0));
				curr->Rotate(rotation);
			}
			// normal
			if (dis_target < 2) {
				float LastR = curr->getRest();
				curr->setRest(LastR - 1);
				curr->SetSpeed(0);
				if (curr->getRest() <= 0) {
					glm::vec3 target = getRandomPos(150, glm::vec3(0.0, -24.3, 0.0), -24.3);
					curr->setTarget(target);
					//reset rest
					float restt = rand() % 300 + 400;
					curr->setRest(restt);

					//get correct oriantation
					glm::vec3 a = curr->GetForward();
					glm::vec3 b = curr->getTarget() - curr->GetPosition();
					float angle = acos(glm::dot(a, b) / (glm::length(a)*glm::length(b)));
					float dir = a.x*b.z - a.z*b.x > 0 ? -1.0f : 1.0f;
					angle = angle * 180 / glm::pi<float>();
					glm::quat rotation = glm::angleAxis(dir*angle * glm::pi<float>() / 180.0f, glm::vec3(0, 1, 0));
					curr->Rotate(rotation);
				}
			}
			else {
				// set direction
				curr->SetSpeed(0.024);
			}

			// tonado suck
			float diss_suck = glm::length(suck - a);
			if (diss_suck <= 10) {
				curr->SetPosition(suck);
			}

		}

		// collision logic for missile ************************************************************************************
		// *********************************************************************************************************************************
		if (curr->GetName() == "Missile") {
			for (int j = 0; j < hieNodeList.size(); j++) {
				SceneNode* current = hieNodeList[j];
				float distance = glm::length(current->GetPosition() - curr->GetPosition());
				// collision between missile and drone
				if (current->GetName() == "Drone_Body") {
					if (distance <= 1.2) {
						current->SetShouldBeDestoried(true);
						curr->SetShouldBeDestoried(true);
					}
				}
				// collision between missile and hen
				if (current->GetName() == "Hen_Body") {
					if (distance <= 2.1) {
						current->setStun(50);
						curr->SetShouldBeDestoried(true);
					}
				}
			}
		}

		if (curr->GetName() == "Skybox") {
			curr->SetPosition(Bird->GetPosition());
		}

		// update position and rotation
		curr->Update();
	}
}


} // namespace game