#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>
#include <SOIL/SOIL.h>

#include "resource_manager.h"
#include "model_loader.h"

namespace game {

ResourceManager::ResourceManager(void){
}


ResourceManager::~ResourceManager(){
}


void ResourceManager::AddResource(ResourceType type, const std::string name, GLuint resource, GLsizei size){

    Resource *res;

    res = new Resource(type, name, resource, size);

    resource_.push_back(res);
}


void ResourceManager::AddResource(ResourceType type, const std::string name, GLuint array_buffer, GLuint element_array_buffer, GLsizei size){

    Resource *res;

    res = new Resource(type, name, array_buffer, element_array_buffer, size);

    resource_.push_back(res);
}


void ResourceManager::LoadResource(ResourceType type, const std::string name, const char *filename) {

	// Call appropriate method depending on type of resource
	if (type == Material) {
		LoadMaterial(name, filename);
	}
	else if (type == Texture) {
		LoadTexture(name, filename);
	}
	else if (type == Mesh) {
		LoadMesh(name, filename);
	}
	else if (type == CubeMap) {
		LoadCubeMap(name, filename);
	}
	else {
		throw(std::invalid_argument(std::string("Invalid type of resource")));
	}
}


Resource *ResourceManager::GetResource(const std::string name) const {

    // Find resource with the specified name
    for (int i = 0; i < resource_.size(); i++){
        if (resource_[i]->GetName() == name){
            return resource_[i];
        }
    }
    return NULL;
}


void ResourceManager::LoadMaterial(const std::string name, const char *prefix) {

	// Load vertex program source code
	std::string filename = std::string(prefix) + std::string(VERTEX_PROGRAM_EXTENSION);
	std::string vp = LoadTextFile(filename.c_str());

	// Load fragment program source code
	filename = std::string(prefix) + std::string(FRAGMENT_PROGRAM_EXTENSION);
	std::string fp = LoadTextFile(filename.c_str());

	// Create a shader from the vertex program source code
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	const char *source_vp = vp.c_str();
	glShaderSource(vs, 1, &source_vp, NULL);
	glCompileShader(vs);

	// Check if shader compiled successfully
	GLint status;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		char buffer[512];
		glGetShaderInfoLog(vs, 512, NULL, buffer);
		throw(std::ios_base::failure(std::string("Error compiling vertex shader: ") + std::string(buffer)));
	}

	// Create a shader from the fragment program source code
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	const char *source_fp = fp.c_str();
	glShaderSource(fs, 1, &source_fp, NULL);
	glCompileShader(fs);

	// Check if shader compiled successfully
	glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		char buffer[512];
		glGetShaderInfoLog(fs, 512, NULL, buffer);
		throw(std::ios_base::failure(std::string("Error compiling fragment shader: ") + std::string(buffer)));
	}

	// Try to also load a geometry shader
	filename = std::string(prefix) + std::string(GEOMETRY_PROGRAM_EXTENSION);
	bool geometry_program = false;
	std::string gp = "";
	GLuint gs;
	try {
		gp = LoadTextFile(filename.c_str());
		geometry_program = true;
	}
	catch (std::exception &e) {
	}

	if (geometry_program) {
		// Create a shader from the geometry program source code
		gs = glCreateShader(GL_GEOMETRY_SHADER);
		const char *source_gp = gp.c_str();
		glShaderSource(gs, 1, &source_gp, NULL);
		glCompileShader(gs);

		// Check if shader compiled successfully
		GLint status;
		glGetShaderiv(gs, GL_COMPILE_STATUS, &status);
		if (status != GL_TRUE) {
			char buffer[512];
			glGetShaderInfoLog(gs, 512, NULL, buffer);
			throw(std::ios_base::failure(std::string("Error compiling geometry shader: ") + std::string(buffer)));
		}
	}

	// Create a shader program linking both vertex and fragment shaders
	// together
	GLuint sp = glCreateProgram();
	glAttachShader(sp, vs);
	glAttachShader(sp, fs);
	if (geometry_program) {
		glAttachShader(sp, gs);
	}
	glLinkProgram(sp);

	// Check if shaders were linked successfully
	glGetProgramiv(sp, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		char buffer[512];
		glGetShaderInfoLog(sp, 512, NULL, buffer);
		throw(std::ios_base::failure(std::string("Error linking shaders: ") + std::string(buffer)));
	}

	// Delete memory used by shaders, since they were already compiled
	// and linked
	glDeleteShader(vs);
	glDeleteShader(fs);
	if (geometry_program) {
		glDeleteShader(gs);
	}

	// Add a resource for the shader program
	AddResource(Material, name, sp, 0);
}

void ResourceManager::CreateSphereParticles(std::string object_name, int num_particles) {

	// Create a set of points which will be the particles
	// This is similar to drawing a sphere: we will sample points on a sphere, but will allow them to also deviate a bit from the sphere along the normal (change of radius)

	// Data buffer
	GLfloat *particle = NULL;

	// Number of attributes per particle: position (3), normal (3), and color (3), texture coordinates (2)
	const int particle_att = 11;

	// Allocate memory for buffer
	try {
		particle = new GLfloat[num_particles * particle_att];
	}
	catch (std::exception &e) {
		throw e;
	}

	float trad = 0.2; // Defines the starting point of the particles along the normal
	float maxspray = 0.5; // This is how much we allow the points to deviate from the sphere
	float u, v, w, theta, phi, spray; // Work variables

	for (int i = 0; i < num_particles; i++) {

		// Get three random numbers
		u = ((double)rand() / (RAND_MAX));
		v = ((double)rand() / (RAND_MAX));
		w = ((double)rand() / (RAND_MAX));

		// Use u to define the angle theta along one direction of the sphere
		theta = u * 2.0*glm::pi<float>();
		// Use v to define the angle phi along the other direction of the sphere
		phi = acos(2.0*v - 1.0);
		// Use w to define how much we can deviate from the surface of the sphere (change of radius)
		spray = maxspray * pow((float)w, (float)(1.0 / 3.0)); // Cubic root of w

		// Define the normal and point based on theta, phi and the spray
		glm::vec3 normal(spray*cos(theta)*sin(phi), spray*sin(theta)*sin(phi), spray*cos(phi));
		glm::vec3 position(normal.x*trad, normal.y*trad, normal.z*trad);
		glm::vec3 color(i / (float)num_particles, 0.0, 1.0 - (i / (float)num_particles)); // We can use the color for debug, if needed

		// Add vectors to the data buffer
		for (int k = 0; k < 3; k++) {
			particle[i*particle_att + k] = position[k];
			particle[i*particle_att + k + 3] = normal[k];
			particle[i*particle_att + k + 6] = color[k];
		}
	}

	// Create OpenGL buffer and copy data
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, num_particles * particle_att * sizeof(GLfloat), particle, GL_STATIC_DRAW);

	// Free data buffers
	delete[] particle;

	// Create resource
	AddResource(PointSet, object_name, vbo, 0, num_particles);
}


std::string ResourceManager::LoadTextFile(const char *filename) {

	// Open file
	std::ifstream f;
	f.open(filename);
	if (f.fail()) {
		throw(std::ios_base::failure(std::string("Error opening file ") + std::string(filename)));
	}

	// Read file
	std::string content;
	std::string line;
	while (std::getline(f, line)) {
		content += line + "\n";
	}

	// Close file
	f.close();

	return content;
}

