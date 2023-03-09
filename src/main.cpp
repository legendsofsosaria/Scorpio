// Core Libraries
#include <crtdbg.h>
#include <iostream>
#include <Windows.h>
#include <SDL.h> //Allows us to use features of SDL Library
#include <SDL_Image.h> 

/*
Use SDL to open window, render some sprites at given locations and scale
*/

//Global variables
constexpr float FPS = 60.0f;
constexpr float DELAY_TIME = 1000.0f / FPS;
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 600;
bool isGameRunning = true;
SDL_Window* pWindow = nullptr; //This is a point to SDL_Window. It stores a memory location which we can use later.
SDL_Renderer* pRenderer = nullptr;

namespace Scorpio
{
	//Declaring a struct declares a new type of object we can make
	//After we can make sprites that contain all the contained data fields and functions
	struct Sprite
	{
		//Can be accessed from outside the struct or class
	public:
		//This is a constructor. It is a special type of function. The compiler knows it's a constructor because
		//it's the same name as the class, and has no return type.
		SDL_Texture* pTexture;
		SDL_Rect src;
		SDL_Rect dst;

		Sprite()
		{
			pTexture = nullptr;
			src = SDL_Rect{ 0,0,0,0 };
			dst = SDL_Rect{ 0,0,0,0 };
		}

		Sprite(SDL_Renderer* renderer, const char* filePathToLoad)
		{
			src = SDL_Rect{ 0,0,0,0 };

			pTexture = IMG_LoadTexture(renderer, filePathToLoad);
			if (pTexture == NULL)
			{
				std::cout << "image failed to load: " << SDL_GetError << std::endl;
			}
			SDL_QueryTexture(pTexture, NULL, NULL, &src.w, &src.h); //get dimensions of the texture
			dst = SDL_Rect{ 0,0,src.w,src.h };
		}

		void Draw(SDL_Renderer* renderer)
		{
			SDL_RenderCopy(pRenderer, pTexture, &src, &dst);
		}

		void setPosition(int x, int y)
		{
			dst.x = x;
			dst.y = y;
		}
	};
}

Scorpio::Sprite playerSoldier;
Scorpio::Sprite enemyScorpion;
Scorpio::Sprite enemyPoison;
Scorpio::Sprite playerBullet;
Scorpio::Sprite desertBackground;
Scorpio::Sprite cactus;


//Initialize opens a window and sets up renderer
bool Init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cout << "SDL Init Failed! " << SDL_GetError();
		return false;
	}
	std::cout << "SDL Init Success\n";

	pWindow = SDL_CreateWindow("Elizabeth Gress: 101465946 & David Asher: 101448950", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH, SCREEN_HEIGHT, 0);

	if (pWindow == NULL) //Error checking
		std::cout << "window creation failed: " << SDL_GetError();
	else
		std::cout << "window creation success\n";

	pRenderer = SDL_CreateRenderer(pWindow, -1, 0);

	if (pRenderer == NULL) //Error checking
		std::cout << "window rendering failed: " << SDL_GetError();
	else
		std::cout << "window rendering success\n";

	return true;
}

void Load()
{
	char* background = "../Assets/textures/background.bmp";
	char* enemy = "../Assets/textures/Scorpion_walk_sheet.gif";
	char* enemyProj = "../Assets/textures/PoisonProjectile.png";
	char* player = "../Assets/textures/charsprite.png";
	char* playerProj = "../Assets/textures/playerprojectile.png";
	char* cactusObst = "../Assets/textures/cactus1_00.png";

	desertBackground = Scorpio::Sprite(pRenderer, background);
	enemyScorpion = Scorpio::Sprite(pRenderer, enemy);
	enemyPoison = Scorpio::Sprite(pRenderer, enemyProj);
	playerSoldier = Scorpio::Sprite(pRenderer, player);
	playerBullet = Scorpio::Sprite(pRenderer, playerProj);
	cactus = Scorpio::Sprite(pRenderer, cactusObst);

	desertBackground.dst.w = SCREEN_WIDTH;
	desertBackground.dst.h = SCREEN_HEIGHT;

	//location to copy enemy from texture
	enemyScorpion.src.w = 144;
	enemyScorpion.src.h = 133;

	//describe location to paste enemy onto the screen
	enemyScorpion.dst.w = 180;
	enemyScorpion.dst.h = 166;
	enemyScorpion.dst.x = 1000;
	enemyScorpion.dst.y = 365;

	//location to copy player from texture
	playerSoldier.src.x = 15;
	playerSoldier.src.y = 12;
	playerSoldier.src.w = 130;
	playerSoldier.src.h = 100;

	//describe location to paste player onto the screen
	playerSoldier.dst.x = 100;
	playerSoldier.dst.y = 432;
	playerSoldier.dst.w = playerSoldier.src.w;
	playerSoldier.dst.h = playerSoldier.src.h;

}
//input variables
bool isUpPressed = false;
bool isDownPressed = false;
bool isLeftPressed = false;
bool isRightPressed = false;
bool isSpacePressed = false;

