#include "common.h"

namespace game {

Common::Common(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture, const Resource *envmap) 
		: SceneNode(name, geometry, material,texture,envmap) {
}


Common::~Common(){
}




void Common::Update(void){
	
	if ((angleSpeed !=0) && (rotRange != 0) &&(abs(angle + angleSpeed) >= abs(rotRange))) {
		angleSpeed *= -1;
	}
	angle += angleSpeed;
	Rotate(glm::normalize(glm::angleAxis(angleSpeed*glm::pi<float>(), rotAxis)));

	if ((transSpeed != 0) && (transRange != 0) && (abs(offset + transSpeed) >= abs(transRange))) {
		transSpeed *= -1;
	}
	offset += transSpeed;
	Translate(offset* transRange *transAxis);

	SceneNode::UpdateNodeInfo();

	if (children->size() > 0) {
		for (int i = 0; i < children->size(); i++) {
			(*children)[i]->Update();
		}
	}

	// melee control
	if (name_ == "Body") {
		if (animation_melee < 0) {
			SetAngleSpeed(0);
		}
		else {
			animation_melee -= 1;
		}
	}

	// fire control
	animation_fire -= 1;
	if (animation_fire < 0 && abs(angleSpeed) * 1000 == 20) {
		if (name_ == "LWing" || name_ == "LWing_tip") {
			angleSpeed = 0.01;
		}
		if (name_ == "RWing" || name_ == "RWing_tip") {
			angleSpeed = -0.01;
		}
	}
}
            
} // namespace game
