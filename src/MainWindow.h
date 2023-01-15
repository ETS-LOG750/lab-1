#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <memory>
#include <glm/gtx/euler_angles.hpp>

#include "ShaderProgram.h"

class MainWindow
{
public:
	MainWindow();

	// Main functions (initialization, run)
	int Initialisation();
	int RenderLoop();

	// Callback to intersept GLFW calls
	void FramebufferSizeCallback(int width, int height);

private:
	// Initialize GLFW callbacks
	void InitializeCallback();
	// Intiialize OpenGL objects (shaders, ...)
	int InitializeGL(); 
	
	// Rendering scene (OpenGL)
	void RenderScene();
	// Rendering interface ImGUI
	void RenderImgui();

private:
	// Settings
	const unsigned int SCR_WIDTH = 900;
	const unsigned int SCR_HEIGHT = 720;
	GLFWwindow* m_window = nullptr;

	// Shading information (modified inside the application)
	glm::vec3 m_ambiant = glm::vec3(0.1f);
	glm::vec3 m_diffuse = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 m_specular = glm::vec3(0.0f);
	float m_sExponent = 4.f;

	// Sphere subdivision information
	int m_subdivision_horizontales = 4;
	int m_subdivision_verticales = 4;

	// Shader informations
	std::unique_ptr<ShaderProgram> m_mainShader = nullptr;
	int m_mainShader_pos_loc = -1; 

	// VAO and buffers (geometry)
	enum VAO_IDs { Triangles, NumVAOs };
	enum Buffer_IDs { Positions, NumVBOs };
	GLuint m_VAOs[NumVAOs];
	GLuint m_VBOs[NumVBOs];
};
