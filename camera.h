#ifndef CAMERA_H_
#define CAMERA_H_

#define GLEW_STATIC
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

namespace game {

    // Abstraction of a camera
    class Camera {

        public:
            Camera(void);
            ~Camera();
 
            // Get global camera attributes
            glm::vec3 GetPosition(void) const;
            glm::quat GetOrientation(void) const;
			std::string GetCameraName() { return name; }
			glm::mat4 GetCurrentViewMatrix() { return view_matrix_; }

            // Set global camera attributes
            void SetPosition(glm::vec3 position);
            void SetOrientation(glm::quat orientation);
			void SetCameraName(std::string n) { name = n; }
			void SetLookAt(glm::vec3 la) { look_at = la; }
            
            // Perform global transformations of camera
            void Translate(glm::vec3 trans);
            void Rotate(glm::quat rot);

            // Get relative attributes of camera
            glm::vec3 GetForward(void) const;
            glm::vec3 GetSide(void) const;
            glm::vec3 GetUp(void) const;
			glm::vec3 GetLookAt() { return look_at; }

            // Perform relative transformations of camera
            void Pitch(float angle);
            void Yaw(float angle);
            void Roll(float angle);

            // Set the view from camera parameters: initial position of camera,
            // point looking at, and up vector
            // Resets the current orientation and position of the camera
            void SetView(glm::vec3 position, glm::vec3 look_at, glm::vec3 up);
            // Set projection from frustum parameters: field-of-view,
            // near and far planes, and width and height of viewport
            void SetProjection(GLfloat fov, GLfloat near, GLfloat far, GLfloat w, GLfloat h);
            // Set all camera-related variables in shader program
            void SetupShader(GLuint program);
			int GetView() { return view; }

        private:
			std::string name; // Camera name, used for identifing different camera
			glm::vec3 position_; // Position of camera
            glm::quat orientation_; // Orientation of camera
            glm::vec3 forward_; // Initial forward vector
            glm::vec3 side_; // Initial side vector
            glm::mat4 view_matrix_; // View matrix
            glm::mat4 projection_matrix_; // Projection matrix
			glm::vec3 look_at = glm::vec3(0);
            // Create view matrix from current camera parameters
            void SetupViewMatrix(void);
			int view = 1; // first view 1, third view -1

    }; // class Camera

} // namespace game

#endif // CAMERA_H_
