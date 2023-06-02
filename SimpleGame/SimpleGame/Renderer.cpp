#include "stdafx.h"
#include "Renderer.h"
#include <assert.h>
#include <random>
#include "LoadPng.h"

constexpr int PARTICLE_NUM = 600;
constexpr float PARTICLE_SIZE = 0.1f;
constexpr float PARTICLE_MAX_SPEED = 0.5f;

std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_real_distribution<float> random_emit_time(0.f, 5.f);
std::uniform_real_distribution<float> random_life_time(2.f, 3.5f);
std::uniform_real_distribution<float> random_velocity_x(-PARTICLE_MAX_SPEED, PARTICLE_MAX_SPEED);
std::uniform_real_distribution<float> random_velocity_y(PARTICLE_MAX_SPEED, PARTICLE_MAX_SPEED + 0.5f);
std::uniform_real_distribution<float> random_amp(-0.1f, 0.1f);
std::uniform_real_distribution<float> random_period(1.f, 2.f);
std::uniform_real_distribution<float> random_angle(0, 1.f);
std::uniform_real_distribution<float> random_color(0.f, 1.f);

Renderer::Renderer(int windowSizeX, int windowSizeY)
{
	Initialize(windowSizeX, windowSizeY);
	//CreateVBO();
}

Renderer::~Renderer()
{
}

void Renderer::Initialize(int windowSizeX, int windowSizeY)
{
	//Set window size
	m_WindowSizeX = windowSizeX;
	m_WindowSizeY = windowSizeY;

	//Load shaders
	m_SolidRectShader = CompileShaders("./Shaders/SolidRect.vs",
		"./Shaders/SolidRect.fs");
	m_particleShader = CompileShaders("./Shaders/Particle.vs",
		"./Shaders/Particle.fs");
	m_fragmentSandboxShader = CompileShaders("./Shaders/FragmentSandbox.vs",
		"./Shaders/FragmentSandbox.fs");
	m_alphaClearShader = CompileShaders("./Shaders/AlphaClear.vs",
		"./Shaders/AlphaClear.fs");
	m_vertexSandboxShader = CompileShaders("./Shaders/VertexSandbox.vs",
		"./Shaders/VertexSandbox.fs");
	m_textureSandboxShader = CompileShaders("./Shaders/TextureSandbox.vs",
		"./Shaders/TextureSandbox.fs");
	m_gridMeshShader = CompileShaders("./Shaders/GridMesh.vs",
		"./Shaders/GridMesh.fs");
	m_drawTextureShader = CompileShaders("./Shaders/DrawTexture.vs",
		"./Shaders/DrawTexture.fs");

	//Create VBOs
	//CreateVertexBufferObjects();
	CreateParticles(PARTICLE_NUM);
	CreateFragmentSandbox();
	CreateAlphaClear();
	CreateVertexSandbox();
	CreateTextureSandbox();
	CreateGridMesh();
	CreateDrawTextureVBO();


	// LoadTexture
	CreateTextures();
	m_rgbTexture = CreatePngTexture("./Resource/그림1.png", GL_NEAREST);

	m_smileTextures[0] = CreatePngTexture("./Resource/texture1.png", GL_NEAREST);
	m_smileTextures[1] = CreatePngTexture("./Resource/texture2.png", GL_NEAREST);
	m_smileTextures[2] = CreatePngTexture("./Resource/texture3.png", GL_NEAREST);
	m_smileTextures[3] = CreatePngTexture("./Resource/texture4.png", GL_NEAREST);
	m_smileTextures[4] = CreatePngTexture("./Resource/texture5.png", GL_NEAREST);
	m_smileTextures[5] = CreatePngTexture("./Resource/texture6.png", GL_NEAREST);

	m_smileTexture = CreatePngTexture("./Resource/Smile.png", GL_NEAREST);
	
	m_particleTexture = CreatePngTexture("./Resource/Orb.png", GL_NEAREST);


	m_spriteTexture = CreatePngTexture("./Resource/explosion.png", GL_NEAREST);

	// FBO
	CreateFBO();

	if (m_SolidRectShader > 0 && m_VBORect > 0)
	{
		m_Initialized = true;
	}
}

bool Renderer::IsInitialized()
{
	return m_Initialized;
}

void Renderer::CreateVertexBufferObjects()
{
	float rect[]
		=
	{
		-1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f, -1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, 1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, //Triangle1
		-1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f,  1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, 1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f, //Triangle2
	};

	glGenBuffers(1, &m_VBORect);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);
}

