#ifndef SCENE_GRAPH_H_
#define SCENE_GRAPH_H_

#include <string>
#include <vector>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "scene_node.h"
#include "resource.h"
#include "missile.h"
#include "camera.h"
#include "SkyBox.h"
#include "common.h"

#define FRAME_BUFFER_WIDTH 1024
#define FRAME_BUFFER_HEIGHT 768

namespace game {

    // Class that manages all the objects in a scene
    class SceneGraph {

        private:
            // Background color
            glm::vec3 background_color_;

            // Scene nodes to render
            //std::vector<SceneNode *> node_;
			std::vector<SceneNode*> hieNodeList;

			GLuint frame_buffer_;
			// Quad vertex array for drawing from texture
			GLuint quad_array_buffer_;
			// Render targets
			GLuint texture_;
			GLuint depth_buffer_;

			// inital a point thats impossible to reach
			glm::vec3 suck = glm::vec3(999,999,999);
        public:
            // Constructor and destructor
            SceneGraph(void);
            ~SceneGraph();

            // Background color
            void SetBackgroundColor(glm::vec3 color);
            glm::vec3 GetBackgroundColor(void) const;
            
            // Create a scene node from the specified resources
            SceneNode *CreateNode(std::string node_name, Resource *geometry, Resource *material);

            // Add an already-created node
			void AddNode(SceneNode *node);
			// Find a list of nodes with a specific name
			SceneNode* GetNode(std::string node_name);

            // Draw the entire scene
            void Draw(Camera *camera);

			void SaveTexture(char * filename);

			void SetupDrawToTexture(void);

			void DrawToTexture(Camera * camera);

			void DisplayTexture(GLuint program, int type);

			glm::vec3 getRandomPos(float radius, glm::vec3 pos, float footvalue);

			// Update entire scene
            void Update(void);

			// Get Node list
			std::vector<SceneNode*> GetNodeList() { return hieNodeList; }
			void SetNodeList(std::vector<SceneNode*> n) { hieNodeList = n; }

			int delicious(glm::vec3 pos);

			void setSuck(glm::vec3 s) { suck = s; }

    }; // class SceneGraph

} // namespace game

#endif // SCENE_GRAPH_H_
