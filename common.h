#ifndef COMMON_H_
#define COMMON_H_

#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>


#include "resource.h"
#include "scene_node.h"

namespace game {

    // Abstraction of an asteroid
    class Common : public SceneNode {

        public:
            // Create asteroid from given resources
            Common(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture = NULL, const Resource *envmap = NULL);

            // Destructor
            ~Common();
            
            // Get/set attributes specific to asteroids
			float GetAngle(void) { return angle; }
			float GetAngleSpeed() { return angleSpeed; }
			glm::vec3 GetRotAxis() { return rotAxis; }
			float GetRotRange() { return rotRange; }
			float GetOffset() { return offset; }
			glm::vec3 GetTransAxis() { return transAxis; }
			float GetTransRange() { return transRange; }
			float GetTransSpeed() { return transSpeed; }

			float getMAnimation() { return animation_melee; }
			float getFAnimation() { return animation_fire; }
			float getSAnimation() { return animation_stun; }

			void SetAngle(float a) {angle = a; }
			void SetAngleSpeed(float as) { angleSpeed = as; }
			void SetRotAxis(glm::vec3 ra) { rotAxis = ra; }
			void SetRotRange(float rr) { rotRange = rr; }
			void SetOffset(float o) { offset = o; }
			void SetTransAxis(glm::vec3 ta) {transAxis = ta; }
			void SetTransRange(float tr) {transRange = tr; }
			void SetTransSpeed(float ts) {transSpeed = ts; }
			
			
			void setMAnimation(float a) { animation_melee = a; }
			void setFAnimation(float f) { animation_fire = f; }
			void setSAnimation(float s) { animation_stun = s; }

            // Update geometry configuration
            virtual void Update(void);
            
        private:
            // Angular momentum of asteroid
            float angle = 0;
			float angleSpeed = 0;
			glm::vec3 rotAxis = glm::vec3(0, 1, 0);
			float rotRange = 0;  // rotat in the range of angle that 

			float offset = 0;
			glm::vec3 transAxis = glm::vec3(0, 1, 0);
			float transRange = 0;
			float transSpeed = 0;


			// Ted add for melee attack
			float animation_melee = 0;
			float animation_fire = 0;
			float animation_stun = 0;

    }; // class Common

} // namespace game

#endif // ASTEROID_H_