void Renderer::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	//쉐이더 오브젝트 생성
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);
	//쉐이더 코드를 쉐이더 오브젝트에 할당
	glShaderSource(ShaderObj, 1, p, Lengths);

	//할당된 쉐이더 코드를 컴파일
	glCompileShader(ShaderObj);

	GLint success;
	// ShaderObj 가 성공적으로 컴파일 되었는지 확인
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];

		//OpenGL 의 shader log 데이터를 가져옴
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		printf("%s \n", pShaderText);
	}

	// ShaderProgram 에 attach!!
	glAttachShader(ShaderProgram, ShaderObj);
}

bool Renderer::ReadFile(char* filename, std::string *target)
{
	std::ifstream file(filename);
	if (file.fail())
	{
		std::cout << filename << " file loading failed.. \n";
		file.close();
		return false;
	}
	std::string line;
	while (getline(file, line)) {
		target->append(line.c_str());
		target->append("\n");
	}
	return true;
}

GLuint Renderer::CompileShaders(char* filenameVS, char* filenameFS)
{
	GLuint ShaderProgram = glCreateProgram(); //빈 쉐이더 프로그램 생성

	if (ShaderProgram == 0) { //쉐이더 프로그램이 만들어졌는지 확인
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, fs;

	//shader.vs 가 vs 안으로 로딩됨
	if (!ReadFile(filenameVS, &vs)) {
		printf("Error compiling vertex shader\n");
		return -1;
	};

	//shader.fs 가 fs 안으로 로딩됨
	if (!ReadFile(filenameFS, &fs)) {
		printf("Error compiling fragment shader\n");
		return -1;
	};

	// ShaderProgram 에 vs.c_str() 버텍스 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	// ShaderProgram 에 fs.c_str() 프레그먼트 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	//Attach 완료된 shaderProgram 을 링킹함
	glLinkProgram(ShaderProgram);

	//링크가 성공했는지 확인
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		// shader program 로그를 받아옴
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error linking shader program\n" << ErrorLog;
		return -1;
	}

	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error validating shader program\n" << ErrorLog;
		return -1;
	}

	glUseProgram(ShaderProgram);
	std::cout << filenameVS << ", " << filenameFS << " Shader compiling is done.\n";

	return ShaderProgram;
}

void Renderer::DrawSolidRect(float x, float y, float z, float size, float r, float g, float b, float a)
{
	float newX, newY;

	GetGLPosition(x, y, &newX, &newY);

	//Program select
	glUseProgram(m_SolidRectShader);

	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Trans"), newX, newY, 0, size);
	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Color"), r, g, b, a);

	int attribPosition = glGetAttribLocation(m_SolidRectShader, "a_Position");
	glEnableVertexAttribArray(attribPosition);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(attribPosition);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawFragmentSandbox()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, m_WindowSizeX / 2, m_WindowSizeY / 2);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
				GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
	glDrawBuffers(5, drawBuffers);
;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLuint shader = m_fragmentSandboxShader;
	glUseProgram(shader);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int posLocation = glGetAttribLocation(shader, "a_position");
	int texLocation = glGetAttribLocation(shader, "a_texCoord");
	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(texLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_fragmentSandboxVBO);
	glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE,
		sizeof(float) * 5, 0);
	glVertexAttribPointer(texLocation, 2, GL_FLOAT, GL_FALSE,
		sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));

	/*int uniformLocation = glGetUniformLocation(shader, "u_points");
	glUniform2f(uniformLocation, 0.5f, 0.5f);*/

	float points[] = { 0.5f, 0.5f, 0.9f, 0.9f, 0.1f, 0.1f };
	int uniformLocation = glGetUniformLocation(shader, "u_points");
	glUniform2fv(uniformLocation, 3, points);

	uniformLocation = glGetUniformLocation(shader, "u_time");
	glUniform1f(uniformLocation, m_time);

	int texULocation = glGetUniformLocation(shader, "u_texture");
	glUniform1i(texULocation, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_rgbTexture);

	uniformLocation = glGetUniformLocation(shader, "u_step");
	glUniform1i(uniformLocation, m_step);
	 
	m_step = (int)(m_time * 10.0f) % 48;

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	DrawTexture(-0.5,  0.5, 256, 256, m_fboAttachTextures[0]);
	DrawTexture( 0.5,  0.5, 256, 256, m_fboAttachTextures[1]);
	DrawTexture(-0.5, -0.5, 256, 256, m_fboAttachTextures[2]);
	DrawTexture( 0.5, -0.5, 256, 256, m_fboAttachTextures[3]);
}

void Renderer::DrawAlphaClear()
{
	GLuint shader = m_alphaClearShader;
	glUseProgram(shader);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int posLocation = glGetAttribLocation(shader, "a_position");
	glEnableVertexAttribArray(posLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_alphaClearVBO);
	glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisable(GL_BLEND);
}

