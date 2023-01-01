#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <time.h>
#include <map>
#include <ft2build.h>
#include <irrKlang.h>
#include <Windows.h>
using namespace irrklang;

#include "input.h"
#include "vertex_array.h"
#include "shader.h"
#include "texture.h"
#include "time.h"
#include "camera.h"
#include "render_state.h"
#include "cube_list.h"
#include "model.h"

#include <functional>

#include FT_FREETYPE_H  

#define global static

global GLFWwindow* GLFWDisplay;
global Camera Cam;
global CubeList Cubes;
global float CubeSpawnTimer = 0.0f;
global float CubeSpawnTickSpeed = 1.5f;
global float CubeSpeed = 4.0f;
global uint32_t PlayerPoints = 0;
global int32_t PlayerHealth = 0;
global bool Suspended = false;
global float LastCubeSpawnX = -1.0f;
global uint32_t MaxCubes = 11;
global bool hasAk47 = false;
global float ak47Timer = 0.0f;
global float ak47MaxTime = 10.0f;
global float ak47ShootTimer = 0.1f;
global float ak47ShootCooldown = 0.1f;
global bool ak47InCubes = false;
global bool hasSword = false;
global bool swordInCubes = false;
global float swordTimer = 0.0f;
global float swordMaxTime = 7.0f;
global bool hasShield = false;
global bool shieldInCubes = false;
global Cube ak47InHand;
global Cube swordInHand;
global Cube shieldInHand;
global std::shared_ptr<Shader> FontShader;
global std::vector<Cube*> Bullets = {};
global ISoundEngine* SoundEngine = createIrrKlangDevice();
global std::shared_ptr<Model> playerModel = nullptr;
global std::shared_ptr<Model> astroidModel = nullptr;
global std::shared_ptr<Model> heartModel = nullptr;
global std::shared_ptr<Model> groundModel = nullptr;
global std::shared_ptr<Model> ak47Model = nullptr;
global std::shared_ptr<Model> skyModel = nullptr;
global std::shared_ptr<Model> spaceShuttleModel = nullptr;
global std::shared_ptr<Model> swordModel = nullptr;
global std::shared_ptr<Model> shieldModel = nullptr;
global float cubeSpeedBeforeItWasChangedBySword;
global int32_t ShieldHealth = 0;

#define windowWidth 1280.0f
#define windowHeight 720.0f

unsigned int FontVAO, FontVBO;

class Item
{
public:
	Item() = default;

	Item(const std::shared_ptr<Model>& model, const glm::vec3& scale,
		const std::string& name, const glm::vec3& rotation = glm::vec3(0.0f),
		const glm::vec3& hitBox = glm::vec3(-1.0f))
		: mModel(model), Scale(scale), Name(name), mCollected(false), Rotation(rotation)
	{
		if (hitBox != glm::vec3(-1.0f))
		{
			Hitbox = hitBox;
		}
		else
		{
			Hitbox = scale;
		}
	}
	std::function<void()> OnCollected;

	inline const std::shared_ptr<Model>& GetModel() const
	{
		return mModel;
	}

	inline bool IsCollected() const
	{
		return mCollected;
	}
	inline void SetCollected(bool collected)
	{
		mCollected = collected;
	}

	void Instantiate(const glm::vec3& position)
	{
		Cubes.AddCube(new Cube(position, Scale, mModel, Name, Rotation, Hitbox / 2.0f));
	}

	glm::vec3 Scale, Rotation, Hitbox;
	std::string Name;
private:
	std::shared_ptr<Model> mModel;
	bool mCollected;
};

class Player
{
public:
	Player(const std::shared_ptr<Model>& model, const glm::vec3& position,
		const glm::vec3& scale, const glm::vec3& hitBox, const glm::vec3& rotation)
		: mModel(model), Position(position), Scale(scale), HitBox(hitBox), Rotation(rotation)
	{
		CubeInstance = new Cube(position, scale, model, "Player", rotation, hitBox / 2.0f);
	}

