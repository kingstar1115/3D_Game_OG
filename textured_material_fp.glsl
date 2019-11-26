#version 130

// Attributes passed from the vertex shader
in vec3 position_interp;
in vec3 normal_interp;
in vec4 color_interp;
in vec2 uv_interp;
in vec3 light_pos;
in vec3 eye_position;

// Uniform (global) buffer
uniform int Flag;
uniform sampler2D texture_map;

// Material attributes (constants)
vec4 ambient_color = vec4(0.0, 0.0, 1.0, 1.0);
vec4 diffuse_color = vec4(0.0, 0.0, 0.7, 1.0);
vec4 specular_color = vec4(0.8, 0.5, 0.9, 1.0);
float phong_exponent = 64.0;
float Ia = 0.2; // Ambient light amount
vec3 Dir_light = vec3(0, -20, 0);

// Toon shadering
varying float lightIntensity;

void main() 
{
    // Retrieve texture value
    vec4 pixel = texture(texture_map, uv_interp);

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
    V = eye_position - position_interp;
    //V = - position_interp; // Eye position is (0, 0, 0) in view coordinates
    V = normalize(V);

    //H = 0.5*(V + L); // Halfway vector
    H = (V + L); // Halfway vector (will be normalized anyway)
    H = normalize(H);

	// high light
	vec3 R = -L + 2*dot(L,N)*N;
    float spec_angle_cos = max(dot(V, R), 0.0);
    float Is = pow(spec_angle_cos, phong_exponent);
        
	// Directional light ld
	vec3 Diectional = normalize(-Dir_light);
	float ld_direct = max(dot(N, Diectional), 0.0);

	// Super shining light
	vec4 color = vec4(1.0,0.9,0.7,0.3);

	
	
	// apply toon shader
	if (Flag == 0){
		if(dot(V,N)<0.0){
			// set black
			gl_FragColor = vec4(vec3(0.0,0.0,0.0),1);
		}
		else{
			gl_FragColor = vec4(vec3(1.0,1.0,1.0),1) + (Ia*ambient_color + ld_direct*color + Is*specular_color);			
		}
	}

	// or not
	else{
		gl_FragColor = pixel + (Ia*ambient_color + ld_direct*color + Is*specular_color);
	}




    
}
