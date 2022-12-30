#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <time.h>

#include "input.h"
#include "vertex_array.h"
#include "shader.h"
#include "texture.h"
#include "time.h"
#include "camera.h"
#include "plane.h"
#include "render_state.h"
#include "cube_list.h"

#define global static

global GLFWwindow* GLFWDisplay;
global Camera Cam;
global CubeList Cubes;
global std::shared_ptr<Texture2D> CubeTexture = nullptr;
global std::shared_ptr<Texture2D> Ak47Texture = nullptr;
global std::shared_ptr<Texture2D> BulletTexture = nullptr;
global std::shared_ptr<Texture2D> HeartTexture = nullptr;
global float CubeSpawnTimer = 0.0f;
global float CubeSpawnTickSpeed = 1.5f;
global float CubeSpeed = 2.0f;
global uint32_t PlayerPoints = 0;
global uint32_t PlayerHealth = 3;
global bool Suspended = false;
global float LastCubeSpawnX = -1.0f;
global uint32_t MaxCubes = 13;
global Cube* Player = nullptr;
global bool hasAk47 = false;
global float ak47Timer = 0.0f;
global float ak47MaxTime = 10.0f;
global float ak47ShootTimer = 0.1f;
global float ak47ShootCooldown = 0.1f;
global bool ak47InCubes = false;
global Cube ak47InHand;

global std::vector<Cube*> Bullets = {};

bool InitWindow(uint32_t width, uint32_t height, const std::string& title)
{
	// Initializing GLFW
	if (!glfwInit()) return false;

	// Creating the window
	GLFWDisplay = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

	std::cout << "Created GLFW Window: '" << title << "' (" << width << "x" << height << ")\n";

	if (!GLFWDisplay) return false;

	glfwSwapInterval(1); // Vsync
	// Making OpenGL context
	glfwMakeContextCurrent(GLFWDisplay);

	// Setting up callbacks
	glfwSetKeyCallback(GLFWDisplay, Input::glfwKeyCallback);
	glfwSetMouseButtonCallback(GLFWDisplay, Input::glfwButtonCallback);
	glfwSetScrollCallback(GLFWDisplay, Input::glfwScrollCallback);
	glfwSetCursorPosCallback(GLFWDisplay, Input::glfwCursorCallback);
	glfwSetFramebufferSizeCallback(GLFWDisplay, [](GLFWwindow* window, int width,
		int height) {
			glViewport(0, 0, width, height);
		});

	// Initializing glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return false;

	std::cout << "Initialized OpenGL version " << glGetString(GL_VERSION) << "\n";


	return true;
}

void TerminateWindow()
{
	glfwDestroyWindow(GLFWDisplay);
	glfwTerminate();
}

void UpdateInput()
{
	if (Input::keyWentDown(GLFW_KEY_A) && Player->Position.x <= 4.0f)
	{;
		Cam.UpdatePosition(CameraDirection::Left);
	}
	if (Input::keyWentDown(GLFW_KEY_D) && Player->Position.x >= 0.0f)
	{
		Cam.UpdatePosition(CameraDirection::Right);
	}
}

