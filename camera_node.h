#include "camera.h"
#include "scene_node.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/string_cast.hpp>
namespace game {
	class CameraNode:public SceneNode
	{


	public:
		CameraNode(Camera* c, std::string cameraName);
		~CameraNode() {};

		Camera* GetCamera() { return camera; }
		void SetCamera(Camera* c) { camera = c; }
		
		virtual glm::vec3 GetPosition(void) const;
		virtual glm::quat GetOrientation(void) const;
		int GetView() { return view; }

		void Pitch(float angle);
		void Yaw(float angle);
		void Roll(float angle);

		virtual void SetPosition(glm::vec3 position);
		virtual void SetOrientation(glm::quat orientation);

		virtual void Draw(Camera *camera);
		virtual void Update(void);


	private:
		Camera* camera;
		GLuint program;
		int view = 1; // first view 1, third view -1


	};
}