//#define GLFW_INCLUDE_NONE
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <chrono>


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

	float vertices[] = {
		 0.0f,  0.5f, 1.0f, 0.0f, 0.0f, // Top - Red
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // Right - Green
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f  // Left - Blue
	};

	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	int vStatus;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vStatus);
	char vShaderLog[512];
	glGetShaderInfoLog(vertexShader, 512, NULL, vShaderLog);
	std::cout << vShaderLog << std::endl;
	if (vStatus != GL_TRUE)
	{
		std::cout << "Vertex shader compilation failed" << std::endl;
		return -4;
	}

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	int fStatus;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fStatus);
	char fShaderLog[512];
	glGetShaderInfoLog(fragmentShader, 512, NULL, fShaderLog);
	std::cout << fShaderLog << std::endl;
	if (fStatus != GL_TRUE)
	{
		std::cout << "Fragment shader compilation failed" << std::endl;
		return -5;
	}

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
		return -6;
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
		glDrawArrays(GL_TRIANGLES, 0, 3);
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


#if 0

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

	float vertices[] = {
		-0.5f,  0.5f, 0.0f, // Top Left
		 0.5f,  0.5f, 0.0f, // Top Right
		 0.5f, -0.5f, 0.0f, // Bottom Right
		-0.5f, -0.5f, 0.0f  // Bottom Left
	};

	float indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	unsigned int vbo;
	glGenBuffers(1, &vbo); // Generate 1 buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // Bind the buffer to the array buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Copy the vertices to the buffer

	unsigned int ebo;
	glGenBuffers(1, &ebo); // Generate 1 buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // Bind the buffer to the element array buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // Copy the indices to the buffer

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // Set the vertex attribute pointer
	glEnableVertexAttribArray(0); // Enable the vertex attribute pointer

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind the buffer
	glBindVertexArray(0); // Unbind the vertex array

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glClearColor(0.0f, 168.0f, 150.0f, 1.0f);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	glfwTerminate();

	return 0;
}

#endif