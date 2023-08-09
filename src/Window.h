#pragma once 

#include <iostream>
#include <string>
#include <glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Chip8.h"

class Window {
public:
	Window(int width, int height, const std::string& title, Chip8* chip8Emulator);
	~Window();

	void update();
	void renderImGui() const;
	void clear() const;
	bool shouldClose() const;
	int getWidth() const;
	int getHeight() const;
	GLuint* getTexture() { return &textureID; }
	GLuint* getVAO() { return &VAO; }
private:
	void initializeImGui() const;
	void shutdownImGui() const;
private:
	GLFWwindow* m_Window;
	Chip8* myChip8;
	int m_Width, m_Height;
	int vSynch;
	
	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
private:
	GLuint VAO{}, VBO{}, textureID{};
	GLuint shaderProgram{}, vertexShader{}, fragmentShader{};
private:
	void initOpenGL();
	GLuint compileShader(const char* source, GLenum type);
	void linkProgram(GLuint program, GLuint vertexShader, GLuint fragmentShader);
};