void ResourceManager::LoadTexture(const std::string name, const char *filename) {

	// Load texture from file
	GLuint texture = SOIL_load_OGL_texture(filename, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	if (!texture) {
		throw(std::ios_base::failure(std::string("Error loading texture ") + std::string(filename) + std::string(": ") + std::string(SOIL_last_result())));
	}

	// Create resource
	AddResource(Texture, name, texture, 0);
}

void ResourceManager::LoadMesh(const std::string name, const char *filename) {

	// First load model into memory. If that goes well, we transfer the
	// mesh to an OpenGL buffer
	TriMesh mesh;

	// Parse file
	// Open file
	std::ifstream f;
	f.open(filename);
	if (f.fail()) {
		throw(std::ios_base::failure(std::string("Error opening file ") + std::string(filename)));
	}

	// Parse lines
	std::string line;
	std::string ignore(" \t\r\n");
	std::string part_separator(" \t");
	std::string face_separator("/");
	bool added_normal = false;
	while (std::getline(f, line)) {
		// Clean extremities of the string
		string_trim(line, ignore);
		// Ignore comments
		if ((line.size() <= 0) ||
			(line[0] == '#')) {
			continue;
		}
		// Parse string
		std::vector<std::string> part = string_split(line, part_separator);
		// Check commands
		if (!part[0].compare(std::string("v"))) {
			if (part.size() >= 4) {
				glm::vec3 position(str_to_num<float>(part[1].c_str()), str_to_num<float>(part[2].c_str()), str_to_num<float>(part[3].c_str()));
				mesh.position.push_back(position);

			}
			else {
				throw(std::ios_base::failure(std::string("Error: v command should have exactly 3 parameters")));
			}
		}
		else if (!part[0].compare(std::string("vn"))) {
			if (part.size() >= 4) {
				glm::vec3 normal(str_to_num<float>(part[1].c_str()), str_to_num<float>(part[2].c_str()), str_to_num<float>(part[3].c_str()));
				mesh.normal.push_back(normal);
				added_normal = true;
			}
			else {
				throw(std::ios_base::failure(std::string("Error: vn command should have exactly 3 parameters")));
			}
		}
		else if (!part[0].compare(std::string("vt"))) {
			if (part.size() >= 3) {
				glm::vec2 tex_coord(str_to_num<float>(part[1].c_str()), str_to_num<float>(part[2].c_str()));
				mesh.tex_coord.push_back(tex_coord);
			}
			else {
				throw(std::ios_base::failure(std::string("Error: vt command should have exactly 2 parameters")));
			}
		}
		else if (!part[0].compare(std::string("f"))) {
			if (part.size() >= 4) {
				if (part.size() > 5) {
					throw(std::ios_base::failure(std::string("Error: f commands with more than 4 vertices not supported")));
				}
				else if (part.size() == 5) {
					// Break a quad into two triangles
					Quad quad;
					for (int i = 0; i < 4; i++) {
						std::vector<std::string> fd = string_split_once(part[i + 1], face_separator);
						if (fd.size() == 1) {
							quad.i[i] = str_to_num<float>(fd[0].c_str()) - 1;
							quad.t[i] = -1;
							quad.n[i] = -1;
						}
						else if (fd.size() == 2) {
							quad.i[i] = str_to_num<float>(fd[0].c_str()) - 1;
							quad.t[i] = str_to_num<float>(fd[1].c_str()) - 1;
							quad.n[i] = -1;
						}
						else if (fd.size() == 3) {
							quad.i[i] = str_to_num<float>(fd[0].c_str()) - 1;
							if (std::string("").compare(fd[1]) != 0) {
								quad.t[i] = str_to_num<float>(fd[1].c_str()) - 1;
							}
							else {
								quad.t[i] = -1;
							}
							quad.n[i] = str_to_num<float>(fd[2].c_str()) - 1;
						}
						else {
							quad.i[i] = str_to_num<float>(fd[i].c_str()) - 1;
							throw(std::ios_base::failure(std::string("Error: f parameter should have 1 or 3 parameters separated by '/'")));
						}
					}
					Face face1, face2;
					face1.i[0] = quad.i[0]; face1.i[1] = quad.i[1]; face1.i[2] = quad.i[2];
					face1.n[0] = quad.n[0]; face1.n[1] = quad.n[1]; face1.n[2] = quad.n[2];
					face1.t[0] = quad.t[0]; face1.t[1] = quad.t[1]; face1.t[2] = quad.t[2];
					face2.i[0] = quad.i[0]; face2.i[1] = quad.i[2]; face2.i[2] = quad.i[3];
					face2.n[0] = quad.n[0]; face2.n[1] = quad.n[2]; face2.n[2] = quad.n[3];
					face2.t[0] = quad.t[0]; face2.t[1] = quad.t[2]; face2.t[2] = quad.t[3];
					mesh.face.push_back(face1);
					mesh.face.push_back(face2);
				}
				else if (part.size() == 4) {
					Face face;
					for (int i = 0; i < 3; i++) {
						std::vector<std::string> fd = string_split_once(part[i + 1], face_separator);
						if (fd.size() == 1) {
							face.i[i] = str_to_num<float>(fd[0].c_str()) - 1;
							face.t[i] = -1;
							face.n[i] = -1;
						}
						else if (fd.size() == 2) {
							face.i[i] = str_to_num<float>(fd[0].c_str()) - 1;
							face.t[i] = str_to_num<float>(fd[1].c_str()) - 1;
							face.n[i] = -1;
						}
						else if (fd.size() == 3) {
							face.i[i] = str_to_num<float>(fd[0].c_str()) - 1;
							if (std::string("").compare(fd[1]) != 0) {
								face.t[i] = str_to_num<float>(fd[1].c_str()) - 1;
							}
							else {
								face.t[i] = -1;
							}
							face.n[i] = str_to_num<float>(fd[2].c_str()) - 1;
						}
						else {
							throw(std::ios_base::failure(std::string("Error: f parameter should have 1, 2, or 3 parameters separated by '/'")));
						}
					}
					mesh.face.push_back(face);
				}
			}
			else {
				throw(std::ios_base::failure(std::string("Error: f command should have 3 or 4 parameters")));
			}
		}
		// Ignore other commands
	}

	// Close file
	f.close();

	// Check if vertex references are correct
	for (unsigned int i = 0; i < mesh.face.size(); i++) {
		for (int j = 0; j < 3; j++) {
			if (mesh.face[i].i[j] >= mesh.position.size()) {
				throw(std::ios_base::failure(std::string("Error: index for triangle ") + num_to_str<int>(mesh.face[i].i[j]) + std::string(" is out of bounds")));
			}
		}
	}

	// Compute degree of each vertex
	std::vector<int> degree(mesh.position.size(), 0);
	for (unsigned int i = 0; i < mesh.face.size(); i++) {
		for (int j = 0; j < 3; j++) {
			degree[mesh.face[i].i[j]]++;
		}
	}

	// Compute vertex normals if no normals were ever added
	if (!added_normal) {
		mesh.normal = std::vector<glm::vec3>(mesh.position.size(), glm::vec3(0.0, 0.0, 0.0));
		for (unsigned int i = 0; i < mesh.face.size(); i++) {
			// Compute face normal
			glm::vec3 vec1, vec2;
			vec1 = mesh.position[mesh.face[i].i[0]] -
				mesh.position[mesh.face[i].i[1]];
			vec2 = mesh.position[mesh.face[i].i[0]] -
				mesh.position[mesh.face[i].i[2]];
			glm::vec3 norm = glm::cross(vec1, vec2);
			norm = glm::normalize(norm);
			// Add face normal to vertices
			mesh.normal[mesh.face[i].i[0]] += norm;
			mesh.normal[mesh.face[i].i[1]] += norm;
			mesh.normal[mesh.face[i].i[2]] += norm;
		}
		for (unsigned int i = 0; i < mesh.normal.size(); i++) {
			if (degree[i] > 0) {
				mesh.normal[i] /= degree[i];
			}
		}
	}

	// Debug
	//print_mesh(mesh);

	// If we got to this point, the file was parsed successfully and the
	// mesh is in memory
	// Now, transfer the mesh to OpenGL buffers
	// Create three new vertices for each face, in case vertex
	// normals/texture coordinates are not consistent over the mesh

	// Number of attributes for vertices and faces
	const int vertex_att = 11;
	const int face_att = 3;

	// Create OpenGL buffers and copy data
	GLuint vbo, ebo;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh.face.size() * 3 * vertex_att * sizeof(GLuint), 0, GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.face.size() * face_att * sizeof(GLuint), 0, GL_STATIC_DRAW);

	unsigned int vertex_index = 0;
	for (unsigned int i = 0; i < mesh.face.size(); i++) {
		// Add three vertices and their attributes
		GLfloat att[3 * vertex_att] = { 0 };
		for (int j = 0; j < 3; j++) {
			// Position
			att[j*vertex_att + 0] = mesh.position[mesh.face[i].i[j]][0];
			att[j*vertex_att + 1] = mesh.position[mesh.face[i].i[j]][1];
			att[j*vertex_att + 2] = mesh.position[mesh.face[i].i[j]][2];
			// Normal
			if (!added_normal) {
				att[j*vertex_att + 3] = mesh.normal[mesh.face[i].i[j]][0];
				att[j*vertex_att + 4] = mesh.normal[mesh.face[i].i[j]][1];
				att[j*vertex_att + 5] = mesh.normal[mesh.face[i].i[j]][2];
			}
			else {
				if (mesh.face[i].n[j] >= 0) {
					att[j*vertex_att + 3] = mesh.normal[mesh.face[i].n[j]][0];
					att[j*vertex_att + 4] = mesh.normal[mesh.face[i].n[j]][1];
					att[j*vertex_att + 5] = mesh.normal[mesh.face[i].n[j]][2];
				}
			}
			// No color in (6, 7, 8)
			// Texture coordinates
			if (mesh.face[i].t[j] >= 0) {
				att[j*vertex_att + 9] = mesh.tex_coord[mesh.face[i].t[j]][0];
				att[j*vertex_att + 10] = mesh.tex_coord[mesh.face[i].t[j]][1];
			}
		}

		// Copy attributes to buffer
		glBufferSubData(GL_ARRAY_BUFFER, i * 3 * vertex_att * sizeof(GLfloat), 3 * vertex_att * sizeof(GLfloat), att);

		// Add triangle
		GLuint findex[face_att] = { 0 };
		findex[0] = vertex_index;
		findex[1] = vertex_index + 1;
		findex[2] = vertex_index + 2;
		vertex_index += 3;

		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, i * face_att * sizeof(GLuint), face_att * sizeof(GLuint), findex);
	}

	// Create resource
	AddResource(Mesh, name, vbo, ebo, mesh.face.size() * face_att);
}
void ResourceManager::LoadCubeMap(const std::string name, const char *filename) {

	// Get base and extension of filename
	std::string fn(filename);
	int pos = fn.find(".");
	std::string base = fn.substr(0, pos);
	std::string ext = fn.substr(pos + 1);

	// Create filenames of each individual cube face
	std::string fn_xp = base + "_ft." + ext;
	std::string fn_xn = base + "_bk." + ext;
	std::string fn_yp = base + "_up." + ext;
	std::string fn_yn = base + "_dn." + ext;
	std::string fn_zp = base + "_rt." + ext;
	std::string fn_zn = base + "_lf." + ext;

	// Load cube map from file
	GLuint texture = SOIL_load_OGL_cubemap(fn_xp.c_str(), fn_xn.c_str(), fn_yp.c_str(), fn_yn.c_str(), fn_zp.c_str(), fn_zn.c_str(), SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	if (!texture) {
		throw(std::ios_base::failure(std::string("Error loading cube map ") + std::string(base) + std::string("<spec>.") + std::string(ext) + std::string(": ") + std::string(SOIL_last_result())));
	}

	// Create resource
	AddResource(CubeMap, name, texture, 0);
}

void ResourceManager::CreateCube(std::string object_name) {

	// This construction uses shared vertices, following the same data
	// format as the other functions 
	// However, vertices are repeated since their normals at each face
	// are different
	// Each face of the cube is defined by four vertices (with the same normal) and two triangles

	// Vertices used to build the cube
	// 11 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3), texture coordinates (2)
	GLfloat vertex[] = {
		// First cube face 
		-0.5, -0.5,  0.5,    0.0,  0.0,  1.0,    1.0, 0.0, 0.0,    0.0, 0.0,
		 0.5, -0.5,  0.5,    0.0,  0.0,  1.0,    0.0, 1.0, 0.0,    1.0, 0.0,
		 0.5,  0.5,  0.5,    0.0,  0.0,  1.0,    0.0, 0.0, 1.0,    1.0, 1.0,
		-0.5,  0.5,  0.5,    0.0,  0.0,  1.0,    1.0, 0.0, 1.0,    0.0, 1.0,
		// Second cube face
		 0.5, -0.5, -0.5,    1.0,  0.0,  0.0,    1.0, 0.0, 0.0,    0.0, 0.0,
		 0.5,  0.5, -0.5,    1.0,  0.0,  0.0,    0.0, 1.0, 0.0,    1.0, 0.0,
		 0.5,  0.5,  0.5,    1.0,  0.0,  0.0,    0.0, 0.0, 1.0,    1.0, 1.0,
		 0.5, -0.5,  0.5,    1.0,  0.0,  0.0,    1.0, 0.0, 1.0,    0.0, 1.0,
		 // Third cube face
		  0.5, -0.5, -0.5,    0.0,  0.0, -1.0,    1.0, 0.0, 0.0,    0.0, 0.0,
		 -0.5, -0.5, -0.5,    0.0,  0.0, -1.0,    0.0, 1.0, 0.0,    1.0, 0.0,
		 -0.5,  0.5, -0.5,    0.0,  0.0, -1.0,    0.0, 0.0, 1.0,    1.0, 1.0,
		  0.5,  0.5, -0.5,    0.0,  0.0, -1.0,    1.0, 0.0, 1.0,    0.0, 1.0,
		  // Fourth cube face
		  -0.5,  0.5, -0.5,   -1.0,  0.0,  0.0,    1.0, 0.0, 0.0,    0.0, 0.0,
		  -0.5, -0.5, -0.5,   -1.0,  0.0,  0.0,    0.0, 1.0, 0.0,    1.0, 0.0,
		  -0.5, -0.5,  0.5,   -1.0,  0.0,  0.0,    0.0, 0.0, 1.0,    1.0, 1.0,
		  -0.5,  0.5,  0.5,   -1.0,  0.0,  0.0,    1.0, 0.0, 1.0,    0.0, 1.0,
		  // Fifth cube face
		  -0.5,  0.5, -0.5,    0.0,  1.0,  0.0,    1.0, 0.0, 0.0,    0.0, 0.0,
		  -0.5,  0.5,  0.5,    0.0,  1.0,  0.0,    0.0, 1.0, 0.0,    0.0, 1.0,
		   0.5,  0.5,  0.5,    0.0,  1.0,  0.0,    0.0, 0.0, 1.0,    1.0, 1.0,
		   0.5,  0.5, -0.5,    0.0,  1.0,  0.0,    1.0, 0.0, 1.0,    1.0, 0.0,
		   // Sixth cube face
			0.5, -0.5, -0.5,    0.0, -1.0,  0.0,    1.0, 0.0, 0.0,    0.0, 0.0,
		   -0.5, -0.5, -0.5,    0.0, -1.0,  0.0,    0.0, 1.0, 0.0,    1.0, 0.0,
		   -0.5, -0.5,  0.5,    0.0, -1.0,  0.0,    0.0, 0.0, 1.0,    1.0, 1.0,
			0.5, -0.5,  0.5,    0.0, -1.0,  0.0,    1.0, 0.0, 1.0,    0.0, 1.0,
	};

	// Triangles
	GLuint face[] = {
		// First cube face, with two triangles
		0, 1, 2,
		0, 2, 3,
		// Second face
		4, 5, 6,
		4, 6, 7,
		// Third face
		8, 9, 10,
		8, 10, 11,
		// Fourth face
		12, 13, 14,
		12, 14, 15,
		// Fifth face
		16, 17, 18,
		16, 18, 19,
		// Sixth face
		20, 21, 22,
		20, 22, 23,
	};

	// Create OpenGL buffers and copy data
	GLuint vbo, ebo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(face), face, GL_STATIC_DRAW);

	// Create resource
	AddResource(Mesh, object_name, vbo, ebo, sizeof(face) / sizeof(GLfloat));
}

