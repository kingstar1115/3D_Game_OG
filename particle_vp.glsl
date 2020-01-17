 #version 400

// Vertex buffer
in vec3 vertex;
in vec3 normal;
in vec3 color;

// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 normal_mat;
uniform float timer;
uniform int flag;

// Attributes forwarded to the geometry shader
out vec3 vertex_color;
out float timestep;

// Simulation parameters (constants)
uniform vec3 up_vec = vec3(0.0, 1.0, 0.0);
uniform vec3 object_color = vec3(0.886, 0.325, 0.06);
float grav = 0.005; // Gravity
float speed = 2.5; // Allows to control the speed of the explosion


void main()
{
    // Let time cycle every four seconds
    float circtime = mod(timer+color.x*4,2.0);
    float t = circtime ; // Our time parameter
    
    // Let's first work in model space (apply only world matrix)
    vec4 position = world_mat * vec4(vertex, 1.0);
    vec4 norm = normal_mat * vec4(normal, 1.0);

	vertex_color = vec3(0.8f,0.8f,0.8f); // Uniform color 

	mat4 scale = mat4(0.1);

    // Move point along normal and down with t*t (acceleration under gravity)
	if(flag == 1){
		circtime = mod(timer+color.x*1,1.0);
   		t = circtime ; // Our time parameter
		position.x += 1*(norm.x*t*speed*0.1);
		position.y += norm.y > 0? 0.2*(-norm.y*t*speed) : 0.2*(norm.y*t*speed);
		position.z += 1*(norm.z*t*speed*0.1);
		scale = mat4(0.2);
	}
	else if(flag == 2){
		circtime = mod(timer+color.x*4,4.0);
    	t = circtime ; // Our time parameter
		position.x += position.y*cos(3.1415926*t)/2*t;
		position.y +=norm.y > 0? 4*(norm.y*t*speed - grav*speed*up_vec.y*t*t) : 4*(-norm.y*t*speed - grav*speed*up_vec.y*t*t);;
		position.z += position.y*sin(3.1415926*t)/2*t;
		vertex_color.r = 0.125-t/8;
		vertex_color.g = 0.35-t/8;
		vertex_color.b = 0.94-t/8;
	}else{
		circtime = mod(timer+color.x*1,1.0);
   		t = circtime ; // Our time parameter
		position.x += 1.5*(norm.x*t*speed);
		position.y += 1.5*(norm.y*t*speed);
		position.z += 1.5*(norm.z*t*speed);
		scale = mat4(0.2);

		    // Define color of vertex
		vertex_color.r = object_color.r-t;
		vertex_color.g = object_color.g-t;
		vertex_color.b = object_color.b-t;
	}
    
   

    // Now apply view transformation
    gl_Position = scale* view_mat * position;
        
    // Forward time step to geometry shader
    timestep = t;
}
