#version 130

// Attributes passed from the vertex shader
in vec3 position_interp;
in vec3 normal_interp;
in vec3 light_pos;

// Material attributes (constants)
// Exponent for specular term
float phong_exponent = 128.0;
// Ambient light amount
float Ia = 0.1; 
// Albedo of the material
vec3 ambient_albedo = vec3(0.3, 0.3, 0.3);
vec3 diffuse_albedo = vec3(0.0, 0.0, 0.5);
vec3 specular_albedo = vec3(0.0, 0.0, 0.0);

// Uniforms
// Camera position in world coordinates
uniform vec3 camera_pos;
// Uniform (global) buffer with the environment map
uniform samplerCube env_map;


void main() 
{
    // Three-term illumination model
    vec3 N, // Interpolated normal for fragment
         L, // Light-source direction
         V, // View direction
         H; // Half-way vector

    // Compute Lambertian term Id
    N = normalize(normal_interp);

    L = (light_pos - position_interp);
    L = normalize(L);

    float Id = max(dot(N, L), 0.0);
    
    // Compute specular term Is for Blinn-Phong shading
    V = camera_pos - position_interp; // Now we need the camera position as we are working in world coordinates
    V = normalize(V);

    //H = 0.5*(V + L); // Halfway vector
    H = (V + L); // Halfway vector (will be normalized anyway)
    H = normalize(H);

    float spec_angle_cos = max(dot(N, H), 0.0);
    float Is = pow(spec_angle_cos, phong_exponent);

    // Full illumination for the fragment
    vec3 illum = Ia*ambient_albedo + Id*diffuse_albedo + Is*specular_albedo;

    // Compute indirect lighting with environment map
    // Reflection vector
    //vec3 Lr = 2.0 * dot(N, V) * N - V;

    // When using GLSL's reflect, we input the vector from eye to point
    vec3 Valt = position_interp - camera_pos;
    vec3 Lr = reflect(Valt, N);

    // Query environment map
    vec4 il = texture(env_map, Lr);

    // Add pixel value to the illumination
    // Modulate influence of environment light by some constant
    illum += 0.7*il.xyz;
 
    // Assign illumination to the fragment
    gl_FragColor = vec4(illum, 1.0);
}
