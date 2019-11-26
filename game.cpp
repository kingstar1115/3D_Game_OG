#include <iostream>
#include <time.h>
#include <sstream>

#include "game.h"
#include "bin/path_config.h"

namespace game {

// Some configuration constants
// They are written here as global variables, but ideally they should be loaded from a configuration file

// Main window settings
const std::string window_title_g = "Demo";
const unsigned int window_width_g = 800;
const unsigned int window_height_g = 600;
const bool window_full_screen_g = false;

// Viewport and camera settings
float camera_near_clip_distance_g = 0.01;
float camera_far_clip_distance_g = 1000.0;
float camera_fov_g = 25.0; // Field-of-view of camera
const glm::vec3 viewport_background_color_g(0.0, 0.0, 0.0);

const glm::vec3 viewport_background_color_white(1.0, 1.0, 1.0);
glm::vec3 camera_position_g(0.5, 0.5, 10.0);
glm::vec3 camera_look_at_g(0.0, 0.0, 0.0);
glm::vec3 camera_up_g(0.0, 1.0, 0.0);

float angle = 1.0472;
float increasement = 0.02;
float lowAn = 1.0472;
float highAn = 2.0944;

float length = 0.05;
float LNincrease = 0.001;
float lowLn = 0.11;
float highLn = 0.66;

// Materials 
const std::string material_directory_g = MATERIAL_DIRECTORY;


Game::Game(void){

    // Don't do work in the constructor, leave it for the Init() function
}


void Game::Init(void){

    // Run all initialization steps
    InitWindow();
    InitView();
    InitEventHandlers();

    // Set variables
    animating_ = true;
}

       
void Game::InitWindow(void){

    // Initialize the window management library (GLFW)
    if (!glfwInit()){
        throw(GameException(std::string("Could not initialize the GLFW library")));
    }

    // Create a window and its OpenGL context
    if (window_full_screen_g){
        window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), glfwGetPrimaryMonitor(), NULL);
    } else {
        window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), NULL, NULL);
    }
    if (!window_){
        glfwTerminate();
        throw(GameException(std::string("Could not create window")));
    }

    // Make the window's context the current one
    glfwMakeContextCurrent(window_);

    // Initialize the GLEW library to access OpenGL extensions
    // Need to do it after initializing an OpenGL context
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK){
        throw(GameException(std::string("Could not initialize the GLEW library: ")+std::string((const char *) glewGetErrorString(err))));
    }
}


void Game::InitView(void){

    // Set up z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Set viewport
    int width, height;
    glfwGetFramebufferSize(window_, &width, &height);
    glViewport(0, 0, width, height);

    // Set up camera
    // Set current view
    camera_.SetView(camera_position_g, camera_look_at_g, camera_up_g);
    // Set projection
    camera_.SetProjection(camera_fov_g, camera_near_clip_distance_g, camera_far_clip_distance_g, width, height);
}


void Game::InitEventHandlers(void){

    // Set event callbacks
    glfwSetKeyCallback(window_, KeyCallback);
    glfwSetFramebufferSizeCallback(window_, ResizeCallback);

    // Set pointer to game object, so that callbacks can access it
    glfwSetWindowUserPointer(window_, (void *) this);
}


void Game::SetupResources(void){

    // Create a torus
    resman_.CreateTorus("TorusMesh");


    // Load material to be applied to torus
    std::string filename = std::string(MATERIAL_DIRECTORY) + std::string("/three-term_shiny_blue");
    resman_.LoadResource(Material, "ShinyBlueMaterial", filename.c_str());

    // Load a cube from an obj file
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/cube.obj");
    resman_.LoadResource(Mesh, "CubeMesh", filename.c_str());

    // Load texture to be applied to the cube
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/checker.png");
    resman_.LoadResource(Texture, "Checker", filename.c_str());

	// Load texture to be applied to the turret
	filename = std::string(MATERIAL_DIRECTORY) + std::string("/camouflage.png");
	resman_.LoadResource(Texture, "Camouflage", filename.c_str());
	// Load texture to be applied to the turret
	filename = std::string(MATERIAL_DIRECTORY) + std::string("/white.png");
	resman_.LoadResource(Texture, "White", filename.c_str());


    // Load material to be applied to the cube
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/textured_material");
    resman_.LoadResource(Material, "TexturedMaterial", filename.c_str());
}


