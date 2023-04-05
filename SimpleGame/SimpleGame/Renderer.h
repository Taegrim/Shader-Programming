#pragma once

#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <chrono>

#include "Dependencies\glew.h"

class Renderer
{
public:
	Renderer(int windowSizeX, int windowSizeY);
	~Renderer();

	bool IsInitialized();
	void DrawSolidRect(float x, float y, float z, float size, float r, float g, float b, float a);

	void Render();
	void Update(float elapsed_time);
	void DrawParticleEffect();

private:
	void Initialize(int windowSizeX, int windowSizeY);
	bool ReadFile(char* filename, std::string *target);
	void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
	GLuint CompileShaders(char* filenameVS, char* filenameFS);
	void CreateVertexBufferObjects();
	void GetGLPosition(float x, float y, float *newX, float *newY);

	void CreateVBO();
	void CreateParticles(int num);

	bool m_Initialized = false;
	
	unsigned int m_WindowSizeX = 0;
	unsigned int m_WindowSizeY = 0;

	GLuint m_VBORect = 0;
	GLuint m_SolidRectShader = 0;

	GLuint m_testVBO{ 0 };
	GLuint m_testVBO2{ 0 };
	GLuint m_colorVBO{ 0 };

	float m_scale{ 0.f };

	GLuint m_particleShader;
	GLuint m_particleVBO;
	GLuint m_particleVerticesCount;

	GLuint m_velocityVBO;
	GLuint m_emitTimeVBO;
	GLuint m_lifeTimeVBO;
	GLuint m_ampVBO;
	GLuint m_periodVBO;
	GLuint m_valueVBO;

	float m_time{};
};

