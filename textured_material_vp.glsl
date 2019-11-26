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
uniform float eye_x;
uniform float eye_y;
uniform float eye_z;

uniform int Flag;

// Attributes forwarded to the fragment shader
out vec3 position_interp;
out vec3 normal_interp;
out vec4 color_interp;
out vec2 uv_interp;
out vec3 light_pos;

out vec3 eye_position;

// Material attributes (constants)
uniform vec3 light_position = vec3(-0.5, -0.5, 1.5);

varying float lightIntensity;

void main()
{
	eye_position = vec3(eye_x,eye_y,eye_z);


	const float bias = 0.6;

	vec3 silhouettePosition = vertex + normalize(normal) * bias;

	vec3 Pos = normalize(eye_position - position_interp);
	vec3 Nor = normalize(normal_interp);

	lightIntensity = dot( Pos, Nor );


	if (Flag == 0){
		if ( lightIntensity >= 0.0 ) {
			gl_Position = projection_mat * view_mat * world_mat * vec4(vertex, 1.0);
		}

		else {
			gl_Position = projection_mat * view_mat * world_mat * vec4(silhouettePosition, 1.0);
		}
	}
    else{
		gl_Position = projection_mat * view_mat * world_mat * vec4(vertex, 1.0);
	}

    position_interp = vec3(view_mat * world_mat * vec4(vertex, 1.0));
    
    normal_interp = vec3(normal_mat * vec4(normal, 0.0));

    color_interp = vec4(color, 1.0);

    uv_interp = uv;

    light_pos = vec3(view_mat * vec4(light_position, 1.0));
}


