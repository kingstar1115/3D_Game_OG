#ifndef ASTEROID_H_
#define ASTEROID_H_

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
    class Asteroid : public SceneNode {

        public:
            // Create asteroid from given resources
            Asteroid(const std::string name, const Resource *geometry, const Resource *material);

            // Destructor
            ~Asteroid();
            
            // Get/set attributes specific to asteroids
            glm::quat GetAngM(void) const;
            void SetAngM(glm::quat angm);

            // Update geometry configuration
            void Update(void);
            
        private:
            // Angular momentum of asteroid
            glm::quat angm_;
    }; // class Asteroid

} // namespace game

#endif // ASTEROID_H_
