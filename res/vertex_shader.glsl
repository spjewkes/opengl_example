#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
  
// The texture coordinates
layout(location = 1) in vec2 vertexUV;

// The normal coordinates
layout(location = 2) in vec3 vertexNormal;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;	
uniform mat4 M;	
uniform mat4 V;	

// Output tex coords
out vec2 UV;

// Output normal
out vec3 normal;

// Output vertex
out vec3 vertex;

void main()
{
	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);

	// UV of vertex
	UV = vertexUV;

	// Normal
	normal = (V * M * vec4(vertexNormal,0)).xyz;

	// Vertex
	vertex = (V * M * vec4(vertexPosition_modelspace,1)).xyz;
}
