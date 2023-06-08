#pragma once

#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <chrono>
#include <array>

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
	void DrawFragmentSandbox();
	void DrawAlphaClear();
	void DrawVertexSandbox();
	void DrawTextureSandbox();
	void DrawSmileTexture();
	void DrawGridMesh();
	void DrawTexture(float x, float y, float scaleX, float scaleY, GLuint texId);
	void DrawFboTexture();

private:
	void Initialize(int windowSizeX, int windowSizeY);
	bool ReadFile(char* filename, std::string *target);
	void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
	GLuint CompileShaders(char* filenameVS, char* filenameFS);
	void CreateVertexBufferObjects();
	void GetGLPosition(float x, float y, float *newX, float *newY);

	void CreateVBO();
	void CreateParticles(int num);
	void CreateFragmentSandbox();
	void CreateAlphaClear();
	void CreateVertexSandbox();
	void CreateTextureSandbox();
	void CreateTextures();
	GLuint CreatePngTexture(char* filePath, GLuint samplingMethod);
	void CreateGridMesh();
	void CreateFBO();
	void CreateFBOs();
	void CreateDrawTextureVBO();

	bool m_Initialized = false;
	
	unsigned int m_WindowSizeX = 0;
	unsigned int m_WindowSizeY = 0;
	unsigned int m_halfSizeX = 0;
	unsigned int m_halfSizeY = 0;

	GLuint m_VBORect = 0;
	GLuint m_SolidRectShader = 0;

	GLuint m_testVBO{ 0 };
	GLuint m_testVBO2{ 0 };

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
	GLuint m_colorVBO;
	GLuint m_uniformVBO;	// position + color

	float m_time{};

	int m_strideCount;

	// ----------------------- 프래그먼트 다루기
	GLuint m_fragmentSandboxShader;
	GLuint m_fragmentSandboxVBO;

	// ----------------------- 잔상 처리 
	GLuint m_alphaClearShader = 0;
	GLuint m_alphaClearVBO = 0;

	// ----------------------- VS 애니메이션
	GLuint m_vertexSandboxShader = 0;
	GLuint m_horiLineVBO = 0;
	GLuint m_horiLineVertexCount = 0;

	// ----------------------- 텍스트 처리
	GLuint m_textureSandboxShader = 0;
	GLuint m_textureSandboxVBO = 0;
	GLuint m_checkerBoardTexture = 0;
	GLuint m_rgbTexture = 0;
	GLuint m_smileTextures[6]{ 0, };
	GLuint m_smileTexture = 0;

	GLuint m_particleTexture = 0;
	GLuint m_spriteTexture = 0;
	GLuint m_step = 0;

	// 그리드 메쉬
	GLuint m_gridMeshShader = 0;
	GLuint m_gridMeshVertexCount = 0;
	GLuint m_gridMeshVBO = 0;


	// ----------------------  프레임 버퍼 오브젝트
	std::array<GLuint, 4> m_fboTextures;
	std::array<GLuint, 4> m_fboAttachTextures;
	GLuint m_depthRenderBuffer = 0;
	std::array<GLuint, 4> m_fbos;

	//// Draw Texture
	GLuint m_drawTextureVBO = 0;
	GLuint m_drawTextureShader = 0;
};