void ResourceManager::Create2Dsquare(std::string object_name)
// Create the geometry of a square with sides of length 1 centered at (0, 0, 0) and facing (0, 0, 1)
{

	// The construction does *not* use shared vertices
	// 9 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3) , uv (2)
	GLfloat vertex[] = {
		-0.5,  0.5,  0.0,    0.0,  0.0,  1.0,    0.0, 1.0, 1.0,		0, 0,
		-0.5, -0.5,  0.0,    0.0,  0.0,  1.0,    1.0, 0.0, 0.0,		0, 1,
		 0.5, -0.5,  0.0,    0.0,  0.0,  1.0,    0.0, 1.0, 0.0,		1, 1,
		 0.5,  0.5,  0.0,    0.0,  0.0,  1.0,    0.0, 0.0, 1.0,		1, 0,
	};
	GLuint face[] = {
		0,1,2,
		0,2,3,
	};

	
	GLuint vbo, ebo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 4 * 11 * sizeof(GLfloat), vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * 3 * sizeof(GLuint), face, GL_STATIC_DRAW);
	
	// Create resource
	AddResource(Mesh, object_name, vbo, ebo, 2 * 3);
	
}


void ResourceManager::CreateSphere(std::string object_name, float radius_x, float radius_y, float radius_z, int num_samples_theta, int num_samples_phi) {
	// Create a sphere using a well-known parameterization

	// Number of vertices and faces to be created
	const GLuint vertex_num = num_samples_theta * num_samples_phi;
	const GLuint face_num = num_samples_theta * (num_samples_phi - 1) * 2;

	// Number of attributes for vertices and faces
	const int vertex_att = 11;
	const int face_att = 3;

	// Data buffers 
	GLfloat *vertex = NULL;
	GLuint *face = NULL;

	// Allocate memory for buffers
	try {
		vertex = new GLfloat[vertex_num * vertex_att]; // 11 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3), 2D texture coordinates (2)
		face = new GLuint[face_num * face_att]; // 3 indices per face
	}
	catch (std::exception &e) {
		throw e;
	}

	// Create vertices 
	float theta, phi; // Angles for parametric equation
	glm::vec3 vertex_position;
	glm::vec3 vertex_normal;
	glm::vec3 vertex_color;
	glm::vec2 vertex_coord;

	for (int i = 0; i < num_samples_theta; i++) {

		theta = 2.0*glm::pi<GLfloat>()*i / (num_samples_theta - 1); // angle theta

		for (int j = 0; j < num_samples_phi; j++) {

			phi = glm::pi<GLfloat>()*j / (num_samples_phi - 1); // angle phi

			// Define position, normal and color of vertex
			vertex_normal = glm::vec3(cos(theta)*sin(phi), sin(theta)*sin(phi), -cos(phi));
			// We need z = -cos(phi) to make sure that the z coordinate runs from -1 to 1 as phi runs from 0 to pi
			// Otherwise, the normal will be inverted
			vertex_position = glm::vec3(vertex_normal.x*radius_x,
				vertex_normal.y*radius_y,
				vertex_normal.z*radius_z),
				vertex_color = glm::vec3(((float)i) / ((float)num_samples_theta), 1.0 - ((float)j) / ((float)num_samples_phi), ((float)j) / ((float)num_samples_phi));
			vertex_coord = glm::vec2(((float)i) / ((float)num_samples_theta), 1.0 - ((float)j) / ((float)num_samples_phi));

			// Add vectors to the data buffer
			for (int k = 0; k < 3; k++) {
				vertex[(i*num_samples_phi + j)*vertex_att + k] = vertex_position[k];
				vertex[(i*num_samples_phi + j)*vertex_att + k + 3] = vertex_normal[k];
				vertex[(i*num_samples_phi + j)*vertex_att + k + 6] = vertex_color[k];
			}
			vertex[(i*num_samples_phi + j)*vertex_att + 9] = vertex_coord[0];
			vertex[(i*num_samples_phi + j)*vertex_att + 10] = vertex_coord[1];
		}
	}

	// Create faces
	for (int i = 0; i < num_samples_theta; i++) {
		for (int j = 0; j < (num_samples_phi - 1); j++) {
			// Two triangles per quad
			glm::vec3 t1(((i + 1) % num_samples_theta)*num_samples_phi + j,
				i*num_samples_phi + (j + 1),
				i*num_samples_phi + j);
			glm::vec3 t2(((i + 1) % num_samples_theta)*num_samples_phi + j,
				((i + 1) % num_samples_theta)*num_samples_phi + (j + 1),
				i*num_samples_phi + (j + 1));
			// Add two triangles to the data buffer
			for (int k = 0; k < 3; k++) {
				face[(i*(num_samples_phi - 1) + j)*face_att * 2 + k] = (GLuint)t1[k];
				face[(i*(num_samples_phi - 1) + j)*face_att * 2 + k + face_att] = (GLuint)t2[k];
			}
		}
	}

	// Create OpenGL buffers and copy data
	//GLuint vao;
	//glGenVertexArrays(1, &vao);
	//glBindVertexArray(vao);

	GLuint vbo, ebo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertex_num * vertex_att * sizeof(GLfloat), vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_num * face_att * sizeof(GLuint), face, GL_STATIC_DRAW);

	// Free data buffers
	delete[] vertex;
	delete[] face;

	// Create resource
	AddResource(Mesh, object_name, vbo, ebo, face_num * face_att);
}


