#include "missile.h"

namespace game {
	Missile::Missile(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture, const Resource *):SceneNode(name, geometry, material)
	{

	}
	/*
	void Missile::selectTarget(std::vector<SceneNode*> nodelist)
	{
		
		targetList = new std::vector<AandT*>();

		glm::vec3 oc;
		glm::vec3 unit = orientation_ * forward_;

		for (int i = 0; i < nodelist.size(); i++) {
			SceneNode* current = nodelist[i];
			while (current != NULL) {
				if (current->GetName().find("AsteroidInstance") == 0) {
					oc = position_ - current->GetPosition();
					float b = 2.0f * glm::dot(unit,oc);
					float c = glm::dot(oc,oc) - current->GetScale().x*current->GetScale().x;
					float discriminant = (b * b) - (4.0f * c);

					if (discriminant > 0.0f) {
						discriminant = sqrt(discriminant);

						float far = (-b + discriminant) / 2.0f/100/speed;
						float near = (-b - discriminant) / 2.0f/100 / speed;

						if (near >= 0 && near < lifeTime) {
							AandT* temp = new AandT();
							temp->target = (Asteroid*)current;
							temp->time = near;
							targetList->push_back(temp);
						}
						else if (far >= 0 && near < lifeTime) {
							AandT* temp = new AandT();
							temp->target = (Asteroid*)current;
							temp->time = far;
							targetList->push_back(temp);
						}
					}
				}
				current = current->GetChild();
			}
		}

	}
	*/
	void Missile::Update()
	{
		/*
		if (targetList->size() > 0) {

			std::vector<AandT*>::iterator it;
			for (it = targetList->begin(); it != targetList->end();) {
				if ((*it)->target->GetShouldBeDestoried()){
					it = targetList->erase(it); 
				}
				else ++it;
			}

			std::sort(targetList->begin(), targetList->end());
			Asteroid* nearestTarget = (*targetList->begin())->target;
			float leastTime = (*targetList->begin())->time;

			if (renderTime >= leastTime) {
				shouldBeDestoried = true;
				nearestTarget->SetShouldBeDestoried(true);
			}
		}
		*/
		SceneNode::UpdateNodeInfo();

	}
	

}

