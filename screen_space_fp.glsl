#version 130

// Passed from the vertex shader
in vec2 uv0;

// Passed from outside
uniform float timer;
uniform sampler2D texture_map;
uniform int type;

void main() 
{
    // wavering
    vec2 pos = uv0;
	vec4 pixel;
	if (type == 1){
		pixel = texture(texture_map, pos);
		if (pos.x<=mod(timer,2.0)){
			// wipe
			pixel = vec4(0,0,1,1);

			// cover
			//pixel += vec4(0,0,1,0.5);
		}
	}else if(type == 2){
		pixel = texture(texture_map, pos*2);
	}else if(type == 3){
		if(length(vec2(0.5)-pos)<=mod(timer,4.0)/4 && length(vec2(0.5)-pos)>=mod(timer-0.1,4.0)/4){
			pos -= 0.1*(pos-vec2(0.5));
		}
		pixel = texture(texture_map, pos);
	}else if(type == 4){
		vec3 blur = vec3(0);

		// clarity degree
		float cd = 100;

		// left-top coord of 25 pixels
		float x = pos.x-mod(pos.x,1/cd)-mod(pos.x-mod(pos.x,1/cd),5/cd);
		float y = pos.y-mod(pos.y,1/cd)-mod(pos.y-mod(pos.y,1/cd),5/cd);
		for(int i = 0 ; i < 5 ; i++){
			for(int j = 0 ; j < 5 ; j++){
				blur += texture(texture_map, vec2(x+i/cd,y+j/cd)).xyz;
			}
		}

		pixel = vec4(blur/25,1);
	}else{
		pixel = texture(texture_map, pos);
	}
    
    gl_FragColor = pixel;
}