void Input()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) //poll until all events are handled
	{
		//decide what to do with this event
		switch (event.type)
		{
			//Moving up and down and left and right with WASD. Note, this is probably pretty weird for our game to have up and down.
			//I'm leaving it in here just to show we *can* do it, but this is likely something we will want to remove later.
			case(SDL_KEYDOWN):
			{
				SDL_Scancode key = event.key.keysym.scancode;
				switch (key)
				{
					case(SDL_SCANCODE_W):
					{
						isUpPressed = true;
						break;
					}
					case(SDL_SCANCODE_S):
					{
						isDownPressed = true;
						break;
					}
					case(SDL_SCANCODE_A):
					{
						isLeftPressed = true;
						break;
					}
					case(SDL_SCANCODE_D):
					{
						isRightPressed = true;
						break;
					}
					case(SDL_SCANCODE_SPACE):
					{
						isSpacePressed = true;
						break;
					}
				}
				break;
			}
			case (SDL_KEYUP):
			{
				SDL_Scancode key = event.key.keysym.scancode;
				switch (key)
				{
					case(SDL_SCANCODE_W):
					{
						isUpPressed = false;
						break;
					}
					case(SDL_SCANCODE_S):
					{
						isDownPressed = false;
						break;
					}
					case(SDL_SCANCODE_A):
					{
						isLeftPressed = false;
						break;
					}
					case(SDL_SCANCODE_D):
					{
						isRightPressed = false;
						break;
					}
				}
				break;
			}
			break;
		}
	}
}

//update your game state, draw the current frame

void Update()
{
	// Define screen boundaries
	const int SCREEN_LEFT = 0;
	const int SCREEN_RIGHT = SCREEN_WIDTH - playerSoldier.dst.w;
	const int SCREEN_TOP = 0;
	const int SCREEN_BOTTOM = SCREEN_HEIGHT - playerSoldier.dst.h;

	if (isUpPressed && playerSoldier.dst.y > SCREEN_TOP)
	{
		playerSoldier.setPosition(playerSoldier.dst.x, playerSoldier.dst.y - 1);
	}
	if (isDownPressed && playerSoldier.dst.y < SCREEN_BOTTOM)
	{
		playerSoldier.setPosition(playerSoldier.dst.x, playerSoldier.dst.y + 1);
	}
	if (isLeftPressed && playerSoldier.dst.x > SCREEN_LEFT)
	{
		playerSoldier.setPosition(playerSoldier.dst.x - 1, playerSoldier.dst.y);
	}
	if (isRightPressed && playerSoldier.dst.x < SCREEN_RIGHT)
	{
		playerSoldier.setPosition(playerSoldier.dst.x + 1, playerSoldier.dst.y);
	}
}

void Draw()
{
	SDL_SetRenderDrawColor(pRenderer, 5, 5, 15, 255);
	SDL_RenderClear(pRenderer);
	desertBackground.Draw(pRenderer);
	enemyScorpion.Draw(pRenderer);
	playerSoldier.Draw(pRenderer);
	//Show the hidden space we were drawing to called the backbuffer.
	SDL_RenderPresent(pRenderer);
}

/**
 * \brief Program Entry Point
 */
int main(int argc, char* args[])
{

	// show and position the application console
	AllocConsole();
	auto console = freopen("CON", "w", stdout);
	const auto window_handle = GetConsoleWindow();
	MoveWindow(window_handle, 100, 700, 800, 200, TRUE);

	// Display Main SDL Window
	isGameRunning = Init();

	Load();

	// Main Game Loop
	while (isGameRunning)
	{
		const auto frame_start = static_cast<float>(SDL_GetTicks());

		Input(); //take player input

		Update(); //update game state

		Draw(); // draw to screen to show new game state to player

		if (const float frame_time = static_cast<float>(SDL_GetTicks()) - frame_start;
			frame_time < DELAY_TIME)
		{
			SDL_Delay(static_cast<int>(DELAY_TIME - frame_time));
		}

		// delta time
		const auto delta_time = (static_cast<float>(SDL_GetTicks()) - frame_start) / 1000.0f;

	}

	return 0;
}