void ResourceManager::CreateMirror(std::string object_name)
{
	int segment_num = 800;
	int vertex_num = segment_num + 1;
	int vertex_att = 11;
	int face_num = segment_num;
	int face_att = 3;

	GLfloat *vertex = NULL;
	GLuint *face = NULL;

	try {
		vertex = new GLfloat[vertex_num * vertex_att]; // 11 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3), 2D texture coordinates (2)
		face = new GLuint[face_num * face_att]; // 3 indices per face
	}
	catch (std::exception &e) {
		throw e;
	}

	float theta = 2.0*glm::pi<GLfloat>() / (segment_num-1);

	// origin
	vertex[segment_num * vertex_att + 0] = 0.0;
	vertex[segment_num * vertex_att + 1] = 0.0;
	vertex[segment_num * vertex_att + 2] = 0.0;
	vertex[segment_num * vertex_att + 3] = 0.0;
	vertex[segment_num * vertex_att + 4] = 0.0;
	vertex[segment_num * vertex_att + 6] = 1.0;
	vertex[segment_num * vertex_att + 6] = 0.0;
	vertex[segment_num * vertex_att + 7] = 0.0;
	vertex[segment_num * vertex_att + 8] = 0.0;
	vertex[segment_num * vertex_att + 9] = 0.0;
	vertex[segment_num * vertex_att + 10] = 0.0;

	float radius = rand() / (float)RAND_MAX / 3 + 0.5;
	int dir = 1;	// radius inrease(1) or decrease(1)
	float delta = 0.01;
	int count = 1;	// count the time of increasing(>0) or decreasing(<0) 
	for (int i = 0; i <= segment_num;i++) {
		if (i % 4 == 0) {
			if ((segment_num - i) / 4 <= abs(count)) {
				dir = count > 0 ? -1 : 1;
			}
			else{
				dir = rand() % 2 == 0 ? 1 : -1;
				count += dir;
			}
		}
		radius += dir * delta;
		vertex[i*vertex_att + 0] = cos(i*theta) * 0.5;
		vertex[i*vertex_att + 1] = sin(i*theta) * 0.5;
		vertex[i*vertex_att + 2] = 0.0;
		vertex[i*vertex_att + 3] = 0.0;
		vertex[i*vertex_att + 4] = 0.0;
		vertex[i*vertex_att + 5] = 1.0;
		vertex[i*vertex_att + 6] = 0.0;
		vertex[i*vertex_att + 7] = 0.0;
		vertex[i*vertex_att + 8] = 0.0;
		vertex[i*vertex_att + 9] = 0.0;
		vertex[i*vertex_att + 10] = 0.0;
	}

	// Create triangles
	for (int i = 0; i < segment_num; i++) {
		// Two triangles per quad
		glm::vec3 t(segment_num, i , (i + 1) % segment_num);

		// Add two triangles to the data buffer
		for (int k = 0; k < 3; k++) {
			face[i*face_att + k] = (GLuint)t[k];
		}
	}

	GLuint vbo, ebo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertex_num * vertex_att * sizeof(GLfloat), vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_num * face_att * sizeof(GLuint), face, GL_STATIC_DRAW);

	// Free data buffers
	delete[] vertex;
	delete[] face;

	// Create resource
	AddResource(Mesh, object_name, vbo, ebo, face_num * face_att);

}

