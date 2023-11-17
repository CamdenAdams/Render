#define GLFW_INCLUDE_NONE
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>

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
	in vec2 texcoord;

	out vec3 Color;
	out vec2 Texcoord;
		
	void main()
	{
		gl_Position = vec4(position, 0.0, 1.0);
		Color = color;
		Texcoord = texcoord;
	}
)glsl";

const char* fragmentShaderSource = R"glsl(
	#version 150 core

	// uniform vec3 triangleColor;
	in vec3 Color;		
	in vec2 Texcoord;

	out vec4 outColor;
	
	uniform sampler2D tex;
		
	void main()
	{
		outColor = texture(tex, Texcoord) * vec4(Color, 1.0);
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

	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	float vertices[] = {
	//	Position	  Color			    TexCoords
		-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top Left - Red
		 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top Right - Green
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom Right - Blue
		-0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  // Bottom Left - Yellow
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

	unsigned int tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	int width, height;
	unsigned char* image = SOIL_load_image("Source/sample.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// This variable corresponds with the gl_position variable in the vertex shader
	int positionAttribute = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(positionAttribute);
	glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0);
	int colorAttribute = glGetAttribLocation(shaderProgram, "color");
	glEnableVertexAttribArray(colorAttribute);
	glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float)));
	int texcoordAttribute = glGetAttribLocation(shaderProgram, "texcoord");
	glEnableVertexAttribArray(texcoordAttribute);
	glVertexAttribPointer(texcoordAttribute, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5 * sizeof(float)));


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