#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 normal;

out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D Tex_Cube;

void main()
{
	vec3 light = vec3(3, 2, 3);
	color = texture( Tex_Cube, UV ).rgb * dot(normalize(light), normalize(normal));
}
