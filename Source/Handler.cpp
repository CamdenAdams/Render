//#define GLFW_INCLUDE_NONE
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <chrono>

#define SHADERLOG(shader, ivENUM) \
{ \
	int status; \
	glGetShaderiv(shader, ivENUM, &status); \
	char log[512]; \
	glGetShaderInfoLog(shader, 512, NULL, log); \
	std::cout << log << std::endl; \
	if (status != GL_TRUE) \
	{ \
		std::cout << "Shader compilation failed" << std::endl; \
		return -4; \
	} \
}


const char* vertexShaderSource = R"glsl(
	#version 150 core
		
	in vec2 position;	
	in vec3 color;

	out vec3 Color;
		
	void main()
	{
		Color = color;
		gl_Position = vec4(position, 0.0, 1.0);
	}
)glsl";

const char* fragmentShaderSource = R"glsl(
	#version 150 core

	// uniform vec3 triangleColor;
	in vec3 Color;		

	out vec4 outColor;
		
	void main()
	{
		outColor = vec4(Color, 1.0);
	}
)glsl";


int main()
{
	GLFWwindow* window;

	if (!glfwInit())
		return -1;

	window = glfwCreateWindow(1280, 960, "Window", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -2;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
		return -3;

	//float vertices[] = {
	//	 0.0f,  0.5f, 0.0f, // Top
	//	 0.5f, -0.5f, 0.0f, // Right
	//	-0.5f, -0.5f, 0.0f // Left
	//};

	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//float vertices[] = {
	//	 0.0f,  0.5f, // Top
	//	 0.5f, -0.5f, // Right
	//	-0.5f, -0.5f  // Left
	//};

	//float vertices[] = {
	//	 0.0f,  0.5f, 1.0f, 0.0f, 0.0f, // Top - Red
	//	 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // Right - Green
	//	-0.5f, -0.5f, 0.0f, 0.0f, 1.0f  // Left - Blue
	//};

	float vertices[] = {
		-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // Top Left - Red
		 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // Top Right - Green
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom Right - Blue
		-0.5f, -0.5f, 1.0f, 1.0f, 1.0f  // Bottom Left - Yellow
	};

	unsigned int elements[] = {
		0, 1, 2,
		2, 3, 0
	};

	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	unsigned int ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	SHADERLOG(vertexShader, GL_COMPILE_STATUS);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	SHADERLOG(fragmentShader, GL_COMPILE_STATUS);

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	int linkStatus;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
	char linkLog[512];
	glGetProgramInfoLog(shaderProgram, 512, NULL, linkLog);
	std::cout << linkLog << std::endl;
	if (linkStatus != GL_TRUE)
	{
		std::cout << "Shader program linking failed" << std::endl;
		return -5;
	}
	glUseProgram(shaderProgram);

	// This variable corresponds with the gl_position variable in the vertex shader
	int positionAttribute = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(positionAttribute);
	glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	int colorAttribute = glGetAttribLocation(shaderProgram, "color");
	glEnableVertexAttribArray(colorAttribute);
	glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));

	int triangleColorLocation = glGetUniformLocation(shaderProgram, "triangleColor");
	auto tStart = std::chrono::high_resolution_clock::now();

	while (!glfwWindowShouldClose(window))
	{
		auto tNow = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration_cast<std::chrono::duration<float>>(tNow - tStart).count();

		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glBindVertexArray(vao);
		//glUniform3f(triangleColorLocation, (sin(time * 4.0f) + 1.0f) / 2.0f, 0.0f, 0.0f);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glfwSwapBuffers(window);
		glfwPollEvents();

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	// all remaining clean up is done here
	glDeleteProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

	glfwTerminate();

	return 0;
}