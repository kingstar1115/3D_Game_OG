#include <iostream>
#include <time.h>
#include <sstream>

#include "game.h"
#include "bin/path_config.h"

namespace game {

	// Some configuration constants
	// They are written here as global variables, but ideally they should be loaded from a configuration file

	// Main window settings
	const std::string window_title_g = "Matrix Hell";
	const unsigned int window_width_g = 800;
	const unsigned int window_height_g = 600;
	const bool window_full_screen_g = false;

	// Viewport and camera settings
	float camera_near_clip_distance_g = 0.01;
	float camera_far_clip_distance_g = 1000.0;
	float camera_fov_g = 20.0; // Field-of-view of camera
	const glm::vec3 viewport_background_color_g(0.0, 0.0, 0.0);
	glm::vec3 camera_position_g(0.0, 0.0, 2.0);
	glm::vec3 camera_look_at_g(0.0, 0.0, 0.0);
	glm::vec3 camera_up_g(0.0, 1.0, 0.0);

	// Materials 
	const std::string material_directory_g = MATERIAL_DIRECTORY;


	Game::Game(void) {

		// Don't do work in the constructor, leave it for the Init() function
	}


	void Game::Init(void) {

		// Run all initialization steps
		InitWindow();
		InitView();
		InitEventHandlers();

		// Set variables
		animating_ = true;
	}


