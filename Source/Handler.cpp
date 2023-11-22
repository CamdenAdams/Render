#define GLFW_INCLUDE_NONE
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

	uniform mat4 trans;
		
	void main()
	{
		Color = color;
		Texcoord = texcoord;
		gl_Position = trans *  vec4(position, 0.0, 1.0);
	}
)glsl";

const char* fragmentShaderSource = R"glsl(
	#version 150 core

	// uniform vec3 triangleColor;
	in vec3 Color;		
	in vec2 Texcoord;

	out vec4 outColor;
	
	uniform sampler2D texKitten;
	uniform sampler2D texPuppy;
	uniform float time;
		
	void main()
	{
		float factor = (sin(time * 3.0) + 1.0) / 2.0;
		outColor = mix(texture(texKitten, Texcoord), texture(texPuppy, Texcoord), factor);
	}
)glsl";


int main()
{
	auto t_start = std::chrono::high_resolution_clock::now();
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

	

	/*glm::vec4 result = trans * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	printf("%f, %f, %f", result.x, result.y, result.y);*/

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
	glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0);
	int colorAttribute = glGetAttribLocation(shaderProgram, "color");
	glEnableVertexAttribArray(colorAttribute);
	glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float)));
	int texcoordAttribute = glGetAttribLocation(shaderProgram, "texcoord");
	glEnableVertexAttribArray(texcoordAttribute);
	glVertexAttribPointer(texcoordAttribute, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5 * sizeof(float)));
	/*glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::rotate(trans, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));*/
	GLint uniTrans = glGetUniformLocation(shaderProgram, "trans");


	GLuint textures[2];
	glGenTextures(2, textures);

	int width, height;
	unsigned char* image;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	image = SOIL_load_image("Source/sample.png", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glUniform1i(glGetUniformLocation(shaderProgram, "texKitten"), 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	image = SOIL_load_image("Source/sample2.png", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glUniform1i(glGetUniformLocation(shaderProgram, "texPuppy"), 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLuint timeUniform = glGetUniformLocation(shaderProgram, "time");

	while (!glfwWindowShouldClose(window))
	{
		auto t_now = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
		glUniform1f(timeUniform, time);

		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glBindVertexArray(vao);
		glm::mat4 trans = glm::mat4(1.0f);
		trans = glm::rotate(trans, time * glm::radians(180.f), glm::vec3(0.f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(trans));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glfwSwapBuffers(window);
		glfwPollEvents();

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	// all remaining clean up is done here

	glDeleteTextures(2, textures);

	glDeleteProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);

	glDeleteVertexArrays(1, &vao);

	glfwTerminate();

	return 0;
}