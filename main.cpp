// Include standard headers
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
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

// Includes for PNG
#include <png.h>
#include <zlib.h>

using namespace std;

// Define the triangles that make up the 6 faces of a cube
static const GLfloat g_vertex_buffer_data[] = {
	// Face 1
	 1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,

	 // Face 2
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,

	 // Face 3
	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	 // Face 4
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,

	 // Face 5
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,

	 // Face 6
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,

	 1.0f,  1.0f,  1.0f,
	 1.0f,  -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f
};

// Define the UV coordinates for each cube face
// The vertical axis is flipped due to the flipping of the image
static const GLfloat g_uv_buffer_data[] = {
	// Face 1
	0.334f, 1.0f - 0.000f,
	0.000f, 1.0f - 0.334f,
	0.000f, 1.0f - 0.000f,

	0.334f, 1.0f - 0.000f,
	0.334f, 1.0f - 0.334f,
	0.000f, 1.0f - 0.334f,
	
	// Face 2
	0.667f, 1.0f - 0.000f,
	0.334f, 1.0f - 0.334f,
	0.667f, 1.0f - 0.334f,

	0.667f, 1.0f - 0.000f,
	0.334f, 1.0f - 0.000f,
	0.334f, 1.0f - 0.334f,

	// Face 3
	0.667f, 1.0f - 0.334f,
	1.000f, 1.0f - 0.334f,
	1.000f, 1.0f - 0.000f,

	0.667f, 1.0f - 0.334f,
	1.000f, 1.0f - 0.000f,
	0.667f, 1.0f - 0.000f,

	// Face 4
	0.000f, 1.0f - 0.334f,
	0.000f, 1.0f - 0.667f,
	0.334f, 1.0f - 0.667f,
	
	0.000f, 1.0f - 0.334f,
	0.334f, 1.0f - 0.667f,
	0.334f, 1.0f - 0.334f,
	
	// Face 5
	0.667f, 1.0f - 0.667f,
	0.667f, 1.0f - 0.334f,
	0.334f, 1.0f - 0.334f,

	0.667f, 1.0f - 0.667f,
	0.334f, 1.0f - 0.334f,
	0.334f, 1.0f - 0.667f,

	// Face 6
	0.667f, 1.0f - 0.334f,
	1.000f, 1.0f - 0.667f,
	1.000f, 1.0f - 0.334f,

	0.667f, 1.0f - 0.334f,
	0.667f, 1.0f - 0.667f,
	1.000f, 1.0f - 0.667f
};

GLuint load_png(const char *imagepath)
{
	const int header_size = 8;
	unsigned char header[header_size];

	// Open the file and check it has a PNG signature
	FILE *file = fopen(imagepath, "rb");
	if (!file)
	{
		cerr << "Image could not be opened: " << imagepath << endl;
		return 0;
	}

	if (fread(header, 1, header_size, file) != header_size)
	{
		cerr << "Failed to read PNG header bytes\n";
		return 0;
	}
	
	if (png_sig_cmp(header, 0, header_size))
	{
		cerr << "File is not a valid PNG: " << imagepath << endl;
		return 0;
	}

	// Create data structures for reading
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr)
	{
		cerr << "Failed to create libPNG header struct\n";
		return 0;
	}

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
		png_destroy_read_struct(&png_ptr, nullptr, nullptr);
		cerr << "Failed to create libPNG info struct\n";
		return 0;
    }

	png_init_io(png_ptr, file);
	png_set_sig_bytes(png_ptr, header_size);

	// Read PNG info
	png_read_info(png_ptr, info_ptr);

	int width      = png_get_image_width(png_ptr, info_ptr);
	int height     = png_get_image_height(png_ptr, info_ptr);
	auto color_type = png_get_color_type(png_ptr, info_ptr);
	auto bit_depth  = png_get_bit_depth(png_ptr, info_ptr);	

	cout << "PNG texture to be loaded: " << imagepath << endl;
	cout << "Width: " << width << endl;
	cout << "Height: " << height << endl;
	cout << "Color type: " << static_cast<int>(color_type) << endl;
	cout << "Bit depth: " << static_cast<int>(bit_depth) << endl;

	// Convert any color type to 8-bit RGBA
	if (bit_depth == 16)
	{
		png_set_strip_16(png_ptr);
	}

	if (color_type == PNG_COLOR_TYPE_PALETTE)
	{
		png_set_palette_to_rgb(png_ptr);
	}

	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
	{
		png_set_expand_gray_1_2_4_to_8(png_ptr);
	}

	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
	{
		png_set_tRNS_to_alpha(png_ptr);
	}

	if (color_type == PNG_COLOR_TYPE_RGB ||
		color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_PALETTE)
	{
		png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);
	}

	if (color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
	{
		png_set_gray_to_rgb(png_ptr);
	}

	png_read_update_info(png_ptr, info_ptr);

	// Now read data
	size_t row_size = png_get_rowbytes(png_ptr, info_ptr);
	png_byte *data = new png_byte[row_size * height];
	vector<png_bytep> row_pointers(height);
	for (int i=0; i<height; i++)
	{
		// Need to flip date over vertically as glTexImage2D expected data origin
		// to be from the bottom left
		row_pointers[height - i - 1] = &data[i * row_size];
	}

	png_read_image(png_ptr, row_pointers.data());

	// Now create GLES texture
	GLuint texture_id;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	// Set-up filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Clean up
	delete [] data;
	png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
	fclose(file);

	return texture_id;
}

