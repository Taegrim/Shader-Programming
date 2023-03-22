#include "stdafx.h"
#include "Renderer.h"
#include <random>

constexpr int PARTICLE_NUM = 10000;
constexpr float PARTICLE_SIZE = 0.003f;
constexpr float PARTICLE_MAX_SPEED = 0.5f;

std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_real_distribution<float> urd((-1.f + PARTICLE_SIZE), (1.f - PARTICLE_SIZE));
std::uniform_real_distribution<float> random_velocity_x(-PARTICLE_MAX_SPEED, PARTICLE_MAX_SPEED);
std::uniform_real_distribution<float> random_velocity_y(PARTICLE_MAX_SPEED, PARTICLE_MAX_SPEED + 0.5f);

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
	m_SolidRectShader = CompileShaders("./Shaders/SolidRect.vs", "./Shaders/SolidRect.fs");
	m_particleShader = CompileShaders("./Shaders/Particle.vs", "./Shaders/Particle.fs");
	
	//Create VBOs
	CreateVertexBufferObjects();
	CreateParticles(PARTICLE_NUM);

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
	std::cout << filenameVS << ", " << filenameFS << " Shader compiling is done.";

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
	if (m_time > 4.f)
		m_time = 0.f;
}

void Renderer::DrawParticleEffect()
{
	int shader_program = m_particleShader;
	glUseProgram(shader_program);

	int attribLocation = glGetAttribLocation(shader_program, "a_Position");
	glEnableVertexAttribArray(attribLocation);
	glBindBuffer(GL_ARRAY_BUFFER, m_particleVBO);
	glVertexAttribPointer(attribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	attribLocation = glGetAttribLocation(shader_program, "a_Velocity");
	glEnableVertexAttribArray(attribLocation);
	glBindBuffer(GL_ARRAY_BUFFER, m_velocityVBO);
	glVertexAttribPointer(attribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	int uniformLocation = glGetUniformLocation(shader_program, "u_Time");

	glUniform1f(uniformLocation, m_time);

	glDrawArrays(GL_TRIANGLES, 0, m_particleVerticesCount);
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
		/*centerX = urd(dre);
		centerY = urd(dre);*/
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


	float* velocities = new float[MAX_FLOAT];

	float xVelocity{};
	float yVelocity{};

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

	delete[] vertices;
	delete[] velocities;
}
