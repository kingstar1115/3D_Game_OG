#version 130

// Vertex buffer
in vec3 vertex;
in vec3 normal;
in vec3 color;
in vec2 uv;

// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;
uniform mat4 normal_mat;

// Attributes forwarded to the fragment shader
out vec3 uvw_interp;


void main()
{
    // Transform the vertex
    vec4 pos = projection_mat * view_mat * world_mat * vec4(vertex, 1.0);

    // Make the mesh always fail the z-buffer test,
    // so that it is always the most distant mesh in the camera view
    pos.z = pos.w; 

    // Pull the mesh slightly into the view volume so that it is visible
    pos.z -= 0.0001; 

    // Assign new position with depth information
    gl_Position = pos;
 
    // Assign texture coordinates
    uvw_interp = vertex;
}
