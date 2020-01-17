#version 130

// Attributes passed from the vertex shader
in vec3 uvw_interp;
uniform samplerCube texture_map;
 
void main()
{    
    gl_FragColor = texture(texture_map, uvw_interp);
}