void Game::SetupScene(void){

    // Set background color for the scene
	scene_.SetBackgroundColor(viewport_background_color_g);
	
    // Create an instance of the torus mesh
    game::SceneNode *torus = CreateInstance("TorusInstance1", "TorusMesh", "ShinyBlueMaterial");
    // Scale the instance
    torus->Scale(glm::vec3(1.5, 1.5, 1.5));
    torus->Translate(glm::vec3(-1.4, 0.0, 0.0));

    // Create an instance of the textured cube
    game::SceneNode *cube = CreateInstance("CubeInstance1", "CubeMesh", "TexturedMaterial", "Checker");
    // Adjust the instance
    cube->Scale(glm::vec3(0.7, 0.7, 0.7));
    glm::quat rotation = glm::angleAxis(-45.0f * -glm::pi<float>()/180.0f, glm::vec3(1.0, 0.0, 0.0));
    cube->Rotate(rotation);
    rotation = glm::angleAxis(-45.0f * -glm::pi<float>()/180.0f, glm::vec3(0.0, 1.0, 0.0));
    cube->Rotate(rotation);
    cube->Translate(glm::vec3(1.4, 0.0, 0.0));

	CreateTurret();
	CreateBird();
	CreateChicken();
}


void Game::CreateBird() {
	resman_.CreatePyramid("Bird_head", 0.44, 0.2, 0.40);
	resman_.CreateCylinder("Bird_body", 0.20, 0.87);
	//							float thick, float bot, float top, float height
	resman_.CreateTrape("Bird_wings", 0.1, 0.46, 0.3, 0.86);
	resman_.CreateTriangle("Bird_wings_tip", 0.1, 0.3, 0.1, 0.66, false);
	//							float thick, float bot, float top, float height, bool tip
	resman_.CreateTriangle("Bird_beak", 0.2, 0.2, 0.03, 0.39, true);

	game::SceneNode *Body = CreateInstance("Body", "Bird_body", "TexturedMaterial", "White");
	Body->Scale(glm::vec3(1.0, 1.0, 1.0));
	Body->Translate(glm::vec3(0.0, 0.0, 10));
	Body->SetOwnTran();


	game::SceneNode *Head = CreateInstance("Head", "Bird_head", "TexturedMaterial", "White");
	Head->Scale(glm::vec3(1.0, 1.0, 1.0));
	Head->Translate(glm::vec3(0.30, 0.9, 0.0));
	Head->SetOwnTran();


	game::SceneNode *LWing = CreateInstance("LWing", "Bird_wings", "TexturedMaterial", "White");
	LWing->Scale(glm::vec3(1.0, 1.0, 1.0));
	glm::quat rotation_LWing = glm::angleAxis(-90 * glm::pi<float>() / 180.0f, glm::vec3(0.0, 0.0, 1.0));
	LWing->Rotate(rotation_LWing);
	glm::quat rotation_LWing2 = glm::angleAxis(180 * glm::pi<float>() / 180.0f, glm::vec3(0.0, 1.0, 0.0));
	LWing->Rotate(rotation_LWing2);
	LWing->Translate(glm::vec3(0.5, 0.3, 0.0));
	LWing->SetOwnTran();

	game::SceneNode *LWing_tip = CreateInstance("LWing_tip", "Bird_wings_tip", "TexturedMaterial", "White");
	LWing_tip->Scale(glm::vec3(1.0, 1.0, 1.0));
	LWing_tip->Translate(glm::vec3(0.16, 0.8, 0.0));
	LWing_tip->SetOwnTran();


	game::SceneNode *RWing = CreateInstance("RWing", "Bird_wings", "TexturedMaterial", "White");
	RWing->Scale(glm::vec3(1.0, 1.0, 1.0));
	glm::quat rotation_RWing = glm::angleAxis(-90 * -glm::pi<float>() / 180.0f, glm::vec3(0.0, 0.0, 1.0));
	RWing->Rotate(rotation_RWing);
	RWing->Translate(glm::vec3(-0.5, 0.3, 0.0));
	RWing->SetOwnTran();

	game::SceneNode *RWing_tip = CreateInstance("LWing_tip", "Bird_wings_tip", "TexturedMaterial", "White");
	RWing_tip->Scale(glm::vec3(1.0, 1.0, 1.0));
	glm::quat rotation_RWing_Tip = glm::angleAxis(180 * -glm::pi<float>() / 180.0f, glm::vec3(1.0, 0.0, 0.0));
	RWing_tip->Rotate(rotation_RWing_Tip);
	RWing_tip->Translate(glm::vec3(0.16, -1.82, 0.0));
	RWing_tip->SetOwnTran();
	

	game::SceneNode *Beak = CreateInstance("Beak", "Bird_beak", "TexturedMaterial", "White");
	Beak->Scale(glm::vec3(1.0, 1.0, 1.0));
	glm::quat rotation_Beak = glm::angleAxis(-90 * glm::pi<float>() / 180.0f, glm::vec3(0.0, 1.0, 0.0));
	Beak->Rotate(rotation_Beak);
	Beak->Translate(glm::vec3(0.0, 1.16, 0.37));
	Beak->SetOwnTran();


	Head->setBaBa(Body);
	LWing->setBaBa(Body);
	RWing->setBaBa(Body);
	Beak->setBaBa(Body);
	LWing_tip->setBaBa(LWing);
	RWing_tip->setBaBa(LWing);
}