void ResourceManager::CreatePyramid(std::string object_name, float bot, float top, float height) {


	// Number of attributes for vertices and faces
	const int vertex_att = 11;
	const int face_att = 3;

	float half_thick = bot / 2;
	float top_x = (bot - top) / 2;
	float bot_uv = bot / (2 * height + top);
	float height_uv_zhen = height / (2 * height + top);
	float height_uv_fan = (height + top) / (2 * height + top);

	// Data buffers 
	GLfloat vertex[] = {
		//position												normal			color			uv
		//front
		-0.5,-0.5,half_thick,									0.0,0.0,0.0,	0.5,0.5,0.5,	0.0,bot_uv,//bot left
		-0.5 + bot,-0.5,half_thick,								0.0,0.0,0.0,	0.5,0.5,0.5,	0.0,0.0,//bot right
		-0.5 + top_x + top,-0.5 + height,top / 2,		0.0,0.0,0.0,	0.5,0.5,0.5,	height_uv_zhen,top_x / bot,//top right
		-0.5 + top_x,-0.5 + height,top / 2,			0.0,0.0,0.0,	0.5,0.5,0.5,	height_uv_zhen,(top_x + top) / bot,//top left
				//back
		-0.5,-0.5,-half_thick,									0.0,0.0,0.0,	0.5,0.5,0.5,	1,bot_uv,//bot left
		-0.5 + bot,-0.5,-half_thick,							0.0,0.0,0.0,	0.5,0.5,0.5,	1.0,0.0,//bot right
		-0.5 + bot - top_x,-0.5 + height,-top / 2,	0.0,0.0,0.0,	0.5,0.5,0.5,	height_uv_fan,top_x / bot,//top right
		-0.5 + top_x,-0.5 + height,-top / 2,			0.0,0.0,0.0,	0.5,0.5,0.5,	height_uv_fan,(top_x + top) / bot,//top left

	};

	GLuint face[] = {
		3,0,1,
		1,2,3,
		6,5,4,
		4,7,6,
		6,7,3,
		3,2,6,
		3,7,4,
		4,0,3,
		6,2,1,
		1,5,6,
		0,4,5,
		5,1,0,
	};


	GLuint vbo, ebo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 8 * vertex_att * sizeof(GLfloat), vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 12 * face_att * sizeof(GLuint), face, GL_STATIC_DRAW);


	// Create resource
	AddResource(Mesh, object_name, vbo, ebo, 12 * face_att);
}

// Bird big wingsq
void ResourceManager::CreateTrape(std::string object_name, float thick, float bot, float top, float height) {


	// Number of attributes for vertices and faces
	const int vertex_att = 11;
	const int face_att = 3;

	float half_thick = thick / 2;

	// Data buffers 
	GLfloat vertex[] = {
		//position								normal			color			uv
		//front
		-0.5,-0.5,half_thick,					0.0,0.0,0.0,	0.5,0.5,0.5,	0.0,0.0,//bot left
		-0.5 + bot,-0.5,half_thick,			0.0,0.0,0.0,	0.5,0.5,0.5,	bot / (bot + top),0.0,//bot right
		-0.5 + bot,-0.5 + height,half_thick,	0.0,0.0,0.0,	0.5,0.5,0.5,	1.0,1.0,//top right
		-0.5 + bot - top,-0.5 + height,half_thick,		0.0,0.0,0.0,	0.5,0.5,0.5,	bot / (bot + top),1.0,//top left
		//back
		-0.5,-0.5,-half_thick,					0.0,0.0,0.0,	0.5,0.5,0.5,	0.0,0.0,//bot left
		-0.5 + bot,-0.5,-half_thick,			0.0,0.0,0.0,	0.5,0.5,0.5,	bot / (bot + top),0.0,//bot right
		-0.5 + bot,-0.5 + height,-half_thick,	0.0,0.0,0.0,	0.5,0.5,0.5,	1.0,1.0,//top right
		-0.5 + bot - top,-0.5 + height,-half_thick,		0.0,0.0,0.0,	0.5,0.5,0.5,	bot / (bot + top),1.0,//top left

	};

	GLuint face[] = {
		0,1,3,
		3,2,1,
		7,5,4,
		7,6,5,
		0,3,4,
		3,7,4,
		6,7,3,
		3,2,6,
		6,5,1,
		1,2,6,
		4,5,1,
		1,0,4,
	};


	GLuint vbo, ebo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 8 * vertex_att * sizeof(GLfloat), vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 12 * face_att * sizeof(GLuint), face, GL_STATIC_DRAW);


	// Create resource
	AddResource(Mesh, object_name, vbo, ebo, 12 * face_att);
}


