#pragma once

#include "scene_node.h"
#include "scene_graph.h"
#include "asteroid.h"
#include <vector>
#include<algorithm>
#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
namespace game {
	class Missile:public SceneNode
	{
		struct AandT{
			Asteroid* target;
			float time;

			bool operator<(AandT* another) {
				return time < another->time;
			}
		};
	public:
		Missile(const std::string name, const Resource *geometry, const Resource *material, const Resource *textrue = NULL, const Resource *envmap = NULL);
		~Missile() {};

		//void selectTarget(std::vector<SceneNode*> nodelist);
		void Update();

	private:
		std::vector<AandT*>* targetList;
	private:
	

	};


}