	void UpdateItems()
	{
		for (auto& cube : Cubes.Cubes)
		{
			if (cube->GetName() != "Player")
			{
				cube->Position -= CubeSpeed * Time::GetDeltaTime();
			}
			for (auto& item : mItems)
			{
				if (cube->GetName() == item.Name)
				{
					if (item.IsCollected() == false)
					{
						item.SetCollected(true);
						item.OnCollected();
					}
				}
			}
		}
	}
	
	void AddCollectableItem(const Item& item)
	{
		mItems.push_back(item);
	}
	void Render()
	{
		CubeInstance->Render();
	}

	void UpdateInput()
	{
		if (Input::keyWentDown(GLFW_KEY_A) && Position.x <= 4.0f)
		{
			Cam.UpdatePosition(CameraDirection::Left);
		}
		if (Input::keyWentDown(GLFW_KEY_D) && Position.x >= 0.0f)
		{
			Cam.UpdatePosition(CameraDirection::Right);
		}
	}
	glm::vec3 Scale, Position, HitBox, Rotation;
	Cube* CubeInstance;
private:
	std::vector<Item> mItems;
	std::shared_ptr<Model> mModel;
};

global Player* player = nullptr;

bool InitWindow(const std::string& title)
{
	// Initializing GLFW
	if (!glfwInit()) return false;

	// Creating the window
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	GLFWDisplay = glfwCreateWindow(mode->width, mode->height, title.c_str(), nullptr, nullptr);

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
				if (rand() % 20 == 2)
				{
					Cubes.AddCube(new Cube(glm::vec3(x_index, 0.0f, 13.0f), glm::vec3(0.25), astroidModel, "StrongCube", glm::vec3(0.0f),
						glm::vec3(0.28f, 0.28f, 0.55f)));
				}
				else
				{
					Cubes.AddCube(new Cube(glm::vec3(x_index, 0.0f, 13.0f), glm::vec3(0.25), astroidModel, "Cube", glm::vec3(0.0f),
						glm::vec3(0.28f, 0.28f, 0.55f)));
				}
			
				bool addedCollectable = false;
				if (rand() %  10 == 2)
				{
					Cubes.AddCube(new Cube(glm::vec3(x_index, 0.0f, 14.5f),
						glm::vec3(0.07f), heartModel, "Heart", glm::vec3(90.0f, 180.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.4f)));
					addedCollectable = true;
					
				}
				if (!addedCollectable && !hasAk47 && !ak47InCubes && !hasSword)
				{
					if (rand() % 45 == 2)
					{
						Cubes.AddCube(new Cube(glm::vec3(x_index, 0.0f, 15.5f),
							glm::vec3(0.1f), ak47Model, "Ak47"));
						addedCollectable = true;
					}
				}
				if (!addedCollectable && !swordInCubes && !hasSword && !hasAk47)
				{
					if (rand() % 60 == 1)
					{
						Cubes.AddCube(new Cube(glm::vec3(x_index, 0.0f, 15.5f),
							glm::vec3(0.008f), swordModel, "Sword", glm::vec3(0.0f), glm::vec3(0.008f / 2.0f, 0.008f / 2.0f, 0.008f)));
						addedCollectable = true;
					}
				}
				if (!addedCollectable && !shieldInCubes && !hasShield && !hasAk47 && !hasSword)
				{
					if (rand() % 12 == 2)
					{
						Cubes.AddCube(new Cube(glm::vec3(x_index, 0.0f, 14.5f), glm::vec3(0.1f), shieldModel,
							"Shield", glm::vec3(90.0f, 90.0f, 180.0f)));
						addedCollectable = true;
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
				if (rand() % 20 == 2)
				{
					Cubes.AddCube(new Cube(glm::vec3(x_index, 0.0f, 13.0f), glm::vec3(0.25), astroidModel, "StrongCube", glm::vec3(0.0f),
						glm::vec3(0.28f, 0.28f, 0.55f)));
				}
				else
				{
					Cubes.AddCube(new Cube(glm::vec3(x_index, 0.0f, 13.0f), glm::vec3(0.25), astroidModel, "Cube", glm::vec3(0.0f),
						glm::vec3(0.28f, 0.28f, 0.55f)));
				}
				bool addedCollectable = false;
				if (rand() % 10 == 2)
				{
					Cubes.AddCube(new Cube(glm::vec3(x_index, 0.0f, 14.5f),
						glm::vec3(0.07f), heartModel, "Heart", glm::vec3(90.0f, 180.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.4f)));
					addedCollectable = true;
					
					
				}
				if (!addedCollectable && !hasAk47 && !ak47InCubes && !hasSword && !hasShield)
				{
					if (rand() % 45 == 2)
					{
						Cubes.AddCube(new Cube(glm::vec3(x_index, 0.0f, 15.5f),
							glm::vec3(0.1f), ak47Model, "Ak47"));
						addedCollectable = true;
						ak47InCubes = true;
					}
				}
				if (!addedCollectable && !swordInCubes && !hasSword && !hasAk47 && !hasShield)
				{
					if (rand() % 60 == 1)
					{
						Cubes.AddCube(new Cube(glm::vec3(x_index, 0.0f, 15.5f),
							glm::vec3(0.008f), swordModel, "Sword", glm::vec3(0.0f), glm::vec3(0.008f / 2.0f, 0.008f / 2.0f, 0.008f)));
						addedCollectable = true;
						swordInCubes = true;
					}
				}
				if (!addedCollectable && !shieldInCubes && !hasShield && !hasAk47 && !hasSword)
				{
					if (rand() % 12 == 2)
					{
						Cubes.AddCube(new Cube(glm::vec3(x_index, 0.0f, 14.5f), glm::vec3(0.1f), shieldModel,
							"Shield", glm::vec3(90.0f, 90.0f, 180.0f)));
						addedCollectable = true;
						shieldInCubes = true;
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
		
		if (hasShield)
		{
			if (cube->GetName() == "Cube" || cube->GetName() ==  "StrongCube")
			{
				if (cube->ColldingWithCube(shieldInHand))
				{
					SoundEngine->play2D("assets/sounds/crash.mp3");
					ShieldHealth--;
					Cubes.RemoveCube(cube);
					continue;
				}
			}
		}
		if (cube->ColldingWithCube(*Player))
		{
			if (cube->GetName() == "Cube" && !hasShield)
			{
				if (!hasSword)
				{
					SoundEngine->play2D("assets/sounds/crash.mp3");
					PlayerHealth--;
					Cubes.RemoveCube(cube);
					continue;
				}
			}
			if (cube->GetName() == "StrongCube" && !hasShield)
			{
				SoundEngine->play2D("assets/sounds/crash.mp3");
				PlayerHealth-=3;
				Cubes.RemoveCube(cube);
				continue;
			}
			else if (cube->GetName() == "Heart")
			{
				SoundEngine->play2D("assets/sounds/health.mp3");
				PlayerHealth++;
				Cubes.RemoveCube(cube);
				continue;
			}
			else if (cube->GetName() == "Ak47")
			{
				hasAk47 = true;			
				ak47InCubes = false;
				Cubes.RemoveCube(cube);				
				continue;
			}
			else if (cube->GetName() == "Sword")
			{
				hasSword = true;
				swordInCubes = false;
				Cubes.RemoveCube(cube);
				continue;
			}
			if (cube->GetName() == "Shield")
			{
				hasShield = true;
				shieldInCubes = false;
				Cubes.RemoveCube(cube);
				continue;
			}
		}
		if (cube->Position.z <= -3.0f)
		{
			if (cube->GetName() == "Cube" || cube->GetName() == "StrongCube")
			{
				PlayerPoints++;
				SoundEngine->play2D("assets/sounds/coin.mp3");
				if (PlayerPoints % 15 == 0)
				{
					if (MaxCubes > 7)
					{
						MaxCubes--;
					}
					if (CubeSpawnTickSpeed >= 0.5f)
					{
						CubeSpawnTickSpeed -= 0.25f;
					}
					CubeSpeed += 0.75f;
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
			ak47InHand.Rotation = glm::vec3(0.0f, 90.0f, 90.0f);
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
					glm::vec3(0.0075f, 0.01f, 0.01f), astroidModel, "Bullet"));
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
			else if (cube->GetName() == "StrongCube")
			{
				if (cube->ColldingWithCube(*bullet))
				{
					auto it = std::find(Bullets.begin(), Bullets.end(), bullet);
					if (it != Bullets.end())
						Bullets.erase(it);
					SoundEngine->play2D("assets/sounds/coin.mp3");
				}
			}
		}
	}
		
}

void UpdateSword()
{
	if (hasSword)
	{
		if (swordInHand.Position == glm::vec3(-20.0f, 0.0f, 0.0f))
		{
			cubeSpeedBeforeItWasChangedBySword = CubeSpeed;
			CubeSpeed += 4.0f;
			swordInHand.Position = glm::vec3(player->Position.x - 0.5f, Player->Position.y, Player->Position.z);
		}
		swordTimer += Time::GetDeltaTime();
		if (swordTimer >= swordMaxTime)
		{
			hasSword = false;
			swordTimer = 0.0f;
			CubeSpeed = cubeSpeedBeforeItWasChangedBySword;
			swordInHand.Position = glm::vec3(-20.0f, 0.0f, 0.0f);
		}
		for (auto& cube : Cubes.Cubes)
		{
			if (cube->ColldingWithCube(*player))
			{
				if (cube->GetName() == "Cube")
				{
					SoundEngine->play2D("assets/sounds/crash.mp3");
					PlayerPoints += 3;
					Cubes.RemoveCube(cube);
				}
			}
		}
	}
}

void UpdateShield()
{
	if (hasShield)
	{
		if (shieldInHand.Position == glm::vec3(-20.0f, 0.0f, 0.0f))
		{
			shieldInHand.Position = glm::vec3(player->Position.x, player->Position.y, player->Position.z + 1.0f);
			ShieldHealth = 3;
		}

		if (ShieldHealth <= 0)
		{
			hasShield = false;	
			shieldInHand.Position = glm::vec3(-20.0f, 0.0f, 0.0f);
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

	swordModel = std::make_shared<Model>("assets/models/Sword/scene.gltf", false);
	playerModel = std::make_shared<Model>("assets/models/Player/scene.gltf", false);
	astroidModel = std::make_shared<Model>("assets/models/Astroid/scene.gltf", false);
	heartModel = std::make_shared<Model>("assets/models/Heart/scene.gltf", false);
	groundModel = std::make_shared<Model>("assets/models/Ground/scene.gltf", false);
	ak47Model = std::make_shared<Model>("assets/models/Ak47/scene.gltf", true);
	skyModel = std::make_shared<Model>("assets/models/Sky/scene.gltf", false);
	spaceShuttleModel = std::make_shared<Model>("assets/models/Shuttle/scene.gltf", false);
	shieldModel = std::make_shared<Model>("assets/models/Shield/scene.gltf", false);

	player = new Player(spaceShuttleModel, glm::vec3(2.0f, 0.0f, 1.0f), glm::vec3(0.1f), glm::vec3(0.15, 0.15f, 0.15f * 2.0f), glm::vec3(0.0f, 90.0f, 0.0f));

	player->AddCollectableItem(Item(heartModel, glm::vec3(0.07f), "Heart", glm::vec3(90.0f, 180.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.4f)));
	player->AddCollectableItem(Item(ak47Model, glm::vec3(0.1f), "Ak47"));
	player->AddCollectableItem(Item(swordModel, glm::vec3(0.008f), "Sword", glm::vec3(0.0f), glm::vec3(0.008f / 2.0f, 0.008f / 2.0f, 0.008f)));
	player->AddCollectableItem(Item(shieldModel, glm::vec3(0.1f), "Shield", glm::vec3(90.0f, 90.0f, 180.0f)));

	Cube spaceShuttle1 = Cube(glm::vec3(-1.0f, 0.0f, 1.0f), glm::vec3(0.1f), spaceShuttleModel, "Shuttle", glm::vec3(90.0f, 90.0f, 0.0f));
	Cube spaceShuttle2 = Cube(glm::vec3(5.0f, 0.0f, 3.0f), glm::vec3(0.1f), spaceShuttleModel, "Shuttle", glm::vec3(90.0f, 90.0f, 0.0f));

	RenderState::GetShader()->Bind();
	RenderState::GetShader()->UploadVec4("uFogColor", glm::vec4(45 / 255.0f, 48 / 255.0f, 156 / 255.0f, 1.0f));
	ak47InHand = Cube(glm::vec3(-20.0f, 0.0f, 0.0f), glm::vec3(0.1f), ak47Model);
	swordInHand = Cube(glm::vec3(-20.0f, 0.0f, 0.0f),
		glm::vec3(0.008f), swordModel);
	shieldInHand = Cube(glm::vec3(-20.0f, 0.0f, 0.0f), glm::vec3(0.15f), shieldModel,
		"ShieldInHand", glm::vec3(90.0f, 90.0f, 180.0f), glm::vec3(0.15f / 2.0f, 0.15f / 2.0f, 0.45f / 2.0f));

	Cube sky = Cube(glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(1.0, 1.0, 2.0f), skyModel);

	hasAk47 = false;

	bool spaceShuttel1MovingAway = true;
	bool spaceShuttel2MovingAway = true;

	SoundEngine->play2D("assets/sounds/music.mp3", true);

	while (!glfwWindowShouldClose(GLFWDisplay))
	{
		Time::Update();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(45 / 255.0f, 48 / 255.0f, 156 / 255.0f, 1.0f);

		RenderState::GetShader()->Bind();
		RenderState::GetShader()->UploadMat4("uView", Cam.GetViewMatrix());
		glm::mat4 proj = glm::perspective(glm::radians(45.0f),
			((float)windowWidth / (float)windowHeight), 0.1f, 1000.0f);
		RenderState::GetShader()->UploadMat4("uProj", proj);
		Cubes.RenderCubes();
		sky.Render();
		player->Render();

		if (!Suspended)
		{
			player->UpdateInput();
		}
		if (!Suspended)
		{
			UpdateBullets();
			UpdateCubes();
			player->UpdateInput();
			UpdateSword();
			UpdateShield();
		}
		for(auto& bullet : Bullets)
			bullet->Render();
		spaceShuttle1.Render();
		spaceShuttle2.Render();

		if (spaceShuttel1MovingAway)
		{
			spaceShuttle1.Position.z += 1.0f * Time::GetDeltaTime();
			if (spaceShuttle1.Position.z >= 3.0f)
				spaceShuttel1MovingAway = false;
		}
		if (!spaceShuttel1MovingAway)
		{
			spaceShuttle1.Position.z -= 1.0f * Time::GetDeltaTime();
			if (spaceShuttle1.Position.z <= 0.0f)
				spaceShuttel1MovingAway = true;
		}
		if (spaceShuttel2MovingAway)
		{
			spaceShuttle2.Position.z += 1.0f * Time::GetDeltaTime();
			if (spaceShuttle2.Position.z >= 2.5f)
				spaceShuttel2MovingAway = false;
		}
		if (!spaceShuttel2MovingAway)
		{
			spaceShuttle2.Position.z -= 1.0f * Time::GetDeltaTime();
			if (spaceShuttle2.Position.z <= 0.0f)
				spaceShuttel2MovingAway = true;
		}
		if (ak47InHand.Position == glm::vec3(player->Position.x - 0.5f, player->Position.y, player->Position.z)) {
			ak47InHand.Render();
			ak47InHand.Position = glm::vec3(player->Position.x - 0.5f, player->Position.y, player->Position.z);
		}
		if (swordInHand.Position != glm::vec3(-20.0f, 0.0f, 0.0f))
		{
			swordInHand.Render();
			swordInHand.Position = glm::vec3(player->Position.x - 0.5f, player->Position.y, player->Position.z);
		}
		if (shieldInHand.Position != glm::vec3(-20.0f, 0.0f, 0.0f))
		{
			shieldInHand.Render(); 
			shieldInHand.Position = glm::vec3(player->Position.x, player->Position.y, player->Position.z + 1.0f);
		}

		std::string scoreText = "Score: " + std::to_string(PlayerPoints);
		std::string healthText = "Health: " + std::to_string(PlayerHealth);

		if (!Suspended)
		{
			RenderText(scoreText, 25, windowHeight - 50, 1.0f, glm::vec3(0.9f, 0.9f, 1.0f));
			RenderText(healthText, windowWidth - 225, windowHeight - 50, 1.0f, glm::vec3(0.9f, 0.2f, 0.3f));

			if (ak47Timer != 0.0f)
			{
				if (hasSword)
				{
					RenderText("Ak47: " + std::to_string((ceil((ak47MaxTime - ak47Timer) * 100.0f) / 100.0f)), 25, windowHeight - 200, 1.0f, glm::vec3(0.9f, 0.9f, 1.0f));
				}
				else
					RenderText("Ak47: " + std::to_string((ceil((ak47MaxTime - ak47Timer) * 100.0f) / 100.0f)), 25, windowHeight - 100, 1.0f, glm::vec3(0.9f, 0.9f, 1.0f));
				
			}
			if (swordTimer != 0.0f)
			{
				if(hasAk47)
					RenderText("Sword: " + std::to_string((ceil((swordMaxTime - swordTimer) * 100.0f) / 100.0f)), 25, windowHeight - 200, 1.0f, glm::vec3(0.9f, 0.9f, 1.0f));
				else
					RenderText("Sword: " + std::to_string((ceil((swordMaxTime - swordTimer) * 100.0f) / 100.0f)), 25, windowHeight - 100, 1.0f, glm::vec3(0.9f, 0.9f, 1.0f));
			}
			if (hasShield)
			{
				RenderText("Shield: " + std::to_string(ShieldHealth), windowWidth - 225, windowHeight - 150, 1.0f, glm::vec3(0.9f, 0.9f, 1.0f));
				player->HitBox = glm::vec3(0.2f * 2.0f, 0.2f * 2.0f, 0.55f * 2.0f);
			}
			else
			{
				player->HitBox = glm::vec3(0.2f * 2.0f, 0.2f * 2.0f, 0.3f * 2.0f);
			}
			if (Input::isKeyDown(GLFW_KEY_ESCAPE))
			{
				Suspended = true;
			}
		}
		if (Suspended)
		{
			RenderText("Press Enter to start", windowWidth / 2.0f - 225.0f, windowHeight /  2.0f, 1.0f, glm::vec3(0.9f, 0.9f, 1.0f));

			if(PlayerPoints != 0)
				RenderText("Score: " + std::to_string(PlayerPoints), windowWidth / 2.0f - 100.0f, windowHeight / 2.0f - 200.0f, 1.0f, glm::vec3(0.9f, 0.9f, 1.0f));
			if (Input::isKeyDown(GLFW_KEY_ENTER)) {
				SoundEngine->play2D("assets/sounds/start.mp3");
				Suspended = false;
				if (PlayerHealth <= 0)
				{
					PlayerPoints = 0;
					PlayerHealth = 3;
					CubeSpeed = 4.0f;
					CubeSpawnTickSpeed = 1.0f;
					cubeSpeedBeforeItWasChangedBySword = CubeSpeed;
					if (hasSword) hasSword = !hasSword;
					if (hasAk47) hasAk47 = !hasAk47;
					if (hasShield) hasShield = !hasShield;
				}
			}
		}
		FontShader->Unbind();

		glfwPollEvents();
		glfwSwapBuffers(GLFWDisplay); 
		if (PlayerHealth <= 0)
		{
			if (!Cubes.Cubes.empty())
			{
				Cubes.Cubes.clear();
				SoundEngine->play2D("assets/sounds/death.mp3");
				player->Position = glm::vec3(2.0f, 0.0f, 1.0f);
			}

			Suspended = true;
		}
	}

	TerminateWindow();
}