// Bird wings and tail and mouse
void ResourceManager::CreateTriangle(std::string object_name, float thick, float bot, float top, float height, bool tip) {


	// Number of attributes for vertices and faces
	const int vertex_att = 11;
	const int face_att = 3;

	float half_thick = thick / 2;

	float tip_orNot = 0;
	if (tip) {
		tip_orNot = top / 2;
	}
	else {
		tip_orNot = half_thick;
	}

	// Data buffers 
	GLfloat vertex[] = {
		//position								normal			color			uv
		//front
		-0.5,-0.5,half_thick,					0.0,0.0,0.0,	0.5,0.5,0.5,	0.0,0.0,//bot left
		-0.5 + bot,-0.5,half_thick,			0.0,0.0,0.0,	0.5,0.5,0.5,	bot / (bot + top),0.0,//bot right
		-0.5 + bot + top,-0.5 + height,tip_orNot,		0.0,0.0,0.0,	0.5,0.5,0.5,	1.0,1.0,//top right
		-0.5 + bot,-0.5 + height,tip_orNot,			0.0,0.0,0.0,	0.5,0.5,0.5,	bot / (bot + top),1.0,//top left
		//back
		-0.5,-0.5,-half_thick,					0.0,0.0,0.0,	0.5,0.5,0.5,	0.0,0.0,//bot left
		-0.5 + bot,-0.5,-half_thick,			0.0,0.0,0.0,	0.5,0.5,0.5,	bot / (bot + top),0.0,//bot right
		-0.5 + bot + top,-0.5 + height,-tip_orNot,	0.0,0.0,0.0,	0.5,0.5,0.5,	1.0,1.0,//top right
		-0.5 + bot,-0.5 + height,-tip_orNot,		0.0,0.0,0.0,	0.5,0.5,0.5,	bot / (bot + top),1.0,//top left

	};

	GLuint face[] = {
		0,1,3,
		3,2,1,
		7,5,4,
		7,6,5,
		0,3,4,
		3,7,4,
		6,7,3,
		3,2,6,
		6,5,1,
		1,2,6,
		4,5,1,
		1,0,4,
	};


	GLuint vbo, ebo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 8 * vertex_att * sizeof(GLfloat), vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 12 * face_att * sizeof(GLuint), face, GL_STATIC_DRAW);


	// Create resource
	AddResource(Mesh, object_name, vbo, ebo, 12 * face_att);
}

void ResourceManager::CreateTail(const std::string object_name, float tail_diff, float thick) {

	// Number of attributes for vertices and faces
	const int vertex_att = 11;
	const int face_att = 3;

	float half_thick = thick / 2;

	// Data buffers 
	GLfloat vertex[] = {
		//position							normal			color			uv
		//top 2 point
		0,0.5,half_thick,					0.0,0.0,0.0,	0.5,0.5,0.5,	0.5,0.5,
		0,0.5,-half_thick,					0.0,0.0,0.0,	0.5,0.5,0.5,	0.5,0.5,
		//bot 4 point
		-1.5*tail_diff, 0.4 - 2 * tail_diff, 0,	0.0,0.0,0.0,	0.5,0.5,0.5,	0.5,0.5,
		-0.5*tail_diff, 0.4 - 2.5 * tail_diff, 0,	0.0,0.0,0.0,	0.5,0.5,0.5,	0.5,0.5,
		0.5*tail_diff, 0.4 - 2.5 * tail_diff, 0,	0.0,0.0,0.0,	0.5,0.5,0.5,	0.5,0.5,
		1.5*tail_diff, 0.4 - 2 * tail_diff, 0,	0.0,0.0,0.0,	0.5,0.5,0.5,	0.5,0.5,

	};

	GLuint face[] = {
		0,2,3,
		0,3,4,
		0,4,5,
		1,2,3,
		1,3,4,
		1,4,5,
		0,1,2,
		1,0,5,
	};


	GLuint vbo, ebo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 6 * vertex_att * sizeof(GLfloat), vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 8 * face_att * sizeof(GLuint), face, GL_STATIC_DRAW);


	// Create resource
	AddResource(Mesh, object_name, vbo, ebo, 12 * face_att);
}

void ResourceManager::CreateTorus(std::string object_name, float loop_radius, float circle_radius, int num_loop_samples, int num_circle_samples){

    // Create a torus
    // The torus is built from a large loop with small circles around the loop

    // Number of vertices and faces to be created
    // Check the construction algorithm below to understand the numbers
    // specified below
    const GLuint vertex_num = num_loop_samples*num_circle_samples;
    const GLuint face_num = num_loop_samples*num_circle_samples*2;

    // Number of attributes for vertices and faces
    const int vertex_att = 11;
    const int face_att = 3;

    // Data buffers for the torus
    GLfloat *vertex = NULL;
    GLuint *face = NULL;

    // Allocate memory for buffers
    try {
        vertex = new GLfloat[vertex_num * vertex_att]; // 11 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3), 2D texture coordinates (2)
        face = new GLuint[face_num * face_att]; // 3 indices per face
    }
    catch  (std::exception &e){
        throw e;
    }

    // Create vertices 
    float theta, phi; // Angles for circles
    glm::vec3 loop_center;
    glm::vec3 vertex_position;
    glm::vec3 vertex_normal;
    glm::vec3 vertex_color;
    glm::vec2 vertex_coord;

    for (int i = 0; i < num_loop_samples; i++){ // large loop
        
        theta = 2.0*glm::pi<GLfloat>()*i/num_loop_samples; // loop sample (angle theta)
        loop_center = glm::vec3(loop_radius*cos(theta), loop_radius*sin(theta), 0); // centre of a small circle

        for (int j = 0; j < num_circle_samples; j++){ // small circle
            
            phi = 2.0*glm::pi<GLfloat>()*j/num_circle_samples; // circle sample (angle phi)
            
            // Define position, normal and color of vertex
            vertex_normal = glm::vec3(cos(theta)*cos(phi), sin(theta)*cos(phi), sin(phi));
            vertex_position = loop_center + vertex_normal*circle_radius;
            vertex_color = glm::vec3(1.0 - ((float) i / (float) num_loop_samples), 
                                            (float) i / (float) num_loop_samples, 
                                            (float) j / (float) num_circle_samples);
            vertex_coord = glm::vec2(theta / (2.0*glm::pi<GLfloat>()),
                                     phi / (2.0*glm::pi<GLfloat>()));

            // Add vectors to the data buffer
            for (int k = 0; k < 3; k++){
                vertex[(i*num_circle_samples+j)*vertex_att + k] = vertex_position[k];
                vertex[(i*num_circle_samples+j)*vertex_att + k + 3] = vertex_normal[k];
                vertex[(i*num_circle_samples+j)*vertex_att + k + 6] = vertex_color[k];
            }
            vertex[(i*num_circle_samples+j)*vertex_att + 9] = vertex_coord[0];
            vertex[(i*num_circle_samples+j)*vertex_att + 10] = vertex_coord[1];
        }
    }

    // Create triangles
    for (int i = 0; i < num_loop_samples; i++){
        for (int j = 0; j < num_circle_samples; j++){
            // Two triangles per quad
            glm::vec3 t1(((i + 1) % num_loop_samples)*num_circle_samples + j, 
                         i*num_circle_samples + ((j + 1) % num_circle_samples),
                         i*num_circle_samples + j);    
            glm::vec3 t2(((i + 1) % num_loop_samples)*num_circle_samples + j,
                         ((i + 1) % num_loop_samples)*num_circle_samples + ((j + 1) % num_circle_samples),
                         i*num_circle_samples + ((j + 1) % num_circle_samples));
            // Add two triangles to the data buffer
            for (int k = 0; k < 3; k++){
                face[(i*num_circle_samples+j)*face_att*2 + k] = (GLuint) t1[k];
                face[(i*num_circle_samples+j)*face_att*2 + k + face_att] = (GLuint) t2[k];
            }
        }
    }

    // Create OpenGL buffers and copy data
    //GLuint vao;
    //glGenVertexArrays(1, &vao);
    //glBindVertexArray(vao);

    GLuint vbo, ebo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_num * vertex_att * sizeof(GLfloat), vertex, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_num * face_att * sizeof(GLuint), face, GL_STATIC_DRAW);

    // Free data buffers
    delete [] vertex;
    delete [] face;

    // Create resource
    AddResource(Mesh, object_name, vbo, ebo, face_num * face_att);
}



