#version 430 core

uniform samplerCube cube_texture;

in vec3 texCoord;

out vec4 out_color;

void main()
{
	out_color = texture(cube_texture,texCoord);
}