#include "camera_node.h"

game::CameraNode::CameraNode(Camera * c, std::string cameraName) :SceneNode(cameraName, NULL, NULL) {
	camera = c; 
	camera->SetCameraName(cameraName); 
	orientation_ = camera->GetOrientation();
	position_ = camera->GetPosition();
}

glm::vec3 game::CameraNode::GetPosition(void) const
{
	return camera->GetPosition();
}

glm::quat game::CameraNode::GetOrientation(void) const
{
	return camera->GetOrientation();
}

void game::CameraNode::Pitch(float angle)
{
	camera->Pitch(angle);
	orientation_ = camera->GetOrientation();
}

void game::CameraNode::Yaw(float angle)
{
	camera->Yaw(angle);
	orientation_ = camera->GetOrientation();
}

void game::CameraNode::Roll(float angle)
{
	camera->Roll(angle);
	orientation_ = camera->GetOrientation();
}

void game::CameraNode::SetPosition(glm::vec3 position)
{
	position_ = position;
	camera->SetPosition(position_);

}

void game::CameraNode::SetOrientation(glm::quat orientation)
{
	orientation_ = orientation;
	camera->SetOrientation(orientation_);
}


void game::CameraNode::Draw(Camera * camera)
{
	for (int i = 0; i < (*children).size(); i++) {
		if ((*children)[i]->GetName() != "Body" && camera->GetCameraName() == "Camera") (*children)[i]->Draw(camera);
		else if (camera->GetCameraName() == "ThirdCamera" || camera->GetCameraName() == "OverlookCamera")(*children)[i]->Draw(camera);

	}
}

void game::CameraNode::Update(void)
{
	if (GetCamera()->GetCameraName() == "Camera") {
		camera->SetLookAt(position_ + (float)800 * camera->GetForward());
	}
	else if (GetCamera()->GetCameraName()=="ThirdCamera") {
		camera->SetLookAt(parent->GetPosition());
		position_ = parent->GetPosition() + glm::vec3(-8, 12, 32);
	}

	
	/*
	if (GetCamera()->GetCameraName() == "ThirdCamera") {
		camera->SetLookAt(parent->GetPosition());
	
		glm::vec3 cur_forward = parent->GetOrientation()*parent->GetForward();
		glm::vec3 cur_up = parent->GetOrientation()*parent->GetUp();
		glm::vec3 cur_side = parent->GetOrientation()*parent->GetSide();

		position_ = parent->GetPosition() + glm::vec3(-2.0f*cur_forward, 6.0f*cur_up, -10.0f*cur_side);

	}
	else { camera->SetLookAt(position_ + (float)800 * camera->GetForward()); }
	*/

	SceneNode::UpdateNodeInfo();
	camera->SetPosition(position_);
	camera->SetOrientation(orientation_);
	if (children->size() > 0) {
		for (int i = 0; i < children->size(); i++) {
			(*children)[i]->Update();
		}
	}
}
