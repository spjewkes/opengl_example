#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 normal;

out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D Tex_Cube;

void main()
{
	vec3 ambient = vec3(0.05, 0.05, 0.05);
	vec3 light = vec3(3, 2, 3);

	float intensity = clamp(dot(normalize(light), normalize(normal)), 0, 1);
	color = ambient + (texture( Tex_Cube, UV ).rgb * intensity);
}