void ResourceManager::CreateCube_noRoof(std::string object_name, float side_length) {
	const int vertex_att = 11;
	const int face_att = 3;

	float half_length = side_length / 2;

	GLfloat vertex[] = {
		//position		normal			RGB				uv
		// last front
		-half_length,half_length,half_length,	0.0,0.0,0.0,	1.0,0.0,0.0,	1.0,0.0,//top left	0
		-half_length,-half_length,half_length,	0.0,0.0,0.0,	1.0,0.0,1.0,	1.0,1.0,//bot left	1
		//front
		-half_length,half_length,half_length,	0.0,0.0,0.0,	1.0,0.0,0.0,	0.0,0.0,//top left	2
		half_length,half_length,half_length,	0.0,0.0,0.0,	0.0,1.0,0.0,	0.25,0.0,//top right	3
		-half_length,-half_length,half_length,	0.0,0.0,0.0,	1.0,0.0,1.0,	0.0,1.0,//bot left	4
		half_length,-half_length,half_length,	0.0,0.0,0.0,	0.0,0.0,0.0,	0.25,1.0,//bot right	5

		//back
		half_length,half_length,-half_length,	0.0,0.0,0.0,	1.0,0.0,0.0,	0.5,0.0,//top left	6
		-half_length,half_length,-half_length,	0.0,0.0,0.0,	0.0,1.0,0.0,	0.75,0.0,//top right	7
		half_length,-half_length,-half_length,	0.0,0.0,0.0,	1.0,0.0,1.0,	0.5,1.0,//bot left	8
		-half_length,-half_length,-half_length,	0.0,0.0,0.0,	0.0,0.0,0.0,	0.75,1.0,//bot right	9
	};

	GLuint face[] = {
		3,2,4,
		4,5,3,
		6,3,5,
		5,8,6,
		7,6,8,
		8,9,7,
		0,7,9,
		9,1,0,

	};

	GLuint vbo, ebo;
	// Create OpenGL buffer for vertices
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 10 * vertex_att * sizeof(GLfloat), vertex, GL_STATIC_DRAW);

	// Create OpenGL buffer for faces
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 8 * face_att * sizeof(GLuint), face, GL_STATIC_DRAW);


	AddResource(Mesh, object_name, vbo, ebo, 8 * face_att);
}

// Create the geometry for a Cylinder
void ResourceManager::CreateCylinder(std::string object_name, float radius, float height) {

	int num_loop_samples = 90;
	int num_circle_samples = 30;

	// Create a torus
	// The torus is built from a large loop with small circles around the loop
	// Vertices are sampled along the circles

	// Number of vertices and faces to be created
	const GLuint vertex_num = num_loop_samples * num_circle_samples;
	const GLuint face_num = num_loop_samples * num_circle_samples * 2 + num_circle_samples * 2 - 4; //last num_circle_sample*2 is for top and bot face

	// Number of attributes for vertices and faces
	const int vertex_att = 11;  // 11 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3), 2D texture coordinates (2)
	const int face_att = 3; // Vertex indices (3)

	// Data buffers for the torus
	GLfloat *vertex = NULL;
	GLuint *face = NULL;

	// Allocate memory for buffers
	try {
		vertex = new GLfloat[vertex_num * vertex_att];
		face = new GLuint[face_num * face_att];
	}
	catch (std::exception &e) {
		throw e;
	}

	// Create vertices 
	float theta, phi; // Angles for circles
	glm::vec3 loop_center;
	glm::vec3 vertex_position;
	glm::vec3 vertex_normal;
	glm::vec3 vertex_color;
	glm::vec2 vertex_coord;

	for (int i = 0; i < num_loop_samples; i++) { // large loop

		theta = height / num_loop_samples; // loop sample (angle theta)
		loop_center = glm::vec3(0, -height / 2 + i * theta, 0); // centre of a small circle

		for (int j = 0; j < num_circle_samples; j++) { // small circle

			phi = 2.0*glm::pi<GLfloat>()*j / num_circle_samples; // circle sample (angle phi)

			// Define position, normal and color of vertex
			vertex_normal = glm::vec3(cos(theta)*cos(phi), sin(theta)*cos(phi), sin(phi));
			vertex_position = loop_center + vertex_normal * radius;
			vertex_color = glm::vec3(139.0f/255.0f, 69.0f/255.0f, 19.0f/255.0f);
			vertex_coord = glm::vec2(theta / 2.0*glm::pi<GLfloat>(),
				phi / 2.0*glm::pi<GLfloat>());

			// Add vectors to the data buffer
			for (int k = 0; k < 3; k++) {
				vertex[(i*num_circle_samples + j)*vertex_att + k] = vertex_position[k];
				vertex[(i*num_circle_samples + j)*vertex_att + k + 3] = vertex_normal[k];
				vertex[(i*num_circle_samples + j)*vertex_att + k + 6] = vertex_color[k];
			}
			vertex[(i*num_circle_samples + j)*vertex_att + 9] = vertex_coord[0];
			vertex[(i*num_circle_samples + j)*vertex_att + 10] = vertex_coord[1];
		}
	}


	// Create triangles
	for (int i = 0; i < num_loop_samples - 1; i++) {
		for (int j = 0; j < num_circle_samples; j++) {
			// Two triangles per quad
			glm::vec3 t1(((i + 1) % num_loop_samples)*num_circle_samples + j,
				i*num_circle_samples + ((j + 1) % num_circle_samples),
				i*num_circle_samples + j);
			glm::vec3 t2(((i + 1) % num_loop_samples)*num_circle_samples + j,
				((i + 1) % num_loop_samples)*num_circle_samples + ((j + 1) % num_circle_samples),
				i*num_circle_samples + ((j + 1) % num_circle_samples));
			// Add two triangles to the data buffer
			for (int k = 0; k < 3; k++) {
				face[(i*num_circle_samples + j)*face_att * 2 + k] = (GLuint)t1[k];
				face[(i*num_circle_samples + j)*face_att * 2 + k + face_att] = (GLuint)t2[k];
			}
		}
	}

	// Create triangles for top and bot
	for (int i = 0; i < num_circle_samples - 2; i += 1) {
		face[num_loop_samples*num_circle_samples * 2 * face_att + i * face_att] = 0;
		face[num_loop_samples*num_circle_samples * 2 * face_att + i * face_att + 1] = i + 1;
		face[num_loop_samples*num_circle_samples * 2 * face_att + i * face_att + 2] = i + 2;
	}

	for (int i = vertex_num - 1; i > vertex_num - 29; i -= 1) {
		face[num_loop_samples*num_circle_samples * 2 * face_att + (num_circle_samples - 2)*face_att + (vertex_num - 1 - i) * face_att] = vertex_num - 1;
		face[num_loop_samples*num_circle_samples * 2 * face_att + (num_circle_samples - 2)*face_att + (vertex_num - 1 - i) * face_att + 1] = i - 1;
		face[num_loop_samples*num_circle_samples * 2 * face_att + (num_circle_samples - 2)*face_att + (vertex_num - 1 - i) * face_att + 2] = i - 2;
	}

	GLuint vbo, ebo;
	// Create OpenGL buffer for vertices
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertex_num * vertex_att * sizeof(GLfloat), vertex, GL_STATIC_DRAW);

	// Create OpenGL buffer for faces
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_num * face_att * sizeof(GLuint), face, GL_STATIC_DRAW);


	// Free data buffers
	delete[] vertex;
	delete[] face;

	AddResource(Mesh, object_name, vbo, ebo, face_num * face_att);
}