void Game::CreateChicken() {
	resman_.CreateSphere("CK_Body", 0.44,0.33,0.33);
	resman_.CreateSphere("CK_Head", 0.25,0.19,0.19);
	//							float thick, float bot, float top, float height, bool tip
	resman_.CreateTriangle("CK_Beak", 0.09, 0.09, 0.01, 0.2, true);
	resman_.CreateCylinder("CK_Legs", 0.036, 0.33);

	game::SceneNode *CK_Body = CreateInstance("CK_Body", "CK_Body", "TexturedMaterial", "White");
	CK_Body->Scale(glm::vec3(1.0, 1.0, 1.0));
	CK_Body->Translate(glm::vec3(0, 0, 20));
	CK_Body->SetOwnTran();

	game::SceneNode *CK_Head = CreateInstance("CK_Head", "CK_Head", "TexturedMaterial", "White");
	CK_Head->Scale(glm::vec3(1.0, 1.0, 1.0));
	CK_Head->Translate(glm::vec3(-0.36, 0.36, 0));
	CK_Head->SetOwnTran();

	game::SceneNode *CK_Beak = CreateInstance("CK_Beak", "CK_Beak", "TexturedMaterial", "White");
	CK_Beak->Scale(glm::vec3(1.0, 1.0, 1.0));
	glm::quat rotation_CKBeak = glm::angleAxis(-90 * glm::pi<float>() / 180.0f, glm::vec3(0.0, 0.0, 1.0));
	CK_Beak->Rotate(rotation_CKBeak);
	rotation_CKBeak = glm::angleAxis(180 * glm::pi<float>() / 180.0f, glm::vec3(1.0, 0.0, 0.0));
	CK_Beak->Rotate(rotation_CKBeak);
	CK_Beak->Translate(glm::vec3(-0.7, -0.45, 0));
	CK_Beak->SetOwnTran();

	game::SceneNode *CK_Lleg = CreateInstance("CK_Lleg", "CK_Legs", "TexturedMaterial", "White");
	CK_Lleg->Scale(glm::vec3(1.0, 1.0, 1.0));
	CK_Lleg->Translate(glm::vec3(0, -0.4, 0.15));
	CK_Lleg->SetOwnTran();

	game::SceneNode *CK_Rleg = CreateInstance("CK_Rleg", "CK_Legs", "TexturedMaterial", "White");
	CK_Rleg->Scale(glm::vec3(1.0, 1.0, 1.0));
	CK_Rleg->Translate(glm::vec3(0, -0.4, -0.15));
	CK_Rleg->SetOwnTran();

	CK_Head->setBaBa(CK_Body);
	CK_Beak->setBaBa(CK_Head);
	CK_Lleg->setBaBa(CK_Body);
	CK_Rleg->setBaBa(CK_Body);
}




