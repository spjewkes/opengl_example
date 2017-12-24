#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;

out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D Tex_Cube;

void main()
{
	color = texture( Tex_Cube, UV ).rgb;
}