void UpdateCubes()
{
	CubeSpawnTimer += Time::GetDeltaTime();
	if (CubeSpawnTimer >= CubeSpawnTickSpeed)
	{
		if (Cubes.Cubes.size() <= MaxCubes)
		{
			if (LastCubeSpawnX == -1.0f)
			{
				float x_index = rand() % 5;
				Cubes.AddCube(new Cube(glm::vec3(x_index, 0.0f, 13.0f), glm::vec3(1.0f), CubeTexture));

				bool addedCollectable = false;
				if (rand() %  10 == 2)
				{
					Cubes.AddCube(new Cube(glm::vec3(x_index, 0.0f, 14.5f),
						glm::vec3(0.5f), HeartTexture, "Heart"));
					addedCollectable = true;
				 }
				if (!addedCollectable && !hasAk47 && !ak47InCubes)
				{
					if (rand() % 45 == 2)
					{
						Cubes.AddCube(new Cube(glm::vec3(x_index, 0.0f, 14.5f),
							glm::vec3(0.5f), Ak47Texture, "Ak47"));
						addedCollectable = true;
						ak47InCubes = true;
					}
				}
				LastCubeSpawnX = x_index;
			}
			else
			{
				float x_index = rand() % 5;
				while (x_index == LastCubeSpawnX)
				{
					x_index = rand() % 5;
				}
				Cubes.AddCube(new Cube(glm::vec3(x_index, 0.0f, 13.0f), glm::vec3(1.0f), CubeTexture));

				bool addedCollectable = false;
				if (rand() % 10 == 2)
				{
					Cubes.AddCube(new Cube(glm::vec3(x_index, 0.0f, 14.5f),
						glm::vec3(0.5f), HeartTexture, "Heart"));
					addedCollectable = true;
				}
				if (!addedCollectable && !hasAk47 && !ak47InCubes)
				{
					if (rand() % 45 == 2)
					{
						Cubes.AddCube(new Cube(glm::vec3(x_index, 0.0f, 14.5f),
							glm::vec3(0.5f), Ak47Texture, "Ak47"));
						addedCollectable = true;
						ak47InCubes = true;
					}
				}
				LastCubeSpawnX = x_index;
			}
			CubeSpawnTimer = 0.0f;
		}
	}
	for (auto& cube : Cubes.Cubes)
	{
		if(cube->GetName() != "Player")
			cube->Position.z -= CubeSpeed * Time::GetDeltaTime();
		
		if (cube->ColldingWithCube(*Player))
		{
			if (cube->GetName() == "Cube")
			{
				PlayerHealth--;
				std::cout << "Player Health: " << PlayerHealth << "\n";
				Cubes.RemoveCube(cube);
				continue;
			}
			else if (cube->GetName() == "Heart")
			{
				PlayerHealth++;
				std::cout << "Player Health: " << PlayerHealth << "\n";
				Cubes.RemoveCube(cube);
				continue;
			}
			else if (cube->GetName() == "Ak47")
			{
				hasAk47 = true;			
				ak47InCubes = false;
				Cubes.RemoveCube(cube);
			}
		}
		if (cube->Position.z <= -3.0f)
		{
			if (cube->GetName() == "Cube")
			{

				PlayerPoints++;
				if (PlayerPoints % 15 == 0)
				{
					if (MaxCubes > 5)
					{
						MaxCubes--;
					}
					if (CubeSpawnTickSpeed >= 0.5f)
					{
						CubeSpawnTickSpeed -= 0.25f;
					}
					if (CubeSpeed <= 12.5)
					{
						CubeSpeed += 0.75f;
					}
				}
				std::cout << "Player Points: " << PlayerPoints << "\n";
				Cubes.RemoveCube(cube);
				continue;
			}
			else if (cube->GetName() == "Heart")
			{
				Cubes.RemoveCube(cube);
				continue;
			}
			else if (cube->GetName() == "Ak47")
			{
				Cubes.RemoveCube(cube);
				ak47InCubes = false;
				continue;
			}

			
		}
	}
}