GLuint load_shaders(const char * vertex_file_path,const char * fragment_file_path)
{

	// Create the shaders
	GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	string vertex_shader_code;
	ifstream vertex_shader_stream(vertex_file_path, ios::in);
	if(vertex_shader_stream.is_open())
	{
		string Line = "";
		while(getline(vertex_shader_stream, Line))
		{
			vertex_shader_code += "\n" + Line;
		}
		vertex_shader_stream.close();
	}
	else
	{
		cerr << "Impossible to open " << vertex_file_path << ". Are you in the right directory? Don't forget to read the FAQ!\n";
		return 0;
	}

	// Read the Fragment Shader code from the file
	string fragment_shader_code;
	ifstream fragment_shader_stream(fragment_file_path, ios::in);
	if(fragment_shader_stream.is_open())
	{
		string Line = "";
		while(getline(fragment_shader_stream, Line))
		{
			fragment_shader_code += "\n" + Line;
		}
		fragment_shader_stream.close();
	}

	GLint result = GL_FALSE;
	int info_log_length;

	// Compile Vertex Shader
	cout << "Compiling shader: " << vertex_file_path << endl;
	char const * vertex_source_pointer = vertex_shader_code.c_str();
	glShaderSource(vertex_shader_id, 1, &vertex_source_pointer , NULL);
	glCompileShader(vertex_shader_id);

	// Check Vertex Shader
	glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
	if ( info_log_length > 0 )
	{
		vector<char> vertex_shader_error_message(info_log_length+1);
		glGetShaderInfoLog(vertex_shader_id, info_log_length, NULL, &vertex_shader_error_message[0]);
		cerr << &vertex_shader_error_message[0] << endl;
	}

	// Compile Fragment Shader
	cout << "Compiling shader: " << fragment_file_path << endl;
	char const * fragment_source_pointer = fragment_shader_code.c_str();
	glShaderSource(fragment_shader_id, 1, &fragment_source_pointer , NULL);
	glCompileShader(fragment_shader_id);

	// Check Fragment Shader
	glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
	if ( info_log_length > 0 )
	{
		vector<char> fragment_shader_error_message(info_log_length+1);
		glGetShaderInfoLog(fragment_shader_id, info_log_length, NULL, &fragment_shader_error_message[0]);
		cerr << &fragment_shader_error_message[0] << endl;
	}

	// Link the program
	cout << "Linking program\n";
	GLuint program_id = glCreateProgram();
	glAttachShader(program_id, vertex_shader_id);
	glAttachShader(program_id, fragment_shader_id);
	glLinkProgram(program_id);

	// Check the program
	glGetProgramiv(program_id, GL_LINK_STATUS, &result);
	glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
	if ( info_log_length > 0 )
	{
		vector<char> ProgramErrorMessage(info_log_length+1);
		glGetProgramInfoLog(program_id, info_log_length, NULL, &ProgramErrorMessage[0]);
		cerr << &ProgramErrorMessage[0] << endl;
	}
	
	glDetachShader(program_id, vertex_shader_id);
	glDetachShader(program_id, fragment_shader_id);
	
	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);

	return program_id;
}

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

	GLuint vertex_array_id;
	glGenVertexArrays(1, &vertex_array_id);
	glBindVertexArray(vertex_array_id);

	// Create the vertex buffer
	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// Create the UV buffer
	GLuint uv_buffer;
	glGenBuffers(1, &uv_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);

	// Load texture
	GLuint cube_texture = load_png("res/texture.png");

	// Create and compile our GLSL program from the shaders
	GLuint program_id = load_shaders( "vertex_shader.glsl", "fragment_shader.glsl" );

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	float angle = 0.0;

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
			glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
			);
  
		// Model matrix : an identity matrix (model will be at the origin)
		glm::mat4 model = glm::mat4(1.0f);

		// Update model to create a rotation
		model = glm::rotate(model, angle, glm::vec3(0.125, 1.0, 0.25));
	
		// Our ModelViewProjection : multiplication of our 3 matrices
		glm::mat4 mvp = projection * view * model;

		angle += 0.01f;
		if (angle >= 360.0f)
		{
			angle = 0.0f;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(program_id);

		// Get a handle for our "MVP" uniform
		// Only during the initialisation
		GLuint matrix_id = glGetUniformLocation(program_id, "MVP");
  
		// Send our transformation to the currently bound shader, in the "MVP" uniform
		// This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
		glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &mvp[0][0]);

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
	}
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 );

	return 0;
}
