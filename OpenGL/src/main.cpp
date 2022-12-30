#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <time.h>
#include <map>
#include <ft2build.h>
#include <irrKlang.h>
using namespace irrklang;

#include "input.h"
#include "vertex_array.h"
#include "shader.h"
#include "texture.h"
#include "time.h"
#include "camera.h"
#include "plane.h"
#include "render_state.h"
#include "cube_list.h"

#include FT_FREETYPE_H  

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
global float CubeSpeed = 4.0f;
global uint32_t PlayerPoints = 0;
global uint32_t PlayerHealth = 0;
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
global std::shared_ptr<Shader> FontShader;
global std::vector<Cube*> Bullets = {};
global ISoundEngine* SoundEngine = createIrrKlangDevice();


#define windowWidth 1280.0f
#define windowHeight 720.0f

unsigned int FontVAO, FontVBO;

bool InitWindow(const std::string& title)
{
	// Initializing GLFW
	if (!glfwInit()) return false;

	// Creating the window
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	GLFWDisplay = glfwCreateWindow(mode->width, mode->height, title.c_str(), glfwGetPrimaryMonitor(), nullptr);

	std::cout << "Created GLFW Window: '" << title << "' (" << mode->width << "x" << mode->height << ")\n";

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
				SoundEngine->play2D("assets/sounds/crash.mp3");
				PlayerHealth--;
				std::cout << "Player Health: " << PlayerHealth << "\n";
				Cubes.RemoveCube(cube);
				continue;
			}
			else if (cube->GetName() == "Heart")
			{
				SoundEngine->play2D("assets/sounds/health.mp3");
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
				SoundEngine->play2D("assets/sounds/coin.mp3");
				if (PlayerPoints % 15 == 0)
				{
					if (MaxCubes > 6)
					{
						MaxCubes--;
					}
					if (CubeSpawnTickSpeed >= 0.5f)
					{
						CubeSpawnTickSpeed -= 0.25f;
					}
					if (CubeSpeed <= 20)
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
				SoundEngine->play2D("assets/sounds/shoot.mp3");
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
			if (cube->GetName() == "Cube")
			{
				if (cube->ColldingWithCube(*bullet))
				{
					auto it = std::find(Bullets.begin(), Bullets.end(), bullet);
					if (it != Bullets.end())
						Bullets.erase(it);
					Cubes.RemoveCube(cube);
					PlayerPoints++;
					SoundEngine->play2D("assets/sounds/coin.mp3");
				}
			}
		}
	}
		
}
struct Character {
	unsigned int TextureID;  // ID handle of the glyph texture
	glm::ivec2   Size;       // Size of glyph
	glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
	unsigned int Advance;    // Offset to advance to next glyph
};

std::map<char, Character> Characters;

bool InitFonts()
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return false;
	}

	FT_Face face;
	if (FT_New_Face(ft, "assets/fonts/arial.ttf", 0, &face))
	{
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return false;
	}
	FT_Set_Pixel_Sizes(face, 0, 48);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

	for (unsigned char c = 0; c < 128; c++)
	{
		// load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// generate texture
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<char, Character>(c, character));
	}


	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	FontShader = std::make_shared<Shader>("assets/shaders/vertex_font.glsl", "assets/shaders/fragment_font.glsl");

	glm::mat4 projection = glm::ortho(0.0f, windowWidth, 0.0f, windowHeight);
	FontShader->Bind();
	FontShader->UploadMat4("projection", projection);

	glGenVertexArrays(1, &FontVAO);
	glGenBuffers(1, &FontVBO);
	glBindVertexArray(FontVAO);
	glBindBuffer(GL_ARRAY_BUFFER, FontVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return true;
}
void RenderText(std::string text, float x, float y, float scale, glm::vec3 color)
{
	// activate corresponding render state	
	FontShader->Bind();
	glUniform3f(glGetUniformLocation(FontShader->GetID(), "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(FontVAO);

	// iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		// update FontVBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// update content of FontVBO memory
		glBindBuffer(GL_ARRAY_BUFFER, FontVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
int main()
{
	srand(time(0));

	if (!InitWindow("3D Game")) return EXIT_FAILURE;
	if (!InitFonts()) return EXIT_FAILURE;
	RenderState::Init();

	Cam = Camera(glm::vec3(2.0f, 3.0f, -4.0f));
	Cubes = CubeList();

	std::shared_ptr<Texture2D> playerTexture = std::make_shared<Texture2D>("assets/textures/lava.jpg");

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


	SoundEngine->play2D("assets/sounds/music.mp3", true);

	while (!glfwWindowShouldClose(GLFWDisplay))
	{
		
		Time::Update();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.8f, 1.0f);

		RenderState::GetShader()->Bind();
		RenderState::GetShader()->UploadMat4("uView", Cam.GetViewMatrix());
		glm::mat4 proj = glm::perspective(glm::radians(45.0f),
			((float)windowWidth / (float)windowHeight), 0.1f, 1000.0f);
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

		std::string scoreText = "Score: " + std::to_string(PlayerPoints);
		std::string healthText = "Health: " + std::to_string(PlayerHealth);

		if (!Suspended)
		{
			RenderText(scoreText, 25, windowHeight - 50, 1.0f, glm::vec3(0.9f, 0.9f, 1.0f));
			RenderText(healthText, windowWidth - 225, windowHeight - 50, 1.0f, glm::vec3(0.9f, 0.2f, 0.3f));
		}
		if (Suspended)
		{
			RenderText("Press Space to start", windowWidth / 2.0f - 225.0f, windowHeight /  2.0f, 1.0f, glm::vec3(0.9f, 0.9f, 1.0f));
		}
		FontShader->Unbind();

		glfwPollEvents();
		glfwSwapBuffers(GLFWDisplay); 
		if (PlayerHealth <= 0)
		{
			if (!Cubes.Cubes.empty())
			{
				Cubes.Cubes.clear();
				PlayerPoints = 0;
				SoundEngine->play2D("assets/sounds/death.mp3");
				Player->Position = glm::vec3(2.0f, 0.0f, 1.0f);
			}

			Suspended = true;
			if (Input::isKeyDown(GLFW_KEY_SPACE)) {
				SoundEngine->play2D("assets/sounds/start.mp3");
				Suspended = false;
				PlayerHealth = 3;
				CubeSpeed = 4.0f;
				CubeSpawnTickSpeed = 1.0f;
			}
		}
	}

	TerminateWindow();
}