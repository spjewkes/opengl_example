// Include standard headers
#include <iostream>
#include <string>
#include <chrono>

// Include GLEW. Always include it before gl.h and glfw.h, since it's a bit magic.
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "utility.hpp"
#include "wavefront_obj.hpp"

using namespace std;

int main()
{
	int width = 1024;
	int height = 768;

	// Initialise GLFW
	if( !glfwInit() )
	{
		cerr << "Failed to initialize GLFW\n";
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Set up for OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

	// Open a window and create its OpenGL context
	GLFWwindow* window;
	window = glfwCreateWindow( width, height, "OpenGL example", NULL, NULL);
	if( window == NULL )
	{
		cerr << "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window); // Initialize GLEW
	glewExperimental=true; // Needed in core profile
	if (glewInit() != GLEW_OK)
	{
		cerr << "Failed to initialize GLEW\n";
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Create the buffers
	GLuint vertex_array_id;
	glGenVertexArrays(1, &vertex_array_id);
	glBindVertexArray(vertex_array_id);

	WavefrontObj object("res/cube.obj");
	object.dump();

	GLuint vertex_buffer = object.create_vertex_buffer();
	GLuint uv_buffer = object.create_tex_coord_buffer();
	GLuint normal_buffer = object.create_normal_buffer();

	// Load texture
	GLuint cube_texture = load_png("res/texture.png");

	// Create and compile our GLSL program from the shaders
	GLuint program_id = load_shaders( "src/vertex_shader.glsl", "src/fragment_shader.glsl" );
	if (!program_id)
	{
		cerr << "Error detected when loading shaders. Aborting.\n";
		abort();
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	double xpos = 0;
	double ypos = 0;
	float x_angle = 0.0;
	float y_angle = 0.0;

	auto tp1 = chrono::system_clock::now();
	auto tp2 = chrono::system_clock::now();

	do
	{
		// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) width / (float)height, 0.1f, 100.0f);
  
		// Or, for an ortho camera :
		// glm::mat4 projection = glm::ortho(-2.0f,2.0f,-2.0f,2.0f,0.0f,100.0f); // In world coordinates
  
		// Camera matrix
		glm::mat4 view = glm::lookAt(
			glm::vec3(3,2,3), // The position of the camera
			glm::vec3(0,0,0), // and looks at the origin
			glm::vec3(0,-1,0)  // Head is up (set to 0,-1,0 to look upside-down)
			);
  
		// Model matrix : an identity matrix (model will be at the origin)
		glm::mat4 model = glm::mat4(1.0f);

		// Update model to create a rotation
		model = glm::rotate(model, x_angle, glm::vec3(0.0, 1.0, 0.0)) * glm::rotate(model, y_angle, glm::vec3(1.0, 0.0, 0.0));
	
		// our ModelViewProjection : multiplication of our 3 matrices
		glm::mat4 mvp = projection * view * model;

		glClearColor(0.0f, 0.0f, 0.05f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(program_id);

		// Set up uniforms
		GLuint mvp_id = glGetUniformLocation(program_id, "MVP");
		glUniformMatrix4fv(mvp_id, 1, GL_FALSE, &mvp[0][0]);

		GLuint m_id = glGetUniformLocation(program_id, "M");
		glUniformMatrix4fv(m_id, 1, GL_FALSE, &model[0][0]);

		GLuint v_id = glGetUniformLocation(program_id, "V");
		glUniformMatrix4fv(v_id, 1, GL_FALSE, &view[0][0]);

		// Texture uniform
		GLuint tex_id = glGetUniformLocation(program_id, "Tex_Cube");
		glUniform1i(tex_id, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cube_texture);

		// First attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

		// Second attribute buffer: texture coords
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
		glVertexAttribPointer(
			1,
			2,
			GL_FLOAT,
			GL_TRUE,
			0,
			(void*)0
			);

		// Third attribute buffer: normals
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
		glVertexAttribPointer(
			2,
			3,
			GL_FLOAT,
			GL_TRUE,
			0,
			(void*)0
			);

		// Draw the array
		glDrawArrays(GL_TRIANGLES, 0, 12*3); // Starting from vertex 0; 3 vertices total -> 1 triangle
		glDisableVertexAttribArray(0);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		// Get time taken to draw the frame
		tp2 = chrono::system_clock::now();
		chrono::duration<float> elapsed_time = tp2 - tp1;
		tp1 = tp2;
		
		char title[256];
		snprintf(title, 256, "OpenGL example - %3.f fps", 1.0 / elapsed_time.count());
		glfwSetWindowTitle(window, title);

		// Move object based on mouse position relative to center
		glfwGetCursorPos(window, &xpos, &ypos);
		x_angle = 0.005f * static_cast<float>(width / 2 - xpos);
		y_angle = 0.005f * static_cast<float>(height / 2 - ypos);
	}
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 );

	return 0;
}