	void Game::InitWindow(void) {

		// Initialize the window management library (GLFW)
		if (!glfwInit()) {
			throw(GameException(std::string("Could not initialize the GLFW library")));
		}

		// Create a window and its OpenGL context
		if (window_full_screen_g) {
			window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), glfwGetPrimaryMonitor(), NULL);
		}
		else {
			window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), NULL, NULL);
		}
		if (!window_) {
			glfwTerminate();
			throw(GameException(std::string("Could not create window")));
		}

		// Make the window's context the current one
		glfwMakeContextCurrent(window_);

		// Initialize the GLEW library to access OpenGL extensions
		// Need to do it after initializing an OpenGL context
		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if (err != GLEW_OK) {
			throw(GameException(std::string("Could not initialize the GLEW library: ") + std::string((const char *)glewGetErrorString(err))));
		}
	}


	void Game::InitView(void) {

		// Set up z-buffer
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);


	}


	void Game::InitEventHandlers(void) {

		// Set event callbacks
		glfwSetKeyCallback(window_, KeyCallback);
		glfwSetFramebufferSizeCallback(window_, ResizeCallback);

		// Set pointer to game object, so that callbacks can access it
		glfwSetWindowUserPointer(window_, (void *)this);
	}


	void Game::SetupResources(void) {


		// Create a torus
		resman_.CreateTorus("TorusMesh");

		resman_.Create2Dsquare("2DSquare");



		// Load material to be applied to torus
		std::string filename = std::string(MATERIAL_DIRECTORY) + std::string("/three-term_shiny_blue");
		resman_.LoadResource(Material, "ShinyBlueMaterial", filename.c_str());

		// Load texture to be applied to the cube
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/checker.png");
		resman_.LoadResource(Texture, "Checker", filename.c_str());

		// Load texture to be applied to the turret
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/camouflage.png");
		resman_.LoadResource(Texture, "Camouflage", filename.c_str());
		// Load texture to be applied to the turret
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/white.png");
		resman_.LoadResource(Texture, "White", filename.c_str());

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/white_House.png");
		resman_.LoadResource(Texture, "White_House", filename.c_str());

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Black.png");
		resman_.LoadResource(Texture, "Black", filename.c_str());
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Health_red.png");
		resman_.LoadResource(Texture, "Health_red", filename.c_str());
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Magic_blue.png");
		resman_.LoadResource(Texture, "Magic_blue", filename.c_str());

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Begin.png");
		resman_.LoadResource(Texture, "Begin", filename.c_str());
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/HappyEnd.png");
		resman_.LoadResource(Texture, "HappyEnd", filename.c_str());
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/SadEnd.png");
		resman_.LoadResource(Texture, "SadEnd", filename.c_str());

		// new add ***********************************************************************************
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Wings.png");
		resman_.LoadResource(Texture, "Wings", filename.c_str());

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Wings_tip.png");
		resman_.LoadResource(Texture, "Wings_tip", filename.c_str());
		
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Beak.png");
		resman_.LoadResource(Texture, "Beak", filename.c_str());

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Roof.png");
		resman_.LoadResource(Texture, "Roof", filename.c_str());

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/Metal.png");
		resman_.LoadResource(Texture, "Metal", filename.c_str());
		

		// Load material to be applied to the cube
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/textured_material");
		resman_.LoadResource(Material, "TexturedMaterial", filename.c_str());



		// Can also check reflections on a cube
		std::string cube_filename = std::string(MATERIAL_DIRECTORY) + std::string("/dense_cube.aobj");
		//resman_.LoadResource(Mesh, "TorusMesh", cube_filename.c_str());

		// Create a cube for the skybox
		resman_.CreateCube("CubeMesh");

		// Load material to be applied to asteroids
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/material");
		resman_.LoadResource(Material, "ObjectMaterial", filename.c_str());

		// Create mirror plane
		resman_.CreateMirror("MirrorMesh");


		// Load material to be applied to torus
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/envmap");
		resman_.LoadResource(Material, "EnvMapMaterial", filename.c_str());

		// Load cube map to be applied to skybox
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/skybox/siege.tga");
		resman_.LoadResource(CubeMap, "SkyboxCubeMap", filename.c_str());

		// Load material to be applied to skybox
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/skybox");
		resman_.LoadResource(Material, "SkyboxMaterial", filename.c_str());

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/ground.png");
		resman_.LoadResource(Texture, "Ground", filename.c_str());

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/common_texture_material");
		resman_.LoadResource(Material, "c_t_material", filename.c_str());

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/aim.png");
		resman_.LoadResource(Texture, "aim", filename.c_str());


		resman_.CreateCylinder ("Cylinder");

		// Load material for screen-space effect
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/screen_space");
		resman_.LoadResource(Material, "ScreenSpaceMaterial", filename.c_str());

		// Setup drawing to texture
		scene_.SetupDrawToTexture();

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/particle");
		resman_.LoadResource(Material, "ParticleMaterial", filename.c_str());

		// Create particles for explosion
		//Feather
		resman_.CreateSphereParticles("SphereParticles1", 1000);
		//Tornado
		resman_.CreateSphereParticles("SphereParticles2", 10000);

		// Chicken
		resman_.CreateSphere("CK_Body", 0.44, 0.33, 0.33);
		resman_.CreateSphere("CK_Head", 0.25, 0.19, 0.19);
		//							float thick, float bot, float top, float height, bool tip
		resman_.CreateTriangle("CK_Beak", 0.09, 0.09, 0.01, 0.2, true);
		resman_.CreateCylinder("CK_Legs", 0.036, 0.33);

		// Hen
		//							float thick, float bot, float top, float height
		resman_.CreateTriangle("Hen_wings", 0.06, 0.4, 0.099, 0.66, true); 

		// Aim
		resman_.Create2Dsquare("Aim");

		// Drone
		resman_.CreateCylinder("Drone_Body", 0.22, 0.3);
		resman_.CreateCylinder("Drone_Center", 0.046, 0.27);
		//							float thick, float bot, float top, float height
		resman_.CreateTrape("Drone_Prop", 0.04, 0.04, 0.04, 0.7);

		// House
		resman_.CreateCube_noRoof("HenHouse", 0.5);
		resman_.CreatePyramid("ROOF", 0.6, 0.1, 0.3);

		// Bird
		resman_.CreatePyramid("Bird_head", 0.44, 0.2, 0.40);
		resman_.CreateCylinder("Bird_body", 0.20, 0.87);
		//							float thick, float bot, float top, float height
		resman_.CreateTrape("Bird_wings", 0.076, 0.46, 0.3, 0.86);
		resman_.CreateTriangle("Bird_wings_tip", 0.07, 0.3, 0.1, 0.66, false);
		//							float thick, float bot, float top, float height, bool tip
		resman_.CreateTriangle("Bird_beak", 0.2, 0.2, 0.03, 0.39, true);
	}

	void Game::SetupScene(void) {

		// Set background color for the scene
		scene_.SetBackgroundColor(viewport_background_color_g);

		// Set up camera
		SetUpCamera();

		// Set background color for the scene
		scene_.SetBackgroundColor(viewport_background_color_g);

		// Create UI
		CreateUI();

		// Create 2D screen texture
		CreateScreen();
		
		// Create skybox
		skybox_ = CreateSkyBoxInstance("Skybox", "CubeMesh", "SkyboxMaterial", "SkyboxCubeMap");
		skybox_->Scale(glm::vec3(50));
		//skybox_->Translate(glm::vec3(0.0, 100.0, 0.0));
		scene_.AddNode(skybox_);
		
		
		// Create lakes
		for (int l = 0; l < 7; l++) {
			Common *lake = CreateCommonInstance("Lake1", "MirrorMesh", "EnvMapMaterial", "", "SkyboxCubeMap");
			// Scale the instance
			lake->Scale(glm::vec3(20));
			glm::vec3 position = getRandomPos();
			lake->Translate(glm::vec3(position.x, -24.8, position.z));
			lake->Rotate(glm::angleAxis(glm::pi<float>() / 2.0f, glm::vec3(1.0, 0.0, 0.0)));
			scene_.AddNode(lake);
		}

		
		
		// Create a ground
		Common * ground = CreateCommonInstance("Ground1", "2DSquare", "c_t_material","Ground");
		ground->Scale(glm::vec3(400));
		ground->Translate(glm::vec3(0.0, -25, 0.0));
		ground->Rotate(glm::angleAxis(glm::pi<float>() / 2.0f, glm::vec3(1.0, 0.0, 0.0)));
		scene_.AddNode(ground);
		

		// initial object
		// Create Housesesesssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
		CreatHouse(glm::vec3(12.0, -22.3, 90));
		CreatHouse(glm::vec3(-110.0, -22.3, 70));
		CreatHouse(glm::vec3(-10.0, -22.3, -46));
		CreatHouse(glm::vec3(66.0, -22.3, -100));

		// Create player bird
		CreateBird(glm::vec3(0.0, 0.0, 0.3));

		// Create initial drone
		for (int u = 0; u < 40; u++) {
			glm::vec3 position = getRandomPos();
			CreateDrone(position);
		}

		// Create initial Chicken
		for (int i = 0; i < 25; i++) {
			glm::vec3 position = getRandomPos();
			CreateChicken(glm::vec3(position.x, -24.3, position.y));
		}

		// Create initial Hen
		for (int j = 0; j < 3; j++) {
			glm::vec3 position = getRandomPos();
			CreateHen(glm::vec3(position.x, -23.7, position.y));
		}
		
	}

	void Game::spawnChicken() {
		if (num_Chicken < min_Chicken) {
			min_Chicken += 5;
			// determine which house to spwan
			int house = rand() % 4 + 1;
			glm::vec3 pos;
			if (house == 1) { pos = glm::vec3(12.0, -23.7, 90); }
			if (house == 2) { pos = glm::vec3(-110.0, -23.7, 70); }
			if (house == 3) { pos = glm::vec3(-10.0, -23.7, -46); }
			if (house == 4) { pos = glm::vec3(66.0, -23.7, -100); }

			int x = rand() % 5 + 5;
			for (int y = 1; y < x; y++) {
				CreateChicken(glm::vec3(pos.x, -24.3, pos.y));
				num_Chicken += 1;
			}
			CreateHen(glm::vec3(pos.x, -23.7, pos.y));
		}
	}

	void Game::spawnDrone() {
		if (num_Drone < min_Drone) {
			min_Drone += 5;

			int x = rand() % 5 + 5;
			for (int y = 1; y < x; y++) {
				glm::vec3 position = getRandomPos();
				CreateDrone(position);
				num_Drone += 1;
			}
		}
	}

	void Game::CreateChicken(glm::vec3 pos) {

		game::Common *CK_Body = CreateCommonInstance("CK_Body", "CK_Body", "TexturedMaterial", "Beak");
		CK_Body->Scale(glm::vec3(1.0, 1.0, 1.0));
		CK_Body->Translate(pos);
		scene_.AddNode(CK_Body);

		game::Common *CK_Head = CreateCommonInstance("CK_Head", "CK_Head", "TexturedMaterial", "Beak");
		CK_Head->Scale(glm::vec3(1.0, 1.0, 1.0));
		CK_Head->Translate(glm::vec3(-0.36, 0.36, 0));

		game::Common *CK_Beak = CreateCommonInstance("CK_Beak", "CK_Beak", "TexturedMaterial", "White");
		CK_Beak->Scale(glm::vec3(1.0, 1.0, 1.0));
		glm::quat rotation_CKBeak = glm::angleAxis(-90 * glm::pi<float>() / 180.0f, glm::vec3(0.0, 0.0, 1.0));
		CK_Beak->Rotate(rotation_CKBeak);
		rotation_CKBeak = glm::angleAxis(180 * glm::pi<float>() / 180.0f, glm::vec3(1.0, 0.0, 0.0));
		CK_Beak->Rotate(rotation_CKBeak);
		CK_Beak->Translate(glm::vec3(-0.7, -0.45, 0));

		game::Common *CK_Lleg = CreateCommonInstance("CK_Lleg", "CK_Legs", "TexturedMaterial", "White");
		CK_Lleg->Scale(glm::vec3(1.0, 1.0, 1.0));
		CK_Lleg->SetAngleSpeed(0.006);
		CK_Lleg->SetRotAxis(glm::vec3(0, 0, 1.0));
		CK_Lleg->SetRotRange(0.12);
		CK_Lleg->SetOrigin(glm::vec3(0, 0.5, 0));
		CK_Lleg->Translate(glm::vec3(0, 0.077, 0.15));

		game::Common *CK_Rleg = CreateCommonInstance("CK_Rleg", "CK_Legs", "TexturedMaterial", "White");
		CK_Rleg->Scale(glm::vec3(1.0, 1.0, 1.0));
		CK_Rleg->SetAngleSpeed(-0.006);
		CK_Rleg->SetRotAxis(glm::vec3(0, 0, 1.0));
		CK_Rleg->SetRotRange(0.12);
		CK_Rleg->SetOrigin(glm::vec3(0, 0.5, 0));
		CK_Rleg->Translate(glm::vec3(0, 0.077, -0.15));

		CK_Head->SetParent(CK_Body);
		CK_Beak->SetParent(CK_Head);
		CK_Lleg->SetParent(CK_Body);
		CK_Rleg->SetParent(CK_Body);

		CK_Body->setTarget(CK_Body->GetPosition());
		CK_Body->SetForward(glm::vec3(-1, 0, 0));
		CK_Body->SetSpeed(0.024);
	}


	void Game::CreateHen(glm::vec3 pos) {

		game::Common *Hen_Body = CreateCommonInstance("Hen_Body", "CK_Body", "TexturedMaterial", "Beak");
		Hen_Body->Scale(glm::vec3(2.5, 2.2, 2.0));
		Hen_Body->Translate(pos);
		scene_.AddNode(Hen_Body);

		game::Common *Hen_Head = CreateCommonInstance("Hen_Head", "CK_Head", "TexturedMaterial", "Beak");
		Hen_Head->Scale(glm::vec3(1.0, 1.0, 1.0));
		Hen_Head->Translate(glm::vec3(-0.36, 0.36, 0));
		scene_.AddNode(Hen_Head);

		game::Common *Hen_Beak = CreateCommonInstance("Hen_Beak", "CK_Beak", "TexturedMaterial", "White");
		Hen_Beak->Scale(glm::vec3(1.0, 1.0, 1.0));
		glm::quat rotation_Hen_Beak = glm::angleAxis(-90 * glm::pi<float>() / 180.0f, glm::vec3(0.0, 0.0, 1.0));
		Hen_Beak->Rotate(rotation_Hen_Beak);
		rotation_Hen_Beak = glm::angleAxis(180 * glm::pi<float>() / 180.0f, glm::vec3(1.0, 0.0, 0.0));
		Hen_Beak->Rotate(rotation_Hen_Beak);
		Hen_Beak->Translate(glm::vec3(-0.7, -0.45, 0));
		scene_.AddNode(Hen_Beak);

		game::Common *Hen_Lleg = CreateCommonInstance("Hen_Lleg", "CK_Legs", "TexturedMaterial", "White");
		Hen_Lleg->Scale(glm::vec3(1.0, 1.0, 1.0));
		Hen_Lleg->SetAngleSpeed(0.006);
		Hen_Lleg->SetRotAxis(glm::vec3(0, 0, 1.0));
		Hen_Lleg->SetRotRange(0.12);
		Hen_Lleg->SetOrigin(glm::vec3(0, 0.5, 0));
		Hen_Lleg->Translate(glm::vec3(0, 0.077, 0.15));
		scene_.AddNode(Hen_Lleg);

		game::Common *Hen_Rleg = CreateCommonInstance("Hen_Rleg", "CK_Legs", "TexturedMaterial", "White");
		Hen_Rleg->Scale(glm::vec3(1.0, 1.0, 1.0));
		Hen_Rleg->SetAngleSpeed(-0.006);
		Hen_Rleg->SetRotAxis(glm::vec3(0, 0, 1.0));
		Hen_Rleg->SetRotRange(0.12);
		Hen_Rleg->SetOrigin(glm::vec3(0, 0.5, 0));
		Hen_Rleg->Translate(glm::vec3(0, 0.077, -0.15));
		scene_.AddNode(Hen_Rleg);

		game::Common *Hen_LWing = CreateCommonInstance("Hen_LWing", "Hen_wings", "TexturedMaterial", "Beak");
		Hen_LWing->Scale(glm::vec3(1.0, 1.0, 1.0));
		glm::quat rotation_Hen_Wing = glm::angleAxis(90 * glm::pi<float>() / 180.0f, glm::vec3(1.0, 0.0, 0.0));
		Hen_LWing->Rotate(rotation_Hen_Wing);
		Hen_LWing->SetAngleSpeed(0.012);
		Hen_LWing->SetRotAxis(glm::vec3(1.0, 0, 0));
		Hen_LWing->SetRotRange(0.24);
		Hen_LWing->SetOrigin(glm::vec3(0, -0.5, 0));
		Hen_LWing->Translate(glm::vec3(0.3, 0.0, 0.3));
		scene_.AddNode(Hen_LWing);

		game::Common *Hen_RWing = CreateCommonInstance("Hen_RWing", "Hen_wings", "TexturedMaterial", "Beak");
		Hen_RWing->Scale(glm::vec3(1.0, 1.0, 1.0));
		rotation_Hen_Wing = glm::angleAxis(-90 * glm::pi<float>() / 180.0f, glm::vec3(1.0, 0.0, 0.0));
		Hen_RWing->Rotate(rotation_Hen_Wing);
		Hen_RWing->SetAngleSpeed(-0.012);
		Hen_RWing->SetRotAxis(glm::vec3(1.0, 0, 0));
		Hen_RWing->SetRotRange(0.24);
		Hen_RWing->SetOrigin(glm::vec3(0, -0.5, 0));
		Hen_RWing->Translate(glm::vec3(0.3, 0, -0.3));
		scene_.AddNode(Hen_RWing);

		Hen_Body->setTarget(Hen_Body->GetPosition());
		Hen_Body->SetSpeed(0.04);
		Hen_Body->SetForward(glm::vec3(-1, 0, 0));
		Hen_Head->SetParent(Hen_Body);
		Hen_Beak->SetParent(Hen_Head);
		Hen_Lleg->SetParent(Hen_Body);
		Hen_Rleg->SetParent(Hen_Body);
		Hen_LWing->SetParent(Hen_Body);
		Hen_RWing->SetParent(Hen_Body);
	}

	// not in scenegraph nodelist, special for tornado effect
	void Game::CreateAim(glm::vec3 pos) {

		game::SceneNode *Aim = CreateCommonInstance("Aim", "Aim", "c_t_material", "aim");
		Aim->Scale(glm::vec3(5));
		glm::quat rotation = glm::angleAxis(90 * glm::pi<float>() / 180.0f, glm::vec3(1.0, 0.0, 0.0));
		Aim->Rotate(rotation);
		scene_.AddNode(Aim);
	}

	void Game::CreateDrone(glm::vec3 pos) {

		game::Common *Drone_Body = CreateCommonInstance("Drone_Body", "Drone_Body", "TexturedMaterial", "Metal");
		Drone_Body->Scale(glm::vec3(2.0, 2.0, 2.0));
		Drone_Body->Translate(pos);
		scene_.AddNode(Drone_Body);

		game::Common *Drone_Center = CreateCommonInstance("Drone_Center", "Drone_Center", "TexturedMaterial", "White");
		Drone_Center->Scale(glm::vec3(1.0, 1.0, 1.0));
		Drone_Center->Translate(glm::vec3(0, 0.27, 0));
		Drone_Center->SetAngleSpeed(0.05);
		Drone_Center->SetRotAxis(glm::vec3(0, 1, 0));

		game::Common *Drone_Prop1 = CreateCommonInstance("Drone_Prop", "Drone_Prop", "TexturedMaterial", "Metal");
		Drone_Prop1->Scale(glm::vec3(1.0, 1.0, 1.0));
		glm::quat rotation_Prop1 = glm::angleAxis(90 * glm::pi<float>() / 180.0f, glm::vec3(0.0, 0.0, 1.0));
		Drone_Prop1->Rotate(rotation_Prop1);
		Drone_Prop1->Translate(glm::vec3(-0.16, 0.6, 0));

		game::Common *Drone_Prop2 = CreateCommonInstance("Drone_Prop", "Drone_Prop", "TexturedMaterial", "Metal");
		Drone_Prop2->Scale(glm::vec3(1.0, 1.0, 1.0));
		glm::quat rotation_Prop2 = glm::angleAxis(90 * glm::pi<float>() / 180.0f, glm::vec3(0.0, 0.0, 1.0));
		Drone_Prop2->Rotate(rotation_Prop2);
		rotation_Prop2 = glm::angleAxis(90 * glm::pi<float>() / 180.0f, glm::vec3(1.0, 0.0, 0.0));
		Drone_Prop2->Rotate(rotation_Prop2);
		Drone_Prop2->Translate(glm::vec3(-0.0, 0.6, 0.16));
		//***************************************************************************************
		// initial drone's target
		Drone_Body->setTarget(Drone_Body->GetPosition());
		Drone_Body->SetSpeed(0.06);
		//***************************************************************************************
		Drone_Center->SetParent(Drone_Body);
		Drone_Prop1->SetParent(Drone_Center);
		Drone_Prop2->SetParent(Drone_Center);
	}


	void Game::CreatHouse(glm::vec3 pos) {

		Common *House = CreateCommonInstance("House", "HenHouse", "TexturedMaterial", "White_House");
		House->Scale(glm::vec3(10.0, 10.0, 10.0));;
		House->Translate(pos);
		scene_.AddNode(House);

		Common *Roof = CreateCommonInstance("Roof", "ROOF", "TexturedMaterial", "Roof");
		Roof->Scale(glm::vec3(1.0, 1.0, 1.0));;
		Roof->Translate(glm::vec3(0.2, 0.75, 0));
		scene_.AddNode(Roof);

		Roof->SetParent(House);
	}

	void Game::CreateBird(glm::vec3 pos) {
		
		resman_.CreateTail("Bird_tail", 0.3, 0.2);

		Common *Body = CreateCommonInstance("Body", "Bird_body", "TexturedMaterial", "Wings");
		Body->Scale(glm::vec3(1.5));
		Body->Translate(pos);
		Body->Rotate(glm::angleAxis(glm::pi<float>() / 2.0f, glm::vec3(1, 0, 0)));
		Body->Rotate(glm::angleAxis(glm::pi<float>(), glm::vec3(0, 0, 1)));

		Common *Head = CreateCommonInstance("Head", "Bird_head", "TexturedMaterial", "White");
		Head->Scale(glm::vec3(1.0, 1.0, 1.0));
		Head->Translate(glm::vec3(0.285, 0.9, 0.0));

		Common *LWing = CreateCommonInstance("LWing", "Bird_wings", "TexturedMaterial", "Wings");
		LWing->Scale(glm::vec3(1.0, 1.0, 1.0));
		glm::quat rotation_LWing = glm::angleAxis(-90 * glm::pi<float>() / 180.0f, glm::vec3(0.0, 0.0, 1.0));
		LWing->Rotate(rotation_LWing);
		glm::quat rotation_LWing2 = glm::angleAxis(180 * glm::pi<float>() / 180.0f, glm::vec3(0.0, 1.0, 0.0));
		LWing->Rotate(rotation_LWing2);
		LWing->Translate(glm::vec3(0.1, 0.3, 0.0));
		LWing->SetAngleSpeed(0.01);
		LWing->SetRotAxis(glm::vec3(1.0, 0, 0));
		LWing->SetRotRange(0.26);
		LWing->SetOrigin(glm::vec3(0, -0.5, 0));

		Common *LWing_tip = CreateCommonInstance("LWing_tip", "Bird_wings_tip", "TexturedMaterial", "Wings_tip");
		LWing_tip->Scale(glm::vec3(1.0, 1.0, 1.0));
		LWing_tip->Translate(glm::vec3(0.16, 0.3, 0.0));
		LWing_tip->SetAngleSpeed(0.01);
		LWing_tip->SetRotAxis(glm::vec3(1.0, 0, 0));
		LWing_tip->SetRotRange(0.26);
		LWing_tip->SetOrigin(glm::vec3(0, -0.5, 0));

		Common *RWing = CreateCommonInstance("RWing", "Bird_wings", "TexturedMaterial", "Wings");
		RWing->Scale(glm::vec3(1.0, 1.0, 1.0));
		glm::quat rotation_RWing = glm::angleAxis(-90 * -glm::pi<float>() / 180.0f, glm::vec3(0.0, 0.0, 1.0));
		RWing->Rotate(rotation_RWing);
		RWing->Translate(glm::vec3(-0.1, 0.3, 0.0));
		RWing->SetAngleSpeed(-0.01);
		RWing->SetRotAxis(glm::vec3(1.0, 0, 0));
		RWing->SetRotRange(0.26);
		RWing->SetOrigin(glm::vec3(0, -0.5, 0));

		Common *RWing_tip = CreateCommonInstance("RWing_tip", "Bird_wings_tip", "TexturedMaterial", "Wings_tip");
		RWing_tip->Scale(glm::vec3(1.0, 1.0, 1.0));
		glm::quat rotation_RWing_Tip = glm::angleAxis(-glm::pi<float>() / 180.0f, glm::vec3(1.0, 0.0, 0.0));
		//RWing_tip->Rotate(rotation_RWing_Tip);
		RWing_tip->Translate(glm::vec3(0.16, 0.3, 0.0));
		RWing_tip->SetAngleSpeed(-0.01);
		RWing_tip->SetRotAxis(glm::vec3(1.0, 0, 0));
		RWing_tip->SetRotRange(0.26);
		RWing_tip->SetOrigin(glm::vec3(0, -0.5, 0));

		Common *Beak = CreateCommonInstance("Beak", "Bird_beak", "TexturedMaterial", "Beak");
		Beak->Scale(glm::vec3(1.0, 1.0, 1.0));
		glm::quat rotation_Beak = glm::angleAxis(-90 * glm::pi<float>() / 180.0f, glm::vec3(0.0, 1.0, 0.0));
		Beak->Rotate(rotation_Beak);
		Beak->Translate(glm::vec3(0.0, 1.16, 0.37));

		Common *Tail = CreateCommonInstance("Tail", "Bird_tail", "TexturedMaterial", "Wings");
		Tail->Scale(glm::vec3(1.0, 1.0, 1.0));
		glm::quat rotation_Tail = glm::angleAxis(12 * glm::pi<float>() / 180.0f, glm::vec3(1.0, 0.0, 0.0));
		Tail->Rotate(rotation_Tail);
		Tail->Translate(glm::vec3(0.0, -0.7, -0.1));

		Body->SetParent(current_camera);
		Head->SetParent(Body);
		LWing->SetParent(Body);
		RWing->SetParent(Body);
		Beak->SetParent(Body);
		LWing_tip->SetParent(LWing);
		RWing_tip->SetParent(RWing);
		Tail->SetParent(Body);

		// set 

		player = Body;

	}

	void Game::resetGame()
	{
		std::vector<SceneNode*> list;
		scene_.SetNodeList(list);
		SetupScene();
		health = 50;
		energy = 50;
		num_Drone = 40;
		min_Drone = 40;
		num_Chicken = 25;
		min_Chicken = 25;
	}

	void Game::SetUpCamera()
	{
		// Set viewport
		int width, height;
		glfwGetFramebufferSize(window_, &width, &height);
		glViewport(0, 0, width, height);

		Camera* camera = new Camera();
		// Set current view
		camera->SetView(camera_position_g, camera_look_at_g, camera_up_g);
		// Set projection
		camera->SetProjection(camera_fov_g, camera_near_clip_distance_g, camera_far_clip_distance_g, width, height);

		CameraNode *CNode = new CameraNode(camera, "Camera");
		CNode->SetFictionFactor(0.005);
		CNode->SetMaxSpeed(1.0);
		scene_.AddNode(CNode);
		current_camera = CNode;
		first_view_camera = CNode;

		// Create third Camera
		camera = new Camera();
		// Set current view
		camera->SetView(camera_position_g + glm::vec3(-2, 6, 10), current_camera->GetPosition(), glm::normalize(glm::vec3(0, 1, 0)));
		// Set projection
		camera->SetProjection(camera_fov_g, camera_near_clip_distance_g, camera_far_clip_distance_g, width, height);

		CameraNode* thirdCamera = new CameraNode(camera, "ThirdCamera");
		thirdCamera->SetParent(CNode);
		third_view_camera = thirdCamera;


		// Create overlook  camera
		camera = new Camera();
		CameraNode* overlookCamera = new CameraNode(camera, "OverlookCamera");
		camera->SetProjection(camera_fov_g, camera_near_clip_distance_g, camera_far_clip_distance_g, width, height);
		overlook_camera = overlookCamera;
		scene_.AddNode(overlookCamera);

	}



	void Game::CreateUI()
	{
		UIshader = resman_.GetResource("c_t_material")->GetResource();
		UIText[0] = resman_.GetResource("Health_red")->GetResource();
		UIText[1] = resman_.GetResource("Black")->GetResource();
		UIText[2] = resman_.GetResource("Magic_blue")->GetResource();
		UIText[3] = resman_.GetResource("Black")->GetResource();

		UIvbo = resman_.GetResource("2DSquare")->GetArrayBuffer();
		UIebo = resman_.GetResource("2DSquare")->GetElementArrayBuffer();

	}

	void Game::CreateScreen()
	{
		Screenshader = resman_.GetResource("c_t_material")->GetResource();

		ScreenText[0] = resman_.GetResource("Begin")->GetResource();
		ScreenText[1] = resman_.GetResource("HappyEnd")->GetResource();
		ScreenText[2] = resman_.GetResource("SadEnd")->GetResource();

		Screenvbo = resman_.GetResource("2DSquare")->GetArrayBuffer();
		Screenebo = resman_.GetResource("2DSquare")->GetElementArrayBuffer();
	}

	void Game::DrawUI() {
		// Enable z-buffer
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		// Select proper material (shader program)
		glUseProgram(UIshader);

		// Set geometry to draw
		glBindBuffer(GL_ARRAY_BUFFER, UIvbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, UIebo);

		// Set attributes for 2d camera
		// Set view matrix in shader
		GLint view_mat = glGetUniformLocation(UIshader, "view_mat");
		glm::mat4 matrix = glm::mat4(1.0f);
		glUniformMatrix4fv(view_mat, 1, GL_FALSE, glm::value_ptr(matrix));

		// Set projection matrix in shader
		GLint projection_mat = glGetUniformLocation(UIshader, "projection_mat");
		glUniformMatrix4fv(projection_mat, 1, GL_FALSE, glm::value_ptr(matrix));

		// Set attributes for shaders
		GLint vertex_att = glGetAttribLocation(UIshader, "vertex");
		glVertexAttribPointer(vertex_att, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(vertex_att);

		GLint normal_att = glGetAttribLocation(UIshader, "normal");
		glVertexAttribPointer(normal_att, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(normal_att);

		GLint color_att = glGetAttribLocation(UIshader, "color");
		glVertexAttribPointer(color_att, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void *)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(color_att);

		GLint tex_att = glGetAttribLocation(UIshader, "uv");
		glVertexAttribPointer(tex_att, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void *)(9 * sizeof(GLfloat)));
		glEnableVertexAttribArray(tex_att);

		glm::mat4 transfMatrix, scalingMatrix, transf;
		GLint world_mat = glGetUniformLocation(UIshader, "world_mat");
		GLint tex = glGetUniformLocation(UIshader, "texture_map");

		
		for (int i = 0; i < 2; i++) {

			transfMatrix = glm::translate(glm::mat4(1.0), glm::vec3(-0.8 + i * 0.2, 0.5, 0));
			scalingMatrix = glm::scale(glm::mat4(1.0), glm::vec3(0.15, 0.5, 1));
			transf = transfMatrix * scalingMatrix;

			glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(transf));

			glUniform1i(tex, 0); // Assign the first texture to the map
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, UIText[i * 2 + 1]);
			// Define texture interpolation
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glDrawElements(GL_TRIANGLES, 16, GL_UNSIGNED_INT, 0);
		}
		

		transfMatrix = glm::translate(glm::mat4(1.0), glm::vec3(-0.8 , 0.5, -0.01));
		float precent = health / max_health;
		scalingMatrix = glm::scale(glm::mat4(1.0), glm::vec3(0.15, 0.5*precent, 1));
		transfMatrix = glm::translate(transfMatrix, glm::vec3(0, -0.25+0.5*precent/2, 0));


		transf = transfMatrix * scalingMatrix;
		glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(transf));

		glUniform1i(tex, 0); // Assign the first texture to the map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, UIText[0]);
		// Define texture interpolation
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glDrawElements(GL_TRIANGLES, 16, GL_UNSIGNED_INT, 0);


		transfMatrix = glm::translate(glm::mat4(1.0), glm::vec3(-0.6, 0.5, -0.01));
		precent = energy / max_energy;
		scalingMatrix = glm::scale(glm::mat4(1.0), glm::vec3(0.15, 0.5*precent, 1));
		transfMatrix = glm::translate(transfMatrix, glm::vec3(0, -0.25+0.5*precent / 2, 0));
		transf = transfMatrix * scalingMatrix;
		glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(transf));

		glUniform1i(tex, 0); // Assign the first texture to the map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, UIText[2]);
		// Define texture interpolation
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glDrawElements(GL_TRIANGLES, 16, GL_UNSIGNED_INT, 0);
		
	}

	void Game::RenderScreen(GameState gs)
	{

		// Clear background
		glClearColor(0,0,0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		int i;
		switch (gs)
		{
		case game::Begining:
			i = 0;
			break;
		case game::Playing:
			break;
		case game::HappyEnd:
			i = 1;
			break;
		case game::SadEnd:
			i = 2;
			break;
		default:
			break;
		}

		// Enable z-buffer
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		// Select proper material (shader program)
		glUseProgram(Screenshader);

		// Set geometry to draw
		glBindBuffer(GL_ARRAY_BUFFER, Screenvbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Screenebo);

		// Set attributes for 2d camera
		// Set view matrix in shader
		GLint view_mat = glGetUniformLocation(Screenshader, "view_mat");
		glm::mat4 matrix = glm::mat4(1.0f);
		glUniformMatrix4fv(view_mat, 1, GL_FALSE, glm::value_ptr(matrix));

		// Set projection matrix in shader
		GLint projection_mat = glGetUniformLocation(Screenshader, "projection_mat");
		glUniformMatrix4fv(projection_mat, 1, GL_FALSE, glm::value_ptr(matrix));

		// Set attributes for shaders
		GLint vertex_att = glGetAttribLocation(Screenshader, "vertex");
		glVertexAttribPointer(vertex_att, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(vertex_att);

		GLint normal_att = glGetAttribLocation(Screenshader, "normal");
		glVertexAttribPointer(normal_att, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(normal_att);

		GLint color_att = glGetAttribLocation(Screenshader, "color");
		glVertexAttribPointer(color_att, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void *)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(color_att);

		GLint tex_att = glGetAttribLocation(Screenshader, "uv");
		glVertexAttribPointer(tex_att, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void *)(9 * sizeof(GLfloat)));
		glEnableVertexAttribArray(tex_att);

		glm::mat4 transfMatrix, scalingMatrix, transf;
		GLint world_mat = glGetUniformLocation(Screenshader, "world_mat");
		GLint tex = glGetUniformLocation(Screenshader, "texture_map");

		transfMatrix = glm::mat4(1.0);
		scalingMatrix = glm::scale(glm::mat4(1.0), glm::vec3(2));
		transf = transfMatrix * scalingMatrix;
		glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(transf));

		glUniform1i(tex, 0); // Assign the first texture to the map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ScreenText[i]);
		// Define texture interpolation
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glDrawElements(GL_TRIANGLES, 16, GL_UNSIGNED_INT, 0);
	}



	void Game::MainLoop(void) {

		// Loop while the user did not close the window
		while (!glfwWindowShouldClose(window_)) {

			// remove distoried object
			std::vector<SceneNode*>::iterator it;
			std::vector<SceneNode*> list = scene_.GetNodeList();
			std::vector<Particle*> pList;
			for (it = list.begin(); it != list.end();) {
				if ((*it)->GetShouldBeDestoried()) { 
					if ((*it)->GetName() == "CK_Body") { num_Chicken -= 1; }
					if ((*it)->GetName() == "Drone_Body") { 
						num_Drone -= 1; 
						pList.push_back(CreateExplosion((*it)->GetPosition()));
					}
					it = list.erase(it); 
				}
				else ++it;
			}
			list.insert(list.end(),pList.begin(),pList.end());
			scene_.SetNodeList(list);

			if (gameStep == Begining || gameStep == HappyEnd || gameStep == SadEnd) {
				RenderScreen(gameStep);
			}
			else {
				// Animate the scene
				if (animating_) {
					static double last_time = 0;
					double current_time = glfwGetTime();
					if ((current_time - last_time) > 0.01) {
						scene_.Update();
						last_time = current_time;
						firecooldown = firecooldown - 0.01 <= 0 ? 0 : firecooldown - 0.01;

						health = health - 0.006 <= 0 ? 0 : health - 0.006;
						if (health <= 0) {
							gameStep = SadEnd;
						}
						else if (health >= 100) {
							gameStep = HappyEnd;
						}
						energy = energy + 0.007 >= 100 ? 100 : energy + 0.007;

						
						if (first_view_camera->getStun() >= 0.0f) { playerstate = Stun; }
						else { playerstate = Normal; }


						if (playerstate == Normal) {
							// Draw the scene
							scene_.Draw(current_camera->GetCamera());
							DrawUI();
						}
						else if (playerstate == Stun) {

							// Draw the scene to a texture
							scene_.DrawToTexture(current_camera->GetCamera());

							// Process the texture with a screen-space effect and display the texture
							scene_.DisplayTexture(resman_.GetResource("ScreenSpaceMaterial")->GetResource(), 4);

							DrawUI();
						}


						// handle suck
						if (suckTime <= 0) { scene_.setSuck(glm::vec3(999, 999, 999)); }
						suckTime -= 1;

						//respawn
						spawnChicken();
						spawnDrone();


					}
				}
			}
			
			// Push buffer drawn in the background onto the display
			glfwSwapBuffers(window_);

			// Update other events like input handling
			glfwPollEvents();
		}
	}


	void Game::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

		// Get user data with a pointer to the game class
		void* ptr = glfwGetWindowUserPointer(window);
		Game *game = (Game *)ptr;

		// Quit game if 'q' is pressed
		if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}

		if (game->gameStep == Begining) {
			if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
				game->gameStep = Playing;
			}
		}
		else if (game->gameStep == HappyEnd) {
			if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
				game->resetGame();
				game->gameStep = Playing;
			}
		}
		else if (game->gameStep == SadEnd) {
		if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
				game->resetGame();
				game->gameStep = Playing;
			}
		}
		else {
			// Stop animation if space bar is pressed
			if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
				game->animating_ = (game->animating_ == true) ? false : true;
			}

			// View control
			float rot_factor(glm::pi<float>() / 80);
			float trans_factor = 1;

			if (game->playerstate != Stun) {
				if (!game->getTornado()) {
					if (key == GLFW_KEY_UP) {
						game->first_view_camera->Pitch(rot_factor);
					}
					if (key == GLFW_KEY_DOWN) {
						game->first_view_camera->Pitch(-rot_factor);
					}
					if (key == GLFW_KEY_LEFT) {
						game->first_view_camera->Yaw(rot_factor);
					}
					if (key == GLFW_KEY_RIGHT) {
						game->first_view_camera->Yaw(-rot_factor);
					}
					if (key == GLFW_KEY_S) {
						game->first_view_camera->Roll(-rot_factor);
					}
					if (key == GLFW_KEY_X) {
						game->first_view_camera->Roll(rot_factor);
					}
					if (key == GLFW_KEY_A && action == GLFW_PRESS) {
						game->first_view_camera->SetAcceleration(0.1);
					}
					if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
						game->first_view_camera->SetAcceleration(0);
					}
					if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
						game->first_view_camera->SetAcceleration(-0.025);
					}
					if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
						game->first_view_camera->SetAcceleration(0);
					}

					if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
						game->thirdview = game->thirdview == true ? false : true;

						CameraNode * fcNode = (CameraNode *)game->scene_.GetNode("Camera");
						CameraNode * tcNode = (CameraNode *)game->scene_.GetNode("ThirdCamera");

						game->current_camera = game->thirdview ? tcNode : fcNode;
					}

					// Fire control
					if (key == GLFW_KEY_F && game->firecooldown == 0) {
						if (game->energy >= 3) {
							game->firecooldown = game->firegap;
							game->fire();
							Common* Lwing = (Common*)game->scene_.GetNode("LWing");

							Lwing->SetAngleSpeed(0.02);

							Lwing->setFAnimation(30);

							Common* Rwing = (Common*)game->scene_.GetNode("RWing");

							Rwing->SetAngleSpeed(-0.02);

							Rwing->setFAnimation(30);

							Common* Lwing_tip = (Common*)game->scene_.GetNode("LWing_tip");

							Lwing_tip->SetAngleSpeed(0.02);

							Lwing_tip->setFAnimation(30);

							Common* Rwing_tip = (Common*)game->scene_.GetNode("RWing_tip");

							Rwing_tip->SetAngleSpeed(-0.02);

							Rwing_tip->setFAnimation(30);
							game->energy -= 3;
						}

					}

					// key to melee attack
					if (key == GLFW_KEY_V && action == GLFW_PRESS) {
						Common* bird = (Common*)game->scene_.GetNode("Body");
						CameraNode * fcNode = (CameraNode *)game->scene_.GetNode("Camera");
						if (bird->getMAnimation() <= 0) {
							bird->SetAngleSpeed(-0.01);
							bird->SetRotAxis(glm::vec3(1.0, 0, 0));
							bird->SetRotRange(0.25);
							bird->SetOrigin(glm::vec3(0, 0, 0));
							bird->setMAnimation(96);

							glm::vec3 pos = fcNode->GetPosition() + fcNode->GetForward()*1.0f;
							int result = game->scene_.delicious(pos);
							std::cout << result << std::endl;
							if (result < 0) { game->first_view_camera->setStun(90); }
							else { game->health += result * 3; }
							game->energy -= 1;
						}
					}

					// key T control tornado
					if (key == GLFW_KEY_T && action == GLFW_PRESS) {
						if (game->energy >= 30) {
							game->setTornado(true);

							game->last_acc = game->first_view_camera->GetAcceleration();
							game->last_speed = game->first_view_camera->GetSpeed();
							game->first_view_camera->SetAcceleration(0);
							game->first_view_camera->SetSpeed(0);
							game->current_camera = game->overlook_camera;
							glm::vec3 pos = game->first_view_camera->GetCamera()->GetPosition();
							glm::vec3 newPos = pos + glm::vec3(0, 50, 0);
							glm::vec3 lookat = glm::vec3(newPos.x, -25, newPos.z);
							glm::vec3 up = glm::vec3(0, 0, -1);

							game->overlook_camera->GetCamera()->SetView(newPos, lookat, up);
							game->overlook_camera->GetCamera()->SetLookAt(lookat);
							game->overlook_camera->SetPosition(newPos);

							// Create aim object
							game->CreateAim(glm::vec3(0.0, 0.0, 0.0));
							SceneNode* Aim = game->scene_.GetNode("Aim");
							glm::vec3 aim_pos = glm::vec3(newPos.x, -24.5, newPos.z);
							Aim->Translate(aim_pos);
							game->aimOrigin = aim_pos;
							game->energy -= 30;
						}
					}
				}
				// control tornado
				else {
					SceneNode* aim = game->scene_.GetNode("Aim");
					glm::vec3 currentPos = aim->GetPosition();
					glm::vec3 nextPosition = currentPos;
					if (key == GLFW_KEY_T && action == GLFW_PRESS) {

						CameraNode * fcNode = (CameraNode *)game->scene_.GetNode("Camera");
						CameraNode * tcNode = (CameraNode *)game->scene_.GetNode("ThirdCamera");
						game->current_camera = game->thirdview ? tcNode : fcNode;

						game->setTornado(false);
						aim->SetShouldBeDestoried(true);
						game->CreateParticleTornado(aim->GetPosition());

						game->scene_.setSuck(aim->GetPosition());
						game->suckTime = 400.0;
						game->first_view_camera->SetAcceleration(game->last_acc);
						game->first_view_camera->SetSpeed(game->last_speed);

					}
					if (key == GLFW_KEY_J) {
						nextPosition = currentPos + glm::vec3(-1, 0, 0)*trans_factor;
					}
					if (key == GLFW_KEY_L) {
						nextPosition = currentPos + glm::vec3(1, 0, 0)*trans_factor;
					}
					if (key == GLFW_KEY_K) {
						nextPosition = currentPos + glm::vec3(0, 0, 1)*trans_factor;
					}
					if (key == GLFW_KEY_I) {
						nextPosition = currentPos + glm::vec3(0, 0, -1)*trans_factor;
					}
					if (glm::length(nextPosition - game->aimOrigin) >= 20) {
						nextPosition = currentPos;
					}

					game->overlook_camera->SetPosition(glm::vec3(nextPosition.x, game->overlook_camera->GetPosition().y, nextPosition.z));
					game->overlook_camera->GetCamera()->SetLookAt(glm::vec3(game->overlook_camera->GetPosition().x, -25, game->overlook_camera->GetPosition().z));
					aim->SetPosition(nextPosition);

				}
			}
		}


		
	}


	void Game::ResizeCallback(GLFWwindow* window, int width, int height) {

		// Set up viewport and camera projection based on new window size
		glViewport(0, 0, width, height);
		void* ptr = glfwGetWindowUserPointer(window);
		Game *game = (Game *)ptr;

		CameraNode* cNode = (CameraNode *)game->scene_.GetNode("Camera");
		cNode->GetCamera()->SetProjection(camera_fov_g, camera_near_clip_distance_g, camera_far_clip_distance_g, width, height);

	}


	Game::~Game() {

		glfwTerminate();
	}
	   
	void Game::fire() {
		//left and right
		CreateMissile(0);
		CreateMissile(1);
	}
	void Game::CreateParticleTornado(glm::vec3 pos) {
		Particle* tornado = CreateParticleInstance("Particle", "SphereParticles2", "ParticleMaterial");
		tornado->SetRenderState(false);
		tornado->SetLifeTime(4.0);
		tornado->SetScale(glm::vec3(0.01, 0.01, 0.01));
		tornado->SetPosition(glm::vec3(pos.x,pos.y-15,pos.z));
		tornado->Rotate(glm::normalize(glm::angleAxis(-(float)glm::pi<float>() / 2, glm::vec3(1, 0, 0))));
		tornado->SetForward(glm::vec3(0, 1, 0));
		tornado->SetFictionFactor(0);
		tornado->setType("Tornado");
		scene_.AddNode(tornado);
	}


	Particle * Game::CreateParticleFeather() {
		Particle* feather = CreateParticleInstance("Particle", "SphereParticles1", "ParticleMaterial");
		feather->SetRenderState(false);
		feather->SetLifeTime(1.0);
		feather->setType("Feather");
		return feather;

	}


	void Game::CreateMissile(int dir)
	{

		Common* missile = CreateCommonInstance("Missile", "Cylinder", "ObjectMaterial");
		missile->SetRenderState(false);
		missile->SetLifeTime(1.0);
		missile->SetScale(glm::vec3(0.05, 1, 0.05));
		missile->SetSpeed(first_view_camera->GetSpeed() + 0.5);
		CameraNode* cNode = (CameraNode*)scene_.GetNode("Camera");
		if (dir == 1) {
			missile->SetPosition(cNode->GetCamera()->GetPosition() + 0.2f*cNode->GetCamera()->GetForward() + 0.8f*cNode->GetCamera()->GetSide());
		}
		else {
			missile->SetPosition(cNode->GetCamera()->GetPosition() + 0.2f*cNode->GetCamera()->GetForward() + -0.8f*cNode->GetCamera()->GetSide());
		}

		missile->SetOrientation(cNode->GetCamera()->GetOrientation());
		missile->Rotate(glm::normalize(glm::angleAxis(-(float)glm::pi<float>() / 2, glm::vec3(1, 0, 0))));
		missile->SetForward(glm::vec3(0, 1, 0));
		missile->SetMaxSpeed(1.6);
		missile->SetSpeed(cNode->GetSpeed() + 0.7);
		missile->SetFictionFactor(0);

		Particle * p = CreateParticleFeather();
		p->SetParent(missile);
		scene_.AddNode(missile);

	}

	Particle* Game::CreateExplosion(glm::vec3 pos)
	{
		Particle* explosion = CreateParticleInstance("Particle", "SphereParticles1", "ParticleMaterial");
		explosion->SetRenderState(false);
		explosion->SetLifeTime(0.4);
		explosion->SetPosition(pos);
		explosion->setType("Explosion");
		return explosion;
	}

	ResourceSet Game::CollectSource(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name, std::string envmap_name) {
		ResourceSet theSet;
		Resource *geom = resman_.GetResource(object_name);
		if (!geom) {
			throw(GameException(std::string("Could not find resource \"") + object_name + std::string("\"")));
		}
		theSet.g = geom;

		Resource *mat = resman_.GetResource(material_name);
		if (!mat) {
			throw(GameException(std::string("Could not find resource \"") + material_name + std::string("\"")));
		}
		theSet.m = mat;
		
		Resource *tex = NULL;
		if (texture_name != "") {
			tex = resman_.GetResource(texture_name);
			if (!tex) {
				throw(GameException(std::string("Could not find resource \"") + material_name + std::string("\"")));
			}
		}
		theSet.t = tex;

		Resource *envmap = NULL;
		if (envmap_name != "") {
			envmap = resman_.GetResource(envmap_name);
			if (!envmap) {
				throw(GameException(std::string("Could not find resource \"") + envmap_name + std::string("\"")));
			}
		}
		theSet.e = envmap;
		return theSet;
	}

	SkyBox* Game::CreateSkyBoxInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name, std::string envmap_name) {
		ResourceSet theSet = CollectSource(entity_name, object_name, material_name, texture_name, envmap_name);

		SkyBox *scn = new SkyBox(entity_name, theSet.g, theSet.m, theSet.t, theSet.e);
		return scn;
	}

	Common* Game::CreateCommonInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name, std::string envmap_name) {
		ResourceSet theSet = CollectSource(entity_name, object_name, material_name, texture_name, envmap_name);
		Common *scn = new Common(entity_name, theSet.g, theSet.m, theSet.t, theSet.e);
		return scn;
	}

	Missile* Game::CreateMissileInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name, std::string envmap_name) {
		ResourceSet theSet = CollectSource(entity_name, object_name, material_name, texture_name, envmap_name);
		Missile *scn = new Missile(entity_name, theSet.g, theSet.m, theSet.t, theSet.e);
		return scn;
	}

	Particle* Game::CreateParticleInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name, std::string envmap_name) {
		ResourceSet theSet = CollectSource(entity_name, object_name, material_name, texture_name, envmap_name);
		Particle *scn = new Particle(entity_name, theSet.g, theSet.m, theSet.t, theSet.e);
		return scn;
	}

	glm::vec3 Game::getRandomPos() {
		float x = rand() % 90 + 1;
		float y = rand() % 90 + 1;
		float z = rand() % 90 + 1;
		float x_flag = rand() % 2 + 1;
		float z_flag = rand() % 2 + 1;
		if (x_flag == 1) { x *= -1; }
		if (z_flag == 1) { z *= -1; }
		glm::vec3 POS = glm::vec3(x, y, z);
		return POS;
	}

} // namespace game
