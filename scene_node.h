#ifndef SCENE_NODE_H_
#define SCENE_NODE_H_

#include <string>
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>

#include "resource.h"
#include "camera.h"

namespace game {

    // Class that manages one object in a scene 
    class SceneNode {

        public:
            // Create scene node from given resources
			SceneNode(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture = NULL, const Resource *envmap = NULL);

			SceneNode * FindIt(std::string node_name);

			// Destructor
            ~SceneNode();

            // Get name of node
            std::string GetName(void);
			void SetName(std::string n) { name_ = n; }

            // Get node attributes
			virtual glm::vec3 GetPosition(void) const;
			virtual glm::quat GetOrientation(void) const;
			float GetSpeed() { return speed; }
			float GetAcceleration() { return acceleration_; }
			float GetMaxSpeed() { return maxSpeed; }
			float GetFictionFactor() { return fictionFactor; }
			glm::vec3 GetScale(void) const;
			glm::mat4 GetTransFMat();
			glm::vec3 GetOrigin() { return rotOrigin; }
			SceneNode* GetParent() { return parent; }
			std::vector <SceneNode*> * GetChildren() { return children; }
			glm::vec3 GetForward() { return orientation_ * forward_; }
			glm::vec3 GetUp() { return orientation_ * up_; }
			glm::vec3 GetSide() { return orientation_ * side_; }

            // Set node attributes
			virtual void SetPosition(glm::vec3 position);
			virtual void SetOrientation(glm::quat orientation);
			void SetSpeed(float s) { speed = s; }
			void SetAcceleration(float a) { acceleration_ = a; }
			void SetMaxSpeed(float ms) { maxSpeed = ms; }
			void SetFictionFactor(float ff) { fictionFactor = ff; }
			void SetScale(glm::vec3 scale);
			void SetTransMatrix(glm::mat4 tm) { transfMatrix = tm; }
			void SetOrigin(glm::vec3 o) { rotOrigin = o; }
			void SetParent(SceneNode* p);
			void AddChild(SceneNode *c);
			void SetForward(glm::vec3 f) { forward_ = f; }


            // Perform transformations on node
			virtual void Translate(glm::vec3 trans);
			virtual void Rotate(glm::quat rot);
			void Scale(glm::vec3 scale);

            // Draw the node according to scene parameters in 'camera'
            // variable
            virtual void Draw(Camera *camera);


			void UpdateNodeInfo(void);

            // Update the node
			virtual void Update(void) {};


            // OpenGL variables
            GLenum GetMode(void) const;
            GLuint GetArrayBuffer(void) const;
            GLuint GetElementArrayBuffer(void) const;
            GLsizei GetSize(void) const;
            GLuint GetMaterial(void) const;

			// life time and destroy
			bool GetShouldBeDestoried() { return shouldBeDestoried; }
			bool GetRenderState() { return constRender; }
			float GetLifeTime() { return lifeTime; }
			float GetRenderTime() { return renderTime; }

			void SetShouldBeDestoried(bool sb) { shouldBeDestoried = sb; }
			void SetRenderState(bool rs) { constRender = rs; }
			void SetLifeTime(float lt) { lifeTime = lt; }
			void SetRenderTiem(float rt) { renderTime = rt; }

			void RemoveChild(SceneNode * child);

			//getter, setter for stun
			float getStun() { return stun; }
			void setStun(float s) { stun = s; }
			//getter, setter for target position
			glm::vec3 getTarget() { return target; }
			void setTarget(glm::vec3 t) { target = t; }
			//getter, setter for rest time
			float getRest() { return rest; }
			void setRest(float r) { rest = r; }
			//getter, setter for moving center and range
			glm::vec3 getMoving_Center() { return moving_Center; }
			void setMoving_Center(glm::vec3 c) { moving_Center = c; }
			float getMoving_Range_radius() { return moving_Range_radius; }
			void setMoving_Range_radius(float r) { moving_Range_radius = r; }


        protected:
			GLuint texture_; // Reference to texture resource
			GLuint envmap_; // Reference to environment map
			bool blending_; // Draw with blending or not

            std::string name_; // Name of the scene node
            GLuint array_buffer_; // References to geometry: vertex and array buffers
            GLuint element_array_buffer_;
            GLenum mode_; // Type of geometry
            GLsizei size_; // Number of primitives in geometry
            GLuint material_; // Reference to shader program
            glm::vec3 position_; // Position of node
            glm::quat orientation_; // Orientation of node
            glm::vec3 scale_; // Scale of node
			glm::vec3 rotOrigin = glm::vec3(0); // the position of rotation origin
			float speed = 0;	// velocity of node
			float acceleration_ = 0; // acceleration of node
			float maxSpeed = 0.3;	// max speed of general object if not specified
			float fictionFactor = 0; // fiction force of general object if not specified

            // Set matrices that transform the node in a shader program
			virtual void SetupShader(GLuint program,Camera* camera);



			// matrixs for transform
			glm::mat4 transfMatrix = glm::mat4(1.0);
			glm::mat4 scalingMatrix = glm::mat4(1.0);

			// parent 
			SceneNode* parent = NULL;
			std::vector<SceneNode*> * children;
					   			
			// intial direction
			glm::vec3 forward_ = glm::vec3(0, 0, -1);
			glm::vec3 side_ = glm::vec3(1, 0, 0);
			glm::vec3 up_ = glm::vec3(0, 1, 0);
			

			bool shouldBeDestoried = false; // if this object should be destoried
			bool constRender = true ;
			float lifeTime = 0;
			float renderTime = 0; // the time of object has been rendered


			// bool stun, only affect bird and hen. If > 0, then stun
			float stun = -1.0;

			// glm::vec3 target position, only affect for drone and hen
			glm::vec3 target;
			float rest = 111;//drone will stop when move to target for some time

			// moving center and moving range for chicken and hen
			glm::vec3 moving_Center = glm::vec3(0.0, -24.9, 0.0);
			float moving_Range_radius = 130;

			// how far can object move far from origin
			float dis_from_origin = 150;
			// ego cant reach ground
			float min_height = -22;

    }; // class SceneNode

} // namespace game

#endif // SCENE_NODE_H_