void ResourceManager::CreateCube(std::string object_name, float side_length) {
	const int vertex_att = 11;
	const int face_att = 3;

	float half_length = side_length / 2;

	GLfloat vertex[] = {
		//position		normal			RGB				uv
		//front
		-half_length,half_length,half_length,	0.0,0.0,0.0,	1.0,0.0,0.0,	0.0,0.0,//top left
		half_length,half_length,half_length,	0.0,0.0,0.0,	0.0,1.0,0.0,	0.0,0.0,//top right
		-half_length,-half_length,half_length,	0.0,0.0,0.0,	1.0,0.0,1.0,	0.0,0.0,//bot left
		half_length,-half_length,half_length,	0.0,0.0,0.0,	0.0,0.0,0.0,	0.0,0.0,//bot right

		//back
		half_length,half_length,-half_length,	0.0,0.0,0.0,	1.0,0.0,0.0,	0.0,0.0,//top left
		-half_length,half_length,-half_length,	0.0,0.0,0.0,	0.0,1.0,0.0,	0.0,0.0,//top right
		half_length,-half_length,-half_length,	0.0,0.0,0.0,	1.0,0.0,1.0,	0.0,0.0,//bot left
		-half_length,-half_length,-half_length,	0.0,0.0,0.0,	0.0,0.0,0.0,	0.0,0.0,//bot right

		//left
		-half_length,half_length,-half_length,	0.0,0.0,0.0,	1.0,0.0,0.0,	0.0,0.0,//top left
		-half_length,half_length,half_length,	0.0,0.0,0.0,	0.0,1.0,0.0,	0.0,0.0,//top right
		-half_length,-half_length,-half_length,	0.0,0.0,0.0,	1.0,0.0,1.0,	0.0,0.0,//bot left
		-half_length,-half_length,half_length,	0.0,0.0,0.0,	0.0,0.0,0.0,	0.0,0.0,//bot right

		//right
		half_length,half_length,half_length,	0.0,0.0,0.0,	1.0,0.0,0.0,	0.0,0.0,//top left
		half_length,half_length,-half_length,	0.0,0.0,0.0,	0.0,1.0,0.0,	0.0,0.0,//top right
		half_length,-half_length,half_length,	0.0,0.0,0.0,	1.0,0.0,1.0,	0.0,0.0,//bot left
		half_length,-half_length,-half_length,	0.0,0.0,0.0,	0.0,0.0,0.0,	0.0,0.0,//bot right

		//top
		-half_length,half_length,-half_length,	0.0,0.0,0.0,	1.0,0.0,0.0,	0.0,0.0,//top left
		half_length,half_length,-half_length,	0.0,0.0,0.0,	0.0,1.0,0.0,	0.0,0.0,//top right
		-half_length,half_length,half_length,	0.0,0.0,0.0,	1.0,0.0,1.0,	0.0,0.0,//bot left
		half_length,half_length,half_length,	0.0,0.0,0.0,	0.0,0.0,0.0,	0.0,0.0,//bot right

		//bot
		-half_length,-half_length,half_length,	0.0,0.0,0.0,	1.0,0.0,0.0,	0.0,0.0,//top left
		half_length,-half_length,half_length,	0.0,0.0,0.0,	0.0,1.0,0.0,	0.0,0.0,//top right
		-half_length,-half_length,-half_length,	0.0,0.0,0.0,	1.0,0.0,1.0,	0.0,0.0,//bot left
		half_length,-half_length,-half_length,	0.0,0.0,0.0,	0.0,0.0,0.0,	0.0,0.0,//bot right
	};

	GLuint face[] = {
		1,0,2,
		2,3,1,
		5,4,6,
		6,7,5,
		9,8,10,
		10,11,9,
		13,12,14,
		14,15,13,
		17,16,18,
		18,19,17,
		21,20,22,
		22,23,21,
	};

	GLuint vbo, ebo;
	// Create OpenGL buffer for vertices
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * vertex_att * sizeof(GLfloat), vertex, GL_STATIC_DRAW);

	// Create OpenGL buffer for faces
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24 * face_att * sizeof(GLuint), face, GL_STATIC_DRAW);


	AddResource(Mesh, object_name, vbo, ebo, 12 * face_att);
}


void string_trim(std::string str, std::string to_trim) {

	// Trim any character in to_trim from the beginning of the string str
	while ((str.size() > 0) &&
		(to_trim.find(str[0]) != std::string::npos)) {
		str.erase(0);
	}

	// Trim any character in to_trim from the end of the string str
	while ((str.size() > 0) &&
		(to_trim.find(str[str.size() - 1]) != std::string::npos)) {
		str.erase(str.size() - 1);
	}
}

std::vector<std::string> string_split(std::string str, std::string separator) {

	// Initialize output
	std::vector<std::string> output;
	output.push_back(std::string(""));
	int string_index = 0;

	// Analyze string
	unsigned int i = 0;
	while (i < str.size()) {
		// Check if character i is a separator
		if (separator.find(str[i]) != std::string::npos) {
			// Split string
			string_index++;
			output.push_back(std::string(""));
			// Skip separators
			while ((i < str.size()) && (separator.find(str[i]) != std::string::npos)) {
				i++;
			}
		}
		else {
			// Otherwise, copy string
			output[string_index] += str[i];
			i++;
		}
	}

	return output;
}

std::vector<std::string> string_split_once(std::string str, std::string separator) {

	// Initialize output
	std::vector<std::string> output;
	output.push_back(std::string(""));
	int string_index = 0;

	// Analyze string
	unsigned int i = 0;
	while (i < str.size()) {
		// Check if character i is a separator
		if (separator.find(str[i]) != std::string::npos) {
			// Split string
			string_index++;
			output.push_back(std::string(""));
			// Skip single separator
			i++;
		}
		else {
			// Otherwise, copy string
			output[string_index] += str[i];
			i++;
		}
	}

	return output;
}

void print_mesh(TriMesh &mesh) {

	for (unsigned int i = 0; i < mesh.position.size(); i++) {
		std::cout << "v " <<
			mesh.position[i].x << " " <<
			mesh.position[i].y << " " <<
			mesh.position[i].z << std::endl;
	}
	for (unsigned int i = 0; i < mesh.normal.size(); i++) {
		std::cout << "vn " <<
			mesh.normal[i].x << " " <<
			mesh.normal[i].y << " " <<
			mesh.normal[i].z << std::endl;
	}
	for (unsigned int i = 0; i < mesh.tex_coord.size(); i++) {
		std::cout << "vt " <<
			mesh.tex_coord[i].x << " " <<
			mesh.tex_coord[i].y << std::endl;
	}
	for (unsigned int i = 0; i < mesh.face.size(); i++) {
		std::cout << "f " <<
			mesh.face[i].i[0] << " " <<
			mesh.face[i].i[1] << " " <<
			mesh.face[i].i[2] << " " << std::endl;
	}
}


template <typename T> std::string num_to_str(T num) {

	std::ostringstream ss;
	ss << num;
	return ss.str();
}


template <typename T> T str_to_num(const std::string &str) {

	std::istringstream ss(str);
	T result;
	ss >> result;
	if (ss.fail()) {
		throw(std::ios_base::failure(std::string("Invalid number: ") + str));
	}
	return result;
}


} // namespace game;