void Renderer::DrawVertexSandbox()
{
	GLuint shader = m_vertexSandboxShader;
	glUseProgram(shader);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int posLocation = glGetAttribLocation(shader, "a_position");
	glEnableVertexAttribArray(posLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_horiLineVBO);
	glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	int uniformLocation = glGetUniformLocation(shader, "u_time");
	glUniform1f(uniformLocation, m_time);

	glDrawArrays(GL_LINE_STRIP, 0, m_horiLineVertexCount);


	glUniform1f(uniformLocation, m_time + 0.3f);
	glDrawArrays(GL_LINE_STRIP, 0, m_horiLineVertexCount);

	glUniform1f(uniformLocation, m_time + 0.5f);
	glDrawArrays(GL_LINE_STRIP, 0, m_horiLineVertexCount);

	glDisable(GL_BLEND);
}

void Renderer::DrawTextureSandbox()
{
	GLuint shader = m_textureSandboxShader;
	glUseProgram(shader);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int posLocation = glGetAttribLocation(shader, "a_position");
	int texLocation = glGetAttribLocation(shader, "a_texPos");
	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(texLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_textureSandboxVBO);
	glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE,
		sizeof(float) * 5, 0);
	glVertexAttribPointer(texLocation, 2, GL_FLOAT, GL_FALSE,
		sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));

	int samplerLocation = glGetUniformLocation(shader, "u_texSampler");
	glUniform1i(samplerLocation, 0);
	glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, m_checkerBoardTexture);
	glBindTexture(GL_TEXTURE_2D, m_rgbTexture);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	int repeatLoc = glGetUniformLocation(shader, "u_xyRepeat");

	float repeat = (float)((int)m_time % 4) + 1.0f;
	glUniform2f(repeatLoc, repeat, 2.0f);

	glDisable(GL_BLEND);
}

void Renderer::DrawSmileTexture()
{
	GLuint shader = m_textureSandboxShader;
	glUseProgram(shader);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int posLocation = glGetAttribLocation(shader, "a_position");
	int texLocation = glGetAttribLocation(shader, "a_texPos");
	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(texLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_textureSandboxVBO);
	glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE,
		sizeof(float) * 5, 0);
	glVertexAttribPointer(texLocation, 2, GL_FLOAT, GL_FALSE,
		sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));

	// ------------ 단일 텍스쳐 여러 장 --------------
	/*int id = (int)m_time % 6;
	int samplerLocation = glGetUniformLocation(shader, "u_texSampler");
	glUniform1i(samplerLocation, id);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_smileTextures[0]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_smileTextures[1]);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_smileTextures[2]);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_smileTextures[3]);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_smileTextures[4]);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, m_smileTextures[5]);*/
	// ------------ 단일 텍스쳐 여러 장 --------------

	// 이어붙인 단일 텍스쳐 사용
	int samplerLocation = glGetUniformLocation(shader, "u_texSampler");
	glUniform1i(samplerLocation, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_smileTexture);

	int offset = (int)m_time % 6;
	int offsetLocation = glGetUniformLocation(shader, "u_offset");
	glUniform1i(offsetLocation, offset);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisable(GL_BLEND);
}

void Renderer::DrawGridMesh()
{
	GLuint shader = m_gridMeshShader;
	glUseProgram(shader);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int posLocation = glGetAttribLocation(shader, "a_position");
	glEnableVertexAttribArray(posLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_gridMeshVBO);
	glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, m_gridMeshVertexCount);

	int uniformLocation = glGetUniformLocation(shader, "u_time");
	glUniform1f(uniformLocation, m_time);

	int samplerLocation = glGetUniformLocation(shader, "u_texSampler");
	glUniform1i(samplerLocation, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_smileTextures[0]);

	glDisable(GL_BLEND);
}

