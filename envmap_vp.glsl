#version 130

// Vertex buffer
in vec3 vertex;
in vec3 normal;
in vec3 color;

// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;
uniform mat4 normal_mat;

// Attributes forwarded to the fragment shader
out vec3 position_interp;
out vec3 normal_interp;
out vec3 light_pos;

// Material attributes (constants)
vec3 light_position = vec3(-0.5, -0.5, 1.5);


void main()
{
    gl_Position = projection_mat * view_mat * world_mat * vec4(vertex, 1.0);

    // In this demo, we work in world coordinates, rather than view
    // coordinates

    position_interp = vec3(world_mat * vec4(vertex, 1.0));

    normal_interp = vec3(normal_mat * vec4(normal, 0.0));

    light_pos = light_position;
}
