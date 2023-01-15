#include "MainWindow.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/quaternion.hpp>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

MainWindow::MainWindow() 
{
}

int MainWindow::Initialisation()
{
	// OpenGL version (usefull for imGUI and other libraries)
	const char* glsl_version = "#version 430 core";

	// glfw: initialize and configure
	// ------------------------------
	glfwInit();

	// Request OpenGL 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	m_window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Labo 1", NULL, NULL);
	if (m_window == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(m_window);
	InitializeCallback();

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return 2;
	}

	// imGui: create interface
	// ---------------------------------------
	// Setup Dear ImGui context
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;// Enable Keyboard Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Other openGL initialization
	// -----------------------------
	return InitializeGL();
}

void MainWindow::InitializeCallback() {
	// These callbacks are to capture event inside the window (mouse mouvement, window resize)
	// Register a custom pointer (to be able to interact with the main application on each event call)
	glfwSetWindowUserPointer(m_window, reinterpret_cast<void*>(this));
	// Example: Change of window size change
	glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
		MainWindow* w = reinterpret_cast<MainWindow*>(glfwGetWindowUserPointer(window));
		w->FramebufferSizeCallback(width, height);
	});
}

int MainWindow::InitializeGL()
{
	// SHADERS_DIR is configured inside CMake
	const std::string directory = SHADERS_DIR;
	
	/////////////////////////////
	// Load the main shader
	/////////////////////////////
	bool mainShaderSuccess = true;
	m_mainShader = std::make_unique<ShaderProgram>();
	mainShaderSuccess &= m_mainShader->addShaderFromSource(GL_VERTEX_SHADER, directory + "basic.vert");
	mainShaderSuccess &= m_mainShader->addShaderFromSource(GL_FRAGMENT_SHADER, directory + "basic.frag");
	mainShaderSuccess &= m_mainShader->link();
	if (!mainShaderSuccess) {
		std::cerr << "Error when loading main shader\n";
		return 4;
	}
	// Get important information from the shader
	if ((m_mainShader_pos_loc = m_mainShader->attributeLocation("vPos")) < 0) {
		std::cerr << "Unable to find shader location for " << "vPos" << std::endl;
		return 3;
	}

	/////////////////////////////
	// Load geometry
	/////////////////////////////
	// Generate VAO and VBO
	glGenVertexArrays(NumVAOs, m_VAOs);
	glGenBuffers(NumVBOs, m_VBOs);

	// Load data on the VBO
	// TODO: Change to make a sphere, look at 01_Triangles examples
	//	Also make a fonction to create proper geometry based on GUI values
	const GLuint NumVertices = 3;
	GLfloat vertices[NumVertices][2] = {
		{ -0.90f, -0.90f }, // Triangle 1
		{ 0.85f, -0.90f },
		{ -0.90f, 0.85f }
	};
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[Positions]); // Activate
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Copy
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Desactivate (Optional)
	
	// Configuration VAO
	glBindVertexArray(m_VAOs[Triangles]);
	// --- Positions
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[Positions]); // Activate'
	// Doc: https://registry.khronos.org/OpenGL-Refpages/gl4/html/glVertexAttribPointer.xhtml
	glVertexAttribPointer(GLuint(m_mainShader_pos_loc), 
		2, // 2D information
		GL_FLOAT, // type: float
		GL_FALSE, // do not need to normalize
		0, // No stride (packed data)
		BUFFER_OFFSET(0) // Ptr to the first component (= start right away)
	);
	glEnableVertexAttribArray(GLuint(m_mainShader_pos_loc));
	// Clean-up (sometimes unecessary)
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Other configurations
	// - Activate depth test: Make sure that surface behind do not show
	glEnable(GL_DEPTH_TEST);

	return 0;
}

void MainWindow::RenderImgui()
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//imgui 
	{
		ImGui::Begin("Labo 1");
		
		// Color
		ImGui::Separator();
		ImGui::Text("Shading: ");
		ImGui::ColorEdit3("Ambiante", &m_ambiant[0]);
		ImGui::ColorEdit3("Diffuse", &m_diffuse[0]);
		ImGui::ColorEdit3("Speculaire", &m_specular[0]);
		ImGui::InputFloat("Speculaire Exponent", &m_sExponent, 1.0f);

		// Other options
		ImGui::Separator();
		ImGui::Text("Sphere Subdivision: ");
		bool changed = false;
		changed |= ImGui::InputInt("Horizontales", &m_subdivision_horizontales);
		changed |= ImGui::InputInt("Verticales", &m_subdivision_verticales);
		if (changed) {
			// Condition true if latitude or longitude have been changed
			// TODO: Reupdate VBO on the GPU
		}

		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void MainWindow::RenderScene()
{
	// Clear color and depth (to know which surface is in front or not)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	m_mainShader->bind(); // Activate shader
	// Draw triangle
	glBindVertexArray(m_VAOs[Triangles]); // Activate VAO
	glDrawArrays(GL_TRIANGLES, 0, 3);     // Draw 3 vertices (= one triangle)
	// TODO: You will need to change the number of verticies based on sphere geometry
}


int MainWindow::RenderLoop()
{
	while (!glfwWindowShouldClose(m_window))
	{
		// Check inputs: Does ESC was pressed?
		if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(m_window, true);

		RenderScene();
		RenderImgui();

		// Show rendering and get events
		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(m_window);
	glfwTerminate();

	return 0;
}

void MainWindow::FramebufferSizeCallback(int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}