void Renderer::DrawTexture(float x, float y, float scaleX, float scaleY, GLuint texId)
{
	GLuint shader = m_drawTextureShader;
	glUseProgram(shader);

	GLuint posLocation = glGetAttribLocation(shader, "a_position");
	glEnableVertexAttribArray(posLocation);

	GLuint texLocation = glGetAttribLocation(shader, "a_texPos");
	glEnableVertexAttribArray(texLocation);

	glBindBuffer(GL_ARRAY_BUFFER, m_drawTextureVBO);
	glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	glVertexAttribPointer(texLocation, 2, GL_FLOAT, GL_FALSE,
		sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));

	GLuint samplerLocation = glGetUniformLocation(shader, "u_texSampler");
	glUniform1i(samplerLocation, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texId);

	GLuint posScaleLocation = glGetUniformLocation(shader, "u_posScale");
	glUniform4f(posScaleLocation, x, y, scaleX, scaleY);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::Render()
{
	int shader_program = m_particleShader;
	glUseProgram(shader_program);

	glUniform4f(glGetUniformLocation(shader_program, "u_Trans"), 0.f, 0.f, 0.f, 1.f);
	glUniform4f(glGetUniformLocation(shader_program, "u_Color"), 1.f, 1.f, 1.f, 1.f);

	int attribLocation = glGetAttribLocation(shader_program, "a_Position");
	glEnableVertexAttribArray(attribLocation);
	glBindBuffer(GL_ARRAY_BUFFER, m_testVBO);
	glVertexAttribPointer(attribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	attribLocation = glGetAttribLocation(shader_program, "a_Position2");
	glEnableVertexAttribArray(attribLocation);
	glBindBuffer(GL_ARRAY_BUFFER, m_testVBO2);
	glVertexAttribPointer(attribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	attribLocation = glGetAttribLocation(shader_program, "a_Color");
	glEnableVertexAttribArray(attribLocation);
	glBindBuffer(GL_ARRAY_BUFFER, m_colorVBO);
	glVertexAttribPointer(attribLocation, 4, GL_FLOAT, GL_FALSE, 0, nullptr);


	int uniformLocation = glGetUniformLocation(m_SolidRectShader, "u_Scale");
	
	m_scale += 0.01f;
	if (m_scale >= 1.f)
		m_scale = 0.f;

	glUniform1f(uniformLocation, m_scale);

	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Renderer::Update(float elapsed_time)
{
	m_time += elapsed_time;
}

void Renderer::DrawParticleEffect()
{
	int shader_program = m_particleShader;
	glUseProgram(shader_program);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);

	int attribLocationPos = glGetAttribLocation(shader_program, "a_position");
	glEnableVertexAttribArray(attribLocationPos);

	int attribLocationColor = glGetAttribLocation(shader_program, "a_color");
	glEnableVertexAttribArray(attribLocationColor);

	int attribLocationVel = glGetAttribLocation(shader_program, "a_velocity");
	glEnableVertexAttribArray(attribLocationVel);

	int attribLocationUv = glGetAttribLocation(shader_program, "a_uv");
	glEnableVertexAttribArray(attribLocationUv);

	glBindBuffer(GL_ARRAY_BUFFER, m_uniformVBO);
	glVertexAttribPointer(attribLocationPos, 3, GL_FLOAT,
		GL_FALSE, sizeof(float) * m_strideCount, 0);
	glVertexAttribPointer(attribLocationColor, 4, GL_FLOAT,
		GL_FALSE, sizeof(float) * m_strideCount, reinterpret_cast<GLvoid*>(sizeof(float) * 3));
	glVertexAttribPointer(attribLocationVel, 3, GL_FLOAT,
		GL_FALSE, sizeof(float) * m_strideCount, reinterpret_cast<GLvoid*>(sizeof(float) * 7));
	glVertexAttribPointer(attribLocationUv, 2, GL_FLOAT,
		GL_FALSE, sizeof(float) * m_strideCount, reinterpret_cast<GLvoid*>(sizeof(float) * 10));


	int attribLocation = glGetAttribLocation(shader_program, "a_emitTime");
	glEnableVertexAttribArray(attribLocation);
	glBindBuffer(GL_ARRAY_BUFFER, m_emitTimeVBO);
	glVertexAttribPointer(attribLocation, 1, GL_FLOAT, GL_FALSE, 0, nullptr);

	attribLocation = glGetAttribLocation(shader_program, "a_lifeTime");
	glEnableVertexAttribArray(attribLocation);
	glBindBuffer(GL_ARRAY_BUFFER, m_lifeTimeVBO);
	glVertexAttribPointer(attribLocation, 1, GL_FLOAT, GL_FALSE, 0, nullptr);

	attribLocation = glGetAttribLocation(shader_program, "a_period");
	glEnableVertexAttribArray(attribLocation);
	glBindBuffer(GL_ARRAY_BUFFER, m_periodVBO);
	glVertexAttribPointer(attribLocation, 1, GL_FLOAT, GL_FALSE, 0, nullptr);

	attribLocation = glGetAttribLocation(shader_program, "a_amp");
	glEnableVertexAttribArray(attribLocation);
	glBindBuffer(GL_ARRAY_BUFFER, m_ampVBO);
	glVertexAttribPointer(attribLocation, 1, GL_FLOAT, GL_FALSE, 0, nullptr);

	attribLocation = glGetAttribLocation(shader_program, "a_value");
	glEnableVertexAttribArray(attribLocation);
	glBindBuffer(GL_ARRAY_BUFFER, m_valueVBO);
	glVertexAttribPointer(attribLocation, 1, GL_FLOAT, GL_FALSE, 0, nullptr);



	int uniformLocation = glGetUniformLocation(shader_program, "u_time");

	glUniform1f(uniformLocation, m_time);

	/*int texLocation = glGetUniformLocation(shader_program, "u_texture");
	glUniform1i(texLocation, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_particleTexture);*/


	int texULocation = glGetUniformLocation(shader_program, "u_texture");
	glUniform1i(texULocation, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_spriteTexture);

	uniformLocation = glGetUniformLocation(shader_program, "u_step");
	glUniform1i(uniformLocation, m_step);

	m_step = (int)(m_time * 10.0f) % 48;

	glDrawArrays(GL_TRIANGLES, 0, m_particleVerticesCount);

	glDisable(GL_BLEND);
}

void Renderer::GetGLPosition(float x, float y, float *newX, float *newY)
{
	*newX = x * 2.f / m_WindowSizeX;
	*newY = y * 2.f / m_WindowSizeY;
}

void Renderer::CreateVBO()
{
	float vertices[] = { 0.f, 0.f, 0.f,
					1.f, 0.f, 0.f,
					1.f, 1.f, 0.f };

	glGenBuffers(1, &m_testVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_testVBO);	// VBO 를 ARRAY_BUFFER 에 Bind 함
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // CPU의 데이터를 GPU 에 전달

	
	float vertices2[] = { -1.f, -1.f, 0.f,
					0.f, -1.f, 0.f,
					0.f, 0.f, 0.f };

	glGenBuffers(1, &m_testVBO2);
	glBindBuffer(GL_ARRAY_BUFFER, m_testVBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

	float color[] = { 1.f, 0.f, 0.f, 1.f,
					0.f, 1.f, 0.f, 1.f,
					0.f, 0.f, 1.f, 1.f };

	glGenBuffers(1, &m_colorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_colorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
}

void Renderer::CreateParticles(int num)
{
	float centerX{}, centerY{};

	m_particleVerticesCount = num * 6;
	int MAX_FLOAT = m_particleVerticesCount * 3;

	float* vertices = new float[MAX_FLOAT];

	for (int i = 0; i < MAX_FLOAT;) {
		centerX = 0.f;
		centerY = 0.f;

		vertices[i++] = centerX - PARTICLE_SIZE;
		vertices[i++] = centerY + PARTICLE_SIZE;
		vertices[i++] = 0.f;
		vertices[i++] = centerX - PARTICLE_SIZE;
		vertices[i++] = centerY - PARTICLE_SIZE;
		vertices[i++] = 0.f;
		vertices[i++] = centerX + PARTICLE_SIZE;
		vertices[i++] = centerY + PARTICLE_SIZE;
		vertices[i++] = 0.f;

		vertices[i++] = centerX + PARTICLE_SIZE;
		vertices[i++] = centerY + PARTICLE_SIZE;
		vertices[i++] = 0.f;
		vertices[i++] = centerX - PARTICLE_SIZE;
		vertices[i++] = centerY - PARTICLE_SIZE;
		vertices[i++] = 0.f;
		vertices[i++] = centerX + PARTICLE_SIZE;
		vertices[i++] = centerY - PARTICLE_SIZE;
		vertices[i++] = 0.f;
	}

	glGenBuffers(1, &m_particleVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_particleVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * MAX_FLOAT, vertices, GL_STATIC_DRAW);

	delete[] vertices;

	int MAX_COLORS = m_particleVerticesCount * 4;
	float* colors = new float[MAX_COLORS];

	float r{}, g{}, b{}, a{};
	for (int i = 0; i < MAX_COLORS;) {
		r = random_color(dre);
		g = random_color(dre);
		b = random_color(dre);
		a = random_color(dre);

		colors[i++] = r;
		colors[i++] = g;
		colors[i++] = b;
		colors[i++] = a;

		colors[i++] = r;
		colors[i++] = g;
		colors[i++] = b;
		colors[i++] = a;

		colors[i++] = r;
		colors[i++] = g;
		colors[i++] = b;
		colors[i++] = a;


		colors[i++] = r;
		colors[i++] = g;
		colors[i++] = b;
		colors[i++] = a;

		colors[i++] = r;
		colors[i++] = g;
		colors[i++] = b;
		colors[i++] = a;

		colors[i++] = r;
		colors[i++] = g;
		colors[i++] = b;
		colors[i++] = a;
	}

	glGenBuffers(1, &m_colorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_colorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * MAX_COLORS, colors, GL_STATIC_DRAW);

	delete[] colors;

	m_strideCount = (3 + 4 + 3 + 2);
	int MAX_UNIFORM_COUNT = m_particleVerticesCount * m_strideCount;
	float* uniforms = new float[MAX_UNIFORM_COUNT];
	
	float xVelocity{};
	float yVelocity{};

	for (int i = 0; i < MAX_UNIFORM_COUNT;) {
		centerX = 0.f;
		centerY = 0.f;
		r = random_color(dre);
		g = random_color(dre);
		b = random_color(dre);
		a = random_color(dre);
		xVelocity = random_velocity_x(dre);
		yVelocity = random_velocity_y(dre);

		uniforms[i++] = centerX - PARTICLE_SIZE;
		uniforms[i++] = centerY + PARTICLE_SIZE;
		uniforms[i++] = 0.f;
		uniforms[i++] = r;
		uniforms[i++] = g;
		uniforms[i++] = b;
		uniforms[i++] = 1.f;
		uniforms[i++] = xVelocity;
		uniforms[i++] = yVelocity;
		uniforms[i++] = 0.f;
		uniforms[i++] = 0.f;
		uniforms[i++] = 0.f;

		uniforms[i++] = centerX - PARTICLE_SIZE;
		uniforms[i++] = centerY - PARTICLE_SIZE;
		uniforms[i++] = 0.f;
		uniforms[i++] = r;
		uniforms[i++] = g;
		uniforms[i++] = b;
		uniforms[i++] = 1.f;
		uniforms[i++] = xVelocity;
		uniforms[i++] = yVelocity;
		uniforms[i++] = 0.f;
		uniforms[i++] = 0.f;
		uniforms[i++] = 1.f;

		uniforms[i++] = centerX + PARTICLE_SIZE;
		uniforms[i++] = centerY + PARTICLE_SIZE;
		uniforms[i++] = 0.f;
		uniforms[i++] = r;
		uniforms[i++] = g;
		uniforms[i++] = b;
		uniforms[i++] = 1.f;
		uniforms[i++] = xVelocity;
		uniforms[i++] = yVelocity;
		uniforms[i++] = 0.f;
		uniforms[i++] = 1.f;
		uniforms[i++] = 0.f;


		uniforms[i++] = centerX + PARTICLE_SIZE;
		uniforms[i++] = centerY + PARTICLE_SIZE;
		uniforms[i++] = 0.f;
		uniforms[i++] = r;
		uniforms[i++] = g;
		uniforms[i++] = b;
		uniforms[i++] = 1.f;
		uniforms[i++] = xVelocity;
		uniforms[i++] = yVelocity;
		uniforms[i++] = 0.f;
		uniforms[i++] = 1.f;
		uniforms[i++] = 0.f;

		uniforms[i++] = centerX - PARTICLE_SIZE;
		uniforms[i++] = centerY - PARTICLE_SIZE;
		uniforms[i++] = 0.f;
		uniforms[i++] = r;
		uniforms[i++] = g;
		uniforms[i++] = b;
		uniforms[i++] = 1.f;
		uniforms[i++] = xVelocity;
		uniforms[i++] = yVelocity;
		uniforms[i++] = 0.f;
		uniforms[i++] = 0.f;
		uniforms[i++] = 1.f;

		uniforms[i++] = centerX + PARTICLE_SIZE;
		uniforms[i++] = centerY - PARTICLE_SIZE;
		uniforms[i++] = 0.f;
		uniforms[i++] = r;
		uniforms[i++] = g;
		uniforms[i++] = b;
		uniforms[i++] = 1.f;
		uniforms[i++] = xVelocity;
		uniforms[i++] = yVelocity;
		uniforms[i++] = 0.f;
		uniforms[i++] = 1.f;
		uniforms[i++] = 1.f;
	}

	glGenBuffers(1, &m_uniformVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_uniformVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * MAX_UNIFORM_COUNT, uniforms, GL_STATIC_DRAW);

	delete[] uniforms;


	float* velocities = new float[MAX_FLOAT];

	for (int i = 0; i < MAX_FLOAT;) {
		xVelocity = random_velocity_x(dre);
		yVelocity = random_velocity_y(dre);
		velocities[i++] = xVelocity;
		velocities[i++] = yVelocity;
		velocities[i++] = 0.f;
		velocities[i++] = xVelocity;
		velocities[i++] = yVelocity;
		velocities[i++] = 0.f;
		velocities[i++] = xVelocity;
		velocities[i++] = yVelocity;
		velocities[i++] = 0.f;

		velocities[i++] = xVelocity;
		velocities[i++] = yVelocity;
		velocities[i++] = 0.f;
		velocities[i++] = xVelocity;
		velocities[i++] = yVelocity;
		velocities[i++] = 0.f;
		velocities[i++] = xVelocity;
		velocities[i++] = yVelocity;
		velocities[i++] = 0.f;
	}

	glGenBuffers(1, &m_velocityVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_velocityVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * MAX_FLOAT, velocities, GL_STATIC_DRAW);

	delete[] velocities;


	float* times = new float[m_particleVerticesCount];

	float time{};
	for (int i = 0; i < m_particleVerticesCount;) {
		time = random_emit_time(dre);

		times[i++] = time;
		times[i++] = time;
		times[i++] = time;

		times[i++] = time;
		times[i++] = time;
		times[i++] = time;
	}

	glGenBuffers(1, &m_emitTimeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_emitTimeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_particleVerticesCount, times, GL_STATIC_DRAW);

	for (int i = 0; i < m_particleVerticesCount;) {
		time = random_life_time(dre);

		times[i++] = time;
		times[i++] = time;
		times[i++] = time;

		times[i++] = time;
		times[i++] = time;
		times[i++] = time;
	}

	glGenBuffers(1, &m_lifeTimeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_lifeTimeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_particleVerticesCount, times, GL_STATIC_DRAW);

	delete[] times;

	
	float* periods = new float[m_particleVerticesCount];

	float period{};
	for (int i = 0; i < m_particleVerticesCount;) {
		period = random_period(dre);

		periods[i++] = period;
		periods[i++] = period;
		periods[i++] = period;

		periods[i++] = period;
		periods[i++] = period;
		periods[i++] = period;
	}

	glGenBuffers(1, &m_periodVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_periodVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_particleVerticesCount, periods, GL_STATIC_DRAW);

	delete[] periods;


	float* amps = new float[m_particleVerticesCount];

	float amp{};
	for (int i = 0; i < m_particleVerticesCount;) {
		amp = random_amp(dre);

		amps[i++] = amp;
		amps[i++] = amp;
		amps[i++] = amp;

		amps[i++] = amp;
		amps[i++] = amp;
		amps[i++] = amp;
	}

	glGenBuffers(1, &m_ampVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ampVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_particleVerticesCount, amps, GL_STATIC_DRAW);
	
	delete[] amps;


	float* values = new float[m_particleVerticesCount];

	float value{};
	for (int i = 0; i < m_particleVerticesCount;) {
		value = random_angle(dre);

		values[i++] = value;
		values[i++] = value;
		values[i++] = value;
			 		  
		values[i++] = value;
		values[i++] = value;
		values[i++] = value;
	}

	glGenBuffers(1, &m_valueVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_valueVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_particleVerticesCount, values, GL_STATIC_DRAW);

	delete[] values;
}

void Renderer::CreateFragmentSandbox()
{
	// position				uv
	float rect[] =
	{
		-1.f, 1.f, 0.f,		 0.f, 0.f,
		-1.f, -1.f, 0.f,	 0.f, 1.f,
		1.f, 1.f, 0.f,		 1.f, 0.f,
		1.f, 1.f, 0.f,		 1.f, 0.f,
		-1.f, -1.f, 0.f,	 0.f, 1.f,
		1.f, -1.f, 0.f,		 1.f, 1.f
	};

	glGenBuffers(1, &m_fragmentSandboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_fragmentSandboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);

}

void Renderer::CreateAlphaClear()
{
	float rect[] =
	{
		-1.f, -1.f, 0.f,
		-1.f, 1.f, 0.f,	
		1.f, 1.f, 0.f,	
		-1.f, -1.f, 0.f,
		1.f, 1.f, 0.f,	
		1.f, -1.f, 0.f
	};

	glGenBuffers(1, &m_alphaClearVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_alphaClearVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);
}

void Renderer::CreateVertexSandbox()
{
	m_horiLineVertexCount = 30;
	int floatCount = m_horiLineVertexCount * 3;
	int index = 0;
	float gap = 2.f / ((float)m_horiLineVertexCount - 1.f);
	float* lineVertices = new float[floatCount];
	for (int i = 0; i < m_horiLineVertexCount; ++i) {
		lineVertices[index] = -1.f + (float)i * gap; index++;
		lineVertices[index] = 0.f; index++;
		lineVertices[index] = 0.f; index++;
	}

	glGenBuffers(1, &m_horiLineVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_horiLineVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * floatCount, lineVertices, GL_STATIC_DRAW);

	delete[] lineVertices;
}

void Renderer::CreateTextureSandbox()
{
	float textureRect[]
	{
		-0.5f, 0.5f, 0.f,	 0.f, 0.f,
		-0.5f, -0.5f, 0.f,	 0.f, 1.f,
		0.5f, 0.5f, 0.f,	 1.f, 0.f,
		0.5f, 0.5f, 0.f,	 1.f, 0.f,
		-0.5f, -0.5f, 0.f,	 0.f, 1.f,
		0.5f, -0.5f, 0.f,	 1.f, 1.f
	};

	glGenBuffers(1, &m_textureSandboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_textureSandboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureRect), textureRect, GL_STATIC_DRAW);
}

void Renderer::CreateTextures()
{
	GLulong checkerboard[] =
	{
	0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
	0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
	0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
	0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
	0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
	0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
	0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
	0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF
	};

	glGenTextures(1, &m_checkerBoardTexture);
	glBindTexture(GL_TEXTURE_2D, m_checkerBoardTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, checkerboard);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

GLuint Renderer::CreatePngTexture(char* filePath, GLuint samplingMethod)
{
	std::vector<unsigned char> image;
	unsigned width{}, height{};
	unsigned error = lodepng::decode(image, width, height, filePath);
	if (error != 0) {
		std::cout << "image load Error : " << filePath << std::endl;
		assert(0);
	}

	GLuint temp{};
	glGenTextures(1, &temp);
	glBindTexture(GL_TEXTURE_2D, temp);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, &image[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, samplingMethod);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, samplingMethod);

	return temp;
}

void Renderer::CreateGridMesh()
{
	float basePosX = -0.5f;
	float basePosY = -0.5f;
	float targetPosX = 0.5f;
	float targetPosY = 0.5f;

	int pointCountX = 24;
	int pointCountY = 24;

	float width = targetPosX - basePosX;
	float height = targetPosY - basePosY;

	float* point = new float[pointCountX * pointCountY * 2];
	float* vertices = new float[(pointCountX - 1) * (pointCountY - 1) * 2 * 3 * 3];
	m_gridMeshVertexCount = (pointCountX - 1) * (pointCountY - 1) * 2 * 3;

	//Prepare points
	for (int x = 0; x < pointCountX; x++)
	{
		for (int y = 0; y < pointCountY; y++)
		{
			point[(y * pointCountX + x) * 2 + 0] = basePosX + width * (x / (float)(pointCountX - 1));
			point[(y * pointCountX + x) * 2 + 1] = basePosY + height * (y / (float)(pointCountY - 1));
		}
	}

	//Make triangles
	int vertIndex = 0;
	for (int x = 0; x < pointCountX - 1; x++)
	{
		for (int y = 0; y < pointCountY - 1; y++)
		{
			//Triangle part 1
			vertices[vertIndex] = point[(y * pointCountX + x) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[(y * pointCountX + x) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
			vertices[vertIndex] = point[((y + 1) * pointCountX + (x + 1)) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[((y + 1) * pointCountX + (x + 1)) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
			vertices[vertIndex] = point[((y + 1) * pointCountX + x) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[((y + 1) * pointCountX + x) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;

			//Triangle part 2
			vertices[vertIndex] = point[(y * pointCountX + x) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[(y * pointCountX + x) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
			vertices[vertIndex] = point[(y * pointCountX + (x + 1)) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[(y * pointCountX + (x + 1)) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
			vertices[vertIndex] = point[((y + 1) * pointCountX + (x + 1)) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[((y + 1) * pointCountX + (x + 1)) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
		}
	}

	glGenBuffers(1, &m_gridMeshVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_gridMeshVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (pointCountX - 1) * (pointCountY - 1) * 2 * 3 * 3, vertices, GL_STATIC_DRAW);

	delete[] vertices;
	delete[] point;
}

void Renderer::CreateFBO()
{
	for (int i = 0; i < m_fboTextures.size(); ++i) {
		glGenTextures(1, &m_fboTextures[i]);
		glBindTexture(GL_TEXTURE_2D, m_fboTextures[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_WindowSizeX, m_WindowSizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		// 해상도는 512 * 512
		// 마지막 인자를 0으로 하여 메모리 공간을 할당하지만 채우진 않음
		
		glGenTextures(1, &m_fboAttachTextures[i]);
		glBindTexture(GL_TEXTURE_2D, m_fboAttachTextures[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_WindowSizeX, m_WindowSizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	}

	// depth 는 texture / render buffer 중 하나로 사용할 수 있음
	// render buffer 로 사용하는 것이 상대적으로 빠름
	glGenRenderbuffers(1, &m_depthRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_WindowSizeX, m_WindowSizeY);	// DEPTH_COMPONENT 로 사용
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);	// 프레임 버퍼를 가져옴
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
		m_fboTextures[0], 0);
	for (auto& fboAttach : m_fboAttachTextures) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
			fboAttach, 0);
	}
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER, m_depthRenderBuffer);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "FBO Create Fail" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);		// 끝난 뒤 프레임 버퍼를 돌려줌
}

void Renderer::CreateDrawTextureVBO()
{
	float sizeX = 1.f / m_WindowSizeX;
	float sizeY = 1.f / m_WindowSizeY;

	float drawTextureRect[]{
		-sizeX,  sizeY, 0.f,		0.f, 0.f,
		-sizeX, -sizeY, 0.f,		0.f, 1.f,
		 sizeX,  sizeY, 0.f,		1.f, 0.f,
		 sizeX,  sizeY, 0.f,		1.f, 0.f,
		-sizeX, -sizeY, 0.f,		0.f, 1.f,
		 sizeX, -sizeY, 0.f,		1.f, 1.f
	};

	glGenBuffers(1, &m_drawTextureVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_drawTextureVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(drawTextureRect), drawTextureRect, GL_STATIC_DRAW);
}