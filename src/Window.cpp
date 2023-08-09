#include "Window.h"

const char* vertexShaderSource = R"glsl(
    #version 330 core
    layout(location = 0) in vec2 position;
    layout(location = 1) in vec2 texCoords;
    out vec2 TexCoords;
    
    void main()
    {
        gl_Position = vec4(position, 0.0f, 1.0f);
        TexCoords = texCoords;
    }
)glsl";

const char* fragmentShaderSource = R"glsl(
    #version 330 core
    in vec2 TexCoords;
    out vec4 color;
    
    uniform sampler2D displayTexture;
    
    void main()
    {
        color = texture(displayTexture, TexCoords);
    }
)glsl";

Window::Window(int width, int height, const std::string& title, Chip8* chip8Emulator)
	: m_Window(nullptr), myChip8(chip8Emulator), m_Width(width), m_Height(height), vSynch(0)
{
	if (!glfwInit()) {
		std::cerr << "Error while initializing GLFW" << std::endl;
		return;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (!m_Window) {
		std::cerr << "Error while creating GLFW window" << std::endl;
		return;
	}
	std::cout << "Window Created" << std::endl;

	glfwMakeContextCurrent(m_Window);

	if (!gladLoadGL())
	{
		std::cerr << "Error while initializing GLAD" << std::endl;
		return;
	}
	const char* glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
	std::cerr << "OpenGL Version: {" << glVersion << "}" << std::endl;

	vSynch = 1;
	glfwSwapInterval(vSynch);
	if (vSynch == 1)
	{
		std::cout << "VSynch is enabled!" << std::endl;
	}
	else
		std::cout << "VSynch is not enabled!" << std::endl;

	initOpenGL();
	initializeImGui();

	glfwSetWindowUserPointer(m_Window, this);
	glfwSetFramebufferSizeCallback(m_Window, framebufferSizeCallback);
	glfwSetKeyCallback(m_Window, keyCallback);
}

Window::~Window()
{
	shutdownImGui();
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteTextures(1, &textureID);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteProgram(shaderProgram);
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

void Window::update()
{
	glfwPollEvents();
	glfwSwapBuffers(m_Window);
}

void Window::clear() const
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

bool Window::shouldClose() const
{
	return glfwWindowShouldClose(m_Window);
}

int Window::getWidth() const
{
	return m_Width;
}

int Window::getHeight() const
{
	return m_Height;
}

void Window::initOpenGL()
{
	float vertices[] = {
		// positions   // texture coords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Texture for Chip-8 display
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
	fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
	shaderProgram = glCreateProgram();
	linkProgram(shaderProgram, vertexShader, fragmentShader);

	glUseProgram(shaderProgram);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

GLuint Window::compileShader(const char* source, GLenum type)
{
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		std::cerr << "Shader Compilation Error\n" << infoLog << std::endl;
	}

	return shader;
}

void Window::linkProgram(GLuint program, GLuint vertexShader, GLuint fragmentShader)
{
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);
	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(program, 512, nullptr, infoLog);

		std::cerr << "Program Linking Error\n" << infoLog << std::endl;
	}
}

void Window::initializeImGui() const
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(m_Window, true);

	const char* glsl_version = "#version 330";
	ImGui_ImplOpenGL3_Init(glsl_version);

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void Window::renderImGui() const
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_NoResize | ImGuiDockNodeFlags_PassthruCentralNode);

	// For the debugger
	float windowWidth = 300.0f;
	float windowHeight = 700.0f;

	ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_FirstUseEver);

	ImGui::SetNextWindowDockID(ImGui::GetID("CHIP-8 Debugger"), ImGuiCond_FirstUseEver);
	ImGui::Begin("CHIP-8 Debugger", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

	ImGui::Text("Display Frequency: %.1f Hz", ImGui::GetIO().DisplayFramebufferScale.x * glfwGetVideoMode(glfwGetPrimaryMonitor())->refreshRate);

	bool vsyncEnabled = vSynch ? true : false;
	ImGui::Text("VSynch: %s", vsyncEnabled ? "On" : "Off");

	ImGui::Text("Frame Time: %.2f ms", 1000.0f / ImGui::GetIO().Framerate);
	ImGui::Text("Delta Time: %.6f ms", ImGui::GetIO().DeltaTime * 1000.0f);

	ImGui::Text("Current Opcode: 0x%04X", myChip8->opcode);

	ImGui::Text("Registers:");
	for (int i = 0; i < 16; ++i) {
		ImGui::Text("V%X: 0x%02X", i, myChip8->registers[i]);
	}
	ImGui::Text("Index Register (I): 0x%04X", myChip8->indexRegister);

	ImGui::Text("Stack Levels:");
	for (int i = 0; i < 16; ++i) {
		ImGui::Text("Stack[%d]: 0x%04X", i, myChip8->stack[i]);
	}

	ImGui::Text("Stack Pointer (SP): 0x%02X", myChip8->sp);
	ImGui::Text("Program Counter (PC): 0x%04X", myChip8->pc);

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Window::shutdownImGui() const
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width - 300.0f, height);
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Window* winInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (!winInstance || !winInstance->myChip8) return;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (action == GLFW_PRESS || action == GLFW_RELEASE)
	{
		bool isPressed = (action == GLFW_PRESS);

		switch (key)
		{
		case GLFW_KEY_3: winInstance->myChip8->keypad[1] = isPressed; break;
		case GLFW_KEY_4: winInstance->myChip8->keypad[2] = isPressed; break;
		case GLFW_KEY_5: winInstance->myChip8->keypad[3] = isPressed; break;
		case GLFW_KEY_6: winInstance->myChip8->keypad[0xC] = isPressed; break;

		case GLFW_KEY_E: winInstance->myChip8->keypad[4] = isPressed; break;
		case GLFW_KEY_R: winInstance->myChip8->keypad[5] = isPressed; break;
		case GLFW_KEY_T: winInstance->myChip8->keypad[6] = isPressed; break;
		case GLFW_KEY_Y: winInstance->myChip8->keypad[0xD] = isPressed; break;

		case GLFW_KEY_D: winInstance->myChip8->keypad[7] = isPressed; break;
		case GLFW_KEY_F: winInstance->myChip8->keypad[8] = isPressed; break;
		case GLFW_KEY_G: winInstance->myChip8->keypad[9] = isPressed; break;
		case GLFW_KEY_H: winInstance->myChip8->keypad[0xE] = isPressed; break;

		case GLFW_KEY_C: winInstance->myChip8->keypad[0xA] = isPressed; break;
		case GLFW_KEY_V: winInstance->myChip8->keypad[0] = isPressed; break;
		case GLFW_KEY_B: winInstance->myChip8->keypad[0xB] = isPressed; break;
		case GLFW_KEY_N: winInstance->myChip8->keypad[0xF] = isPressed; break;

		default: break;
		}
	}
}