// Material with no illumination simulation

#version 130

// Attributes passed from the vertex shader
in vec4 color_interp;


void main() 
{
	gl_FragColor = color_interp;
	//gl_FragColor = vec4(0.6, 0.6, 0.6, 1.0);
}
