run_gl3_example: main.cpp
	g++ -std=c++11 main.cpp -o run_gl3_example -framework OpenGL -lGLEW -lglfw -lpng
