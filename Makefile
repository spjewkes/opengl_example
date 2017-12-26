run_gl3_example: main.cpp utility.cpp utility.hpp
	g++ -std=c++11 main.cpp utility.cpp -o run_gl3_example -framework OpenGL -lGLEW -lglfw -lpng
