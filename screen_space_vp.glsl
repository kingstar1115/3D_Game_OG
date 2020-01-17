#version 130

in vec3 position;
in vec2 uv;

out vec2 uv0;

void main()
{
    gl_Position = vec4(position, 1.0);

    uv0 = uv;
}