void Game::CreateTurret() {

	resman_.CreateCylinder("SimpleCylinderMesh_Center", 0.45, 0.5);
	resman_.CreateCylinder("SimpleCylinderMesh_Base", 0.24, 0.6);
	resman_.CreateCylinder("SimpleCylinderMesh_Canon", 0.09, 0.45);
	resman_.CreateCylinder("SimpleCylinderMesh_Barrel", 0.05, 0.88);

	// Create an instance of the turret mesh
	// Base
	game::SceneNode *base = CreateInstance("Base", "SimpleCylinderMesh_Base", "TexturedMaterial", "Camouflage");
	// Scale the instance
	base->Scale(glm::vec3(1.0, 1.0, 1.0));
	base->Translate(glm::vec3(0.0, 0.0, -7));
	base->SetOwnTran();

	// Center
	game::SceneNode *center = CreateInstance("Center", "SimpleCylinderMesh_Center", "TexturedMaterial", "Camouflage");
	// Scale the instance
	center->Scale(glm::vec3(1.0, 1.0, 1.0));
	center->Translate(glm::vec3(0.0, 0.4, 0.0));
	center->SetOwnTran();

	// Canon
	game::SceneNode *canon = CreateInstance("Canon", "SimpleCylinderMesh_Canon", "TexturedMaterial", "White");
	// Scale the instance
	canon->Scale(glm::vec3(1.0, 1.0, 1.0));
	canon->Translate(glm::vec3(-0.4, 0.0, 0.0));
	canon->SetOwnTran();

	// Barrel
	game::SceneNode *barrel = CreateInstance("Barrel", "SimpleCylinderMesh_Barrel", "TexturedMaterial", "White");
	// Scale the instance
	barrel->Scale(glm::vec3(1.0, 1.0, 1.0));
	barrel->Translate(glm::vec3(0.0, length, 0.0));
	barrel->SetOwnTran();


	center->setBaBa(base);
	canon->setBaBa(center);
	barrel->setBaBa(canon);
}


void Game::MainLoop(void){

    // Loop while the user did not close the window
    while (!glfwWindowShouldClose(window_)){
        // Animate the scene
        if (animating_){
            static double last_time = 0;
            double current_time = glfwGetTime();
            if ((current_time - last_time) > 0.01){
                //scene_.Update();
                // Animate the torus
                SceneNode *node = scene_.GetNode("TorusInstance1");
                glm::quat rotation = glm::angleAxis(glm::pi<float>()/180.0f, glm::vec3(0.0, 1.0, 0.0));
                node->Rotate(rotation);

                // Animate the cube
                node = scene_.GetNode("CubeInstance1");
                rotation = glm::angleAxis(glm::pi<float>()/180.0f, glm::vec3(0.0, 0.0, 1.0));
                node->Rotate(rotation);
                rotation = glm::angleAxis(2.0f * glm::pi<float>()/180.0f, glm::vec3(1.0, 0.0, 0.0));
                node->Rotate(rotation);


				if (angle <= lowAn && increasement < 0) {
					increasement *= -1;
				}
				else if (angle >= highAn && increasement > 0) {
					increasement *= -1;
				}

				angle += increasement;

				// canon in and out
				if (length <= lowLn && LNincrease < 0) {
					LNincrease *= -1;
				}
				else if (length >= highLn && LNincrease > 0) {
					LNincrease *= -1;
				}

				length += LNincrease;

				glm::quat rotation_Base = glm::angleAxis(glm::pi<float>() / 180.0f, glm::vec3(0.0, 1.0, 0.0));
				scene_.GetNode("Base")->Rotate(rotation_Base);

				scene_.GetNode("Canon")->setRotation(angle);

				scene_.GetNode("Barrel")->SetPosition(glm::vec3(0, length, 0));



                last_time = current_time;
            }
        }

        // Draw the scene
        scene_.Draw(&camera_);

        // Push buffer drawn in the background onto the display
        glfwSwapBuffers(window_);

        // Update other events like input handling
        glfwPollEvents();
    }
}


