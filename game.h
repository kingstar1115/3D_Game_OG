#ifndef GAME_H_
#define GAME_H_

#include <exception>
#include <string>
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "scene_graph.h"
#include "resource_manager.h"
#include "camera.h"
#include "camera_node.h"
#include "SkyBox.h"
#include "common.h"
#include "missile.h"
#include "Particle.h"


namespace game {

	// Exception type for the game
	class GameException : public std::exception
	{
	private:
		std::string message_;
	public:
		GameException(std::string message) : message_(message) {};
		virtual const char* what() const throw() { return message_.c_str(); };
		virtual ~GameException() throw() {};
	};

	struct ResourceSet {
		Resource* g = NULL;
		Resource* m = NULL;
		Resource* t = NULL;
		Resource* e = NULL;
	};

	typedef enum GameState { Begining, Playing, HappyEnd, SadEnd } GameStep;
	typedef enum ObjectState { Normal, Stun, BulletTime} StateType;

	// Game application
	class Game {

	public:
		// Constructor and destructor
		Game(void);
		~Game();
		void fire();
		void CreateParticleTornado(glm::vec3 pos);
		Particle * CreateParticleFeather();
		// Call Init() before calling any other method
		void Init(void);
		// Set up resources for the game
		void SetupResources(void);
		// Set up initial scene
		void SetupScene(void);
		// Create object
		void CreatHouse(glm::vec3 pos);
		void CreateBird(glm::vec3 pos);
		void CreateDrone(glm::vec3 pos);
		void CreateAim(glm::vec3 pos);
		void CreateChicken(glm::vec3 pos);
		void CreateHen(glm::vec3 pos);
		// Run the game: keep the application active
		void MainLoop(void);


		// control tornado
		bool getLast_View() { return viewFlag; }
		void setLast_View(bool v) { viewFlag = v; }
		bool getTornado() { return tornado; }
		void setTornado(bool t) { tornado = t; }

		glm::vec3 getRandomPos();
		void spawnChicken();
		void spawnDrone();

	private:
		// GLFW window
		GLFWwindow* window_;

		// Scene graph containing all nodes to render
		SceneGraph scene_;

		// Resources available to the game
		ResourceManager resman_;

		// Flag to turn animation on/off
		bool animating_;

		// The number of screen effect type
		int type_ = 0;

		// Methods to initialize the game
		void InitWindow(void);
		void InitView(void);
		void InitEventHandlers(void);

		// Methods to handle events
		static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void ResizeCallback(GLFWwindow* window, int width, int height);

		// Camera abstraction
		CameraNode * current_camera;
		CameraNode * first_view_camera;
		CameraNode * third_view_camera;
		CameraNode * overlook_camera;
		SkyBox * skybox_;
		void CreateMissile(int dir);
		Particle* CreateExplosion(glm::vec3 pos);

		ResourceSet CollectSource(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name, std::string envmap_name);

		SkyBox * CreateSkyBoxInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name = std::string(""), std::string envmap_name = std::string(""));

		Common * CreateCommonInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name = std::string(""), std::string envmap_name = std::string(""));

		Missile * CreateMissileInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name = std::string(""), std::string envmap_name = std::string(""));

		Particle * CreateParticleInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name = std::string(""), std::string envmap_name = std::string(""));

		void resetGame();
		void SetUpCamera();

		void CreateUI();
		void CreateScreen();

		void DrawUI();
		void RenderScreen(GameState);
	
		int view = 1; // first view 1, third view -1
		bool thirdview = false; // if the view is needed to be changed
		float firecooldown = 0.0; // the current cool down time of firing 
		float firegap = 0.5;	// the less gap time of firing

		StateType playerstate = Normal;
		GameStep gameStep = Begining;

		float health = 50;
		float energy = 50;
		float max_health = 100;
		float max_energy = 100;

		// aim circle
		glm::vec3 aimOrigin;

		// control the tornado
		bool viewFlag;
		bool tornado = false;
		float last_speed;
		float last_acc;

		Common* player;

		GLuint UIshader;
		GLuint UIText[4];
		GLuint UIvbo;
		GLuint UIebo;

		GLuint ScreenText[3];
		GLuint Screenshader;
		GLuint Screenvbo;
		GLuint Screenebo;


		int num_Drone = 40;
		int min_Drone = 25;
		int num_Chicken = 25;
		int min_Chicken = 15;


		// tornado suck
		float suckTime = 0;

	}; // class Game

} // namespace game

#endif // GAME_H_
