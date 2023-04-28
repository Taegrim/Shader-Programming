/*
Copyright 2022 Lee Taek Hee (Tech University of Korea)

This program is free software: you can redistribute it and/or modify
it under the terms of the What The Hell License. Do it plz.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY.
*/

#include "stdafx.h"
#include <iostream>
#include <chrono>
#include "Dependencies\glew.h"
#include "Dependencies\freeglut.h"

#include "Renderer.h"
#include "LoadPng.h"

Renderer *g_Renderer = NULL;
std::chrono::time_point<std::chrono::system_clock> g_last_time;
constexpr auto frame_time = std::chrono::duration<double>(1.0 / 60.0);

constexpr int g_windowWidth = 1024;
constexpr int g_windowHeight = 768;

using namespace std::chrono;

void RenderScene(void)
{

	auto now = system_clock::now(); 
	auto exec = duration_cast<std::chrono::duration<double>>(now - g_last_time);

	if (exec >= frame_time) {
		//g_Renderer->DrawAlphaClear();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 0.f);

		// Renderer Test
		//g_Renderer->DrawSolidRect(0, 0, 0, 4, 1, 0, 1, 1);
		//g_Renderer->DrawParticleEffect();
		g_Renderer->DrawFragmentSandbox();
		//g_Renderer->DrawVertexSandbox();
		//g_Renderer->DrawTextureSandbox();

		g_Renderer->Update(exec.count());

		g_last_time = now;
		glutSwapBuffers();
	}
}

void Idle(void)
{
	RenderScene();
}

void MouseInput(int button, int state, int x, int y)
{
	RenderScene();
}

void KeyInput(unsigned char key, int x, int y)
{
	RenderScene();
}

void SpecialKeyInput(int key, int x, int y)
{
	RenderScene();
}

int main(int argc, char **argv)
{
	// Initialize GL things
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(g_windowWidth, g_windowHeight);
	glutCreateWindow("Game Software Engineering KPU");

	g_last_time = std::chrono::system_clock::now();

	glewInit();
	if (glewIsSupported("GL_VERSION_3_0"))
	{
		std::cout << " GLEW Version is 3.0\n ";
	}
	else
	{
		std::cout << "GLEW 3.0 not supported\n ";
	}

	// Initialize Renderer
	g_Renderer = new Renderer(g_windowWidth, g_windowHeight);
	if (!g_Renderer->IsInitialized())
	{
		std::cout << "Renderer could not be initialized.. \n";
	}

	glutDisplayFunc(RenderScene);
	glutIdleFunc(Idle);
	glutKeyboardFunc(KeyInput);
	glutMouseFunc(MouseInput);
	glutSpecialFunc(SpecialKeyInput);

	glutMainLoop();

	delete g_Renderer;

    return 0;
}