void Game::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){

    // Get user data with a pointer to the game class
    void* ptr = glfwGetWindowUserPointer(window);
    Game *game = (Game *) ptr;

    // Quit game if 'q' is pressed
    if (key == GLFW_KEY_Q && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }

    // Stop animation if space bar is pressed
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS){
        game->animating_ = (game->animating_ == true) ? false : true;
    }

    // View control
    float rot_factor(glm::pi<float>() / 180);
    float trans_factor = 1.0;
    if (key == GLFW_KEY_UP){
        game->camera_.Pitch(rot_factor);
    }
    if (key == GLFW_KEY_DOWN){
        game->camera_.Pitch(-rot_factor);
    }
    if (key == GLFW_KEY_LEFT){
        game->camera_.Yaw(rot_factor);
    }
    if (key == GLFW_KEY_RIGHT){
        game->camera_.Yaw(-rot_factor);
    }
    if (key == GLFW_KEY_S){
        game->camera_.Roll(-rot_factor);
    }
    if (key == GLFW_KEY_X){
        game->camera_.Roll(rot_factor);
    }
    if (key == GLFW_KEY_A){
        game->camera_.Translate(game->camera_.GetForward()*trans_factor);
    }
    if (key == GLFW_KEY_Z){
        game->camera_.Translate(-game->camera_.GetForward()*trans_factor);
    }
    if (key == GLFW_KEY_J){
        game->camera_.Translate(-game->camera_.GetSide()*trans_factor);
    }
    if (key == GLFW_KEY_L){
        game->camera_.Translate(game->camera_.GetSide()*trans_factor);
    }
    if (key == GLFW_KEY_I){
        game->camera_.Translate(game->camera_.GetUp()*trans_factor);
    }
    if (key == GLFW_KEY_K){
        game->camera_.Translate(-game->camera_.GetUp()*trans_factor);
    }
	if (key == GLFW_KEY_B && action == GLFW_PRESS) {
		if (game->camera_.getFlag() == 1) {
			game->camera_.setFlag(0);
			game->scene_.SetBackgroundColor(viewport_background_color_white);
		}
		else {
			game->camera_.setFlag(1);
			game->scene_.SetBackgroundColor(viewport_background_color_g);
		}
	}
}


void Game::ResizeCallback(GLFWwindow* window, int width, int height){

    // Set up viewport and camera projection based on new window size
    glViewport(0, 0, width, height);
    void* ptr = glfwGetWindowUserPointer(window);
    Game *game = (Game *) ptr;
    game->camera_.SetProjection(camera_fov_g, camera_near_clip_distance_g, camera_far_clip_distance_g, width, height);
}


Game::~Game(){
    
    glfwTerminate();

}


Asteroid *Game::CreateAsteroidInstance(std::string entity_name, std::string object_name, std::string material_name){

    // Get resources
    Resource *geom = resman_.GetResource(object_name);
    if (!geom){
        throw(GameException(std::string("Could not find resource \"")+object_name+std::string("\"")));
    }

    Resource *mat = resman_.GetResource(material_name);
    if (!mat){
        throw(GameException(std::string("Could not find resource \"")+material_name+std::string("\"")));
    }

    // Create asteroid instance
    Asteroid *ast = new Asteroid(entity_name, geom, mat);
    scene_.AddNode(ast);
    return ast;
}


void Game::CreateAsteroidField(int num_asteroids){

    // Create a number of asteroid instances
    for (int i = 0; i < num_asteroids; i++){
        // Create instance name
        std::stringstream ss;
        ss << i;
        std::string index = ss.str();
        std::string name = "AsteroidInstance" + index;

        // Create asteroid instance
        Asteroid *ast = CreateAsteroidInstance(name, "SimpleSphereMesh", "ObjectMaterial");

        // Set attributes of asteroid: random position, orientation, and
        // angular momentum
        ast->SetPosition(glm::vec3(-300.0 + 600.0*((float) rand() / RAND_MAX), -300.0 + 600.0*((float) rand() / RAND_MAX), 600.0*((float) rand() / RAND_MAX)));
        ast->SetOrientation(glm::normalize(glm::angleAxis(glm::pi<float>()*((float) rand() / RAND_MAX), glm::vec3(((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX)))));
        ast->SetAngM(glm::normalize(glm::angleAxis(0.05f*glm::pi<float>()*((float) rand() / RAND_MAX), glm::vec3(((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX)))));
    }
}


SceneNode *Game::CreateInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name){

    Resource *geom = resman_.GetResource(object_name);
    if (!geom){
        throw(GameException(std::string("Could not find resource \"")+object_name+std::string("\"")));
    }

    Resource *mat = resman_.GetResource(material_name);
    if (!mat){
        throw(GameException(std::string("Could not find resource \"")+material_name+std::string("\"")));
    }

    Resource *tex = NULL;
    if (texture_name != ""){
        tex = resman_.GetResource(texture_name);
        if (!tex){
            throw(GameException(std::string("Could not find resource \"")+material_name+std::string("\"")));
        }
    }

    SceneNode *scn = scene_.CreateNode(entity_name, geom, mat, tex);
    return scn;
}

} // namespace game
