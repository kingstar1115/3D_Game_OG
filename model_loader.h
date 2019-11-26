#ifndef MODEL_LOADER_H_
#define MODEL_LOADER_H_

#include <exception>
#include <string>
#include <fstream>
#include <sstream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace game {

// Auxiliary definitions and functions for model loading

// A triangular face
struct Face {
    int i[3];
    int n[3];
    int t[3];
};

// A quad face
struct Quad {
    int i[4];
    int n[4];
    int t[4];
};

// A mesh stored in memory
struct TriMesh {
    std::vector<glm::vec3> position;
    std::vector<glm::vec3> normal;
    std::vector<glm::vec2> tex_coord;
    std::vector<Face> face;
};

// Helper functions 
// Trim any character in to_trim from the beginning and end of str
void string_trim(std::string str, std::string to_trim);
// Split string into substrings according to characters in separator
std::vector<std::string> string_split(std::string str, std::string separator);
// Split string into substrings according to characters in separator. A
// split is performed when one separator character is found, rather than
// a sequence of separators
std::vector<std::string> string_split_once(std::string str, std::string separator);
// Print a mesh stored internally
void print_mesh(TriMesh &mesh);
// Conversion between strings and numbers
template <typename T> std::string num_to_str(T num);
template <typename T> T str_to_num(const std::string &str);

} // namespace game;

#endif // MODEL_LOADER_H_
