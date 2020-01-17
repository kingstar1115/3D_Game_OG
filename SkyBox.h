#pragma once
#include "resource.h"
#include "scene_node.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/string_cast.hpp>
namespace game {
	class SkyBox:public SceneNode
	{


	public:
		SkyBox(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture = NULL, const Resource *envmap = NULL);
		~SkyBox() {};
		
		
		virtual void Update(void);

	};
}