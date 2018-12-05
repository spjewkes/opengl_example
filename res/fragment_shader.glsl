#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 normal;
in vec3 vertex;

out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D Tex_Cube;

void main()
{
	vec3 ambient = vec3(0.05, 0.05, 0.05) * texture( Tex_Cube, UV ).rgb;

	vec3 light = vec3(3, 2, 3);
	vec3 to_light = normalize(light - vertex);

	float cos_angle = clamp(dot(normalize(normal), to_light), 0.0, 1.0);

	vec3 diffuse = texture( Tex_Cube, UV ).rgb * cos_angle;

	vec3 reflection = normalize(2.0 * dot(normalize(normal), to_light) * normalize(normal) - to_light);
	vec3 to_camera = normalize(-1.0 * vertex);

	float shininess = 12.0;
	cos_angle = clamp(dot(reflection, to_camera), 0.0, 1.0);
	cos_angle = pow(cos_angle, shininess);

	vec3 specular = vec3(0, 0, 0);

	if (cos_angle > 0.0)
	{
		vec3 light_color = vec3(1, 1, 1);
		specular = light_color * cos_angle;
		diffuse = diffuse * (1.0 - cos_angle);
	}

	color = ambient + diffuse + specular;
}
