#define GLEW_INCLUDE_NONE
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <chrono>

#include "Shader/VertexShaderStrings.h"
#include "vertices.h"

enum ShaderLogType {
    COMPILE,
    LINK
};


GLuint loadTexture(const GLchar* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height;
    unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
    glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}

GLuint shaderLogCheck(GLuint shader, ShaderLogType type) {
	GLint success = GL_TRUE;
	GLchar infoLog[512];
	
    if(type == COMPILE) {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if(!success) {
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
			return 0;
		}
	} else if(type == LINK) {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if(!success) {
			glGetProgramInfoLog(shader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
			return 0;
		}
	}

	return success;
}

int createShaderProgram(const GLchar* vertexSource, const GLchar* fragmentSource, GLuint& vertexShader, GLuint& fragmentShader, GLuint& shaderProgram) {
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    shaderLogCheck(vertexShader, COMPILE);

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    shaderLogCheck(fragmentShader, COMPILE);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    shaderLogCheck(shaderProgram, LINK);

    return 1;
}

void setSceneVertexAttributes(GLuint shaderProgram) {
    // 
}

void setScreenVertexAttributes(GLuint shaderProgram) {
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

    GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
}

int main() {
	
    GLFWwindow* window;

    if(!glfwInit())
        return -1;

    window = glfwCreateWindow(1280, 960, "Render", NULL, NULL);
    if(!window) {
		glfwTerminate();
		return -2;
	}

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        return -3;

    GLuint vaoCube, vaoQuad;
    glGenVertexArrays(1, &vaoCube);
    glGenVertexArrays(1, &vaoQuad);

    GLuint vboCube, vboQuad;
    glGenBuffers(1, &vboCube);
    glGenBuffers(1, &vboQuad);

    glBindBuffer(GL_ARRAY_BUFFER, vboCube);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices::cubeVertices), Vertices::cubeVertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, vboQuad);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices::quadVertices), Vertices::quadVertices, GL_STATIC_DRAW);

    ShaderStruct* shaderSources = new ShaderStruct();

    /*GLuint sceneVertexShader, sceneFragmentShader, sceneShaderProgram;
    createShaderProgram(shaderSources->sceneVertexSource, shaderSources->sceneFragmentSource, sceneVertexShader, sceneFragmentShader, sceneShaderProgram);*/

    GLuint screenVertexShader, screenFragmentShader, screenShaderProgram;
    createShaderProgram(shaderSources->screenVertexSource, shaderSources->screenFragmentSource, screenVertexShader, screenFragmentShader, screenShaderProgram);

    glBindVertexArray(vaoQuad);
    glBindBuffer(GL_ARRAY_BUFFER, vboQuad);
    setScreenVertexAttributes(screenShaderProgram);

    glUseProgram(screenShaderProgram);
    glUniform1i(glGetUniformLocation(screenShaderProgram, "texFramebuffer"), 0);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 168.0f / 255.0f, 107.0f / 255.0f, 1.0f);

        //custom framebuffer operations here

        {

        }

        //set default framebuffer
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindVertexArray(vaoQuad);
        //glDisable(GL_DEPTH_TEST);
        glUseProgram(screenShaderProgram);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
        
        if (glfwGetKey(window, GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    glDeleteVertexArrays(1, &vaoCube);
    glDeleteVertexArrays(1, &vaoQuad);
    glDeleteBuffers(1, &vboCube);
    glDeleteBuffers(1, &vboQuad);
    /*glDeleteShader(sceneVertexShader);
    glDeleteShader(sceneFragmentShader);
    glDeleteProgram(sceneShaderProgram);*/
    glDeleteShader(screenVertexShader);
    glDeleteShader(screenFragmentShader);
    glDeleteProgram(screenShaderProgram);

    delete shaderSources;

	return 0;
}