void UpdateBullets()
{
	if (hasAk47)
	{
		if (ak47InHand.Position != glm::vec3(Player->Position.x - 0.5f, Player->Position.y, Player->Position.z))
		{
			ak47InHand.Position = glm::vec3(Player->Position.x - 0.5f, Player->Position.y, Player->Position.z);
		}
		ak47Timer += Time::GetDeltaTime();
		if (ak47Timer >= ak47MaxTime)
		{
			hasAk47 = false;
			ak47Timer = 0.0f;
			ak47InHand.Position = glm::vec3(-20.0f, 0.0f, 0.0f);
		}
		if (Input::isKeyDown(GLFW_KEY_SPACE))
		{
			ak47ShootTimer += Time::GetDeltaTime();
			if (ak47ShootTimer > ak47ShootCooldown)
			{
				Bullets.push_back(new Cube(glm::vec3(Player->Position.x, Player->Position.y, Player->Position.z + 0.5f),
					glm::vec3(0.1f, 0.1f, 0.2f), BulletTexture, "Bullet"));
				ak47ShootTimer = 0.0f;
			}
		}
	}
	for (auto& bullet : Bullets)
	{
		bullet->Position.z += 10.0f * Time::GetDeltaTime();
	}
	for (auto& cube : Cubes.Cubes)
	{
		for (auto& bullet : Bullets)
		{
			if (cube->ColldingWithCube(*bullet))
			{
				auto it = std::find(Bullets.begin(), Bullets.end(), bullet);
				if (it != Bullets.end())
					Bullets.erase(it);
				Cubes.RemoveCube(cube);
			}
		}
	}
		
}
int main()
{
	srand(time(0));

	if (!InitWindow(1280, 720, "3D Game")) return EXIT_FAILURE;
	RenderState::Init();

	Cam = Camera(glm::vec3(2.0f, 3.0f, -4.0f));
	Cubes = CubeList();

	std::shared_ptr<Texture2D> playerTexture = std::make_shared<Texture2D>("assets/textures/grass.jpg");

	Player = new Cube(glm::vec3(2.0f, 0.0f, 1.0f), glm::vec3(0.5f, 1.0f, 0.5f), playerTexture, "Player");

	CubeTexture = std::make_shared<Texture2D>("assets/textures/brick.jpg");
	HeartTexture = std::make_shared<Texture2D>("assets/textures/heart.png");
	BulletTexture = std::make_shared<Texture2D>("assets/textures/lava.jpg");
	Ak47Texture = std::make_shared<Texture2D>("assets/textures/ak47.png");
	
	std::shared_ptr<Texture2D> groundTexture = std::make_shared<Texture2D>(
		"assets/textures/ground.png"
		);

	RenderState::GetShader()->Bind();
	RenderState::GetShader()->UploadVec4("uFogColor", glm::vec4(0.2f, 0.3f, 0.8f, 1.0f));

	Cube ground = Cube(glm::vec3(0.0f, -7.5f, 10.0f), glm::vec3(30.0f, 10.0f, 30.0f), groundTexture);
	ak47InHand = Cube(glm::vec3(-20.0f, 0.0f, 0.0f), glm::vec3(0.5f), Ak47Texture);

	hasAk47 = false;

	while (!glfwWindowShouldClose(GLFWDisplay))
	{
		if (PlayerHealth <= 0)
		{
			if (!Cubes.Cubes.empty()) 
			{
				Cubes.Cubes.clear();
				PlayerPoints = 0;
			}
				
			Suspended = true;
			if (Input::isKeyDown(GLFW_KEY_SPACE)) {
				Suspended = false;
				PlayerHealth = 3;
				CubeSpeed = 1.5f;
				CubeSpawnTickSpeed = 1.0f;
			}
		}
		Time::Update();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.8f, 1.0f);


		RenderState::GetShader()->UploadMat4("uView", Cam.GetViewMatrix());
		glm::mat4 proj = glm::perspective(glm::radians(45.0f),
			((float)1280.0f / (float)720.0f), 0.1f, 1000.0f);
		RenderState::GetShader()->UploadMat4("uProj", proj);
		ground.Render();
		Player->Render();
		Cubes.RenderCubes();

		if (Input::keyWentDown(GLFW_KEY_A) && Player->Position.x < 4.0f)
		{
			Player->Position.x += 1.0f;
		}
		if (Input::keyWentDown(GLFW_KEY_D) && Player->Position.x > 0.0f)
		{
			Player->Position.x -= 1.0f;
		}
		if (!Suspended)
		{
			UpdateBullets();
			UpdateCubes();
			UpdateInput();
		}
		for(auto& bullet : Bullets)
			bullet->Render();

		if(ak47InHand.Position == glm::vec3(Player->Position.x - 0.5f, Player->Position.y, Player->Position.z))
			ak47InHand.Render();

		glfwPollEvents();
		glfwSwapBuffers(GLFWDisplay);
	}

	TerminateWindow();
}