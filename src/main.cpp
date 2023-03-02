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

	//location to copy enemy from texture
	enemyScorpion.src.w = 144;
	enemyScorpion.src.h = 133;

	//describe location to paste enemy onto the screen
	enemyScorpion.dst.w = 180;
	enemyScorpion.dst.h = 166;
	enemyScorpion.dst.x = 1000;
	enemyScorpion.dst.y = 365;

	//location to copy enemy projectiles from texture
	enemyPoison.src.w = 47;
	enemyPoison.src.h = 37;

	//describe location to paste enemy projectiles onto the screen
	enemyPoison.dst.w = 47;
	enemyPoison.dst.h = 37;
	enemyPoison.dst.x = 970;
	enemyPoison.dst.y = 500;

	//location to copy player from texture
	playerSoldier.src.x = 15;
	playerSoldier.src.y = 12;
	playerSoldier.src.w = 130;
	playerSoldier.src.h = 100;

	//describe location to paste player onto the screen
	playerSoldier.dst.x = 100;
	playerSoldier.dst.y = 432;

	//location to copy player projectile from texture
	playerBullet.src.w = 233;
	playerBullet.src.h = 134;

	//describe location to paste player projectile onto the screen
	playerBullet.dst.x = playerSoldier.dst.x + playerSoldier.dst.w;
	playerBullet.dst.y = playerSoldier.dst.y + playerSoldier.dst.h - (playerBullet.src.h / 4);
	playerBullet.dst.w = playerBullet.src.w / 4;
	playerBullet.dst.h = playerBullet.src.h / 4;

	//location to copy cactus obstacle from texture
	cactus.src.w = 84;
	cactus.src.h = 249;

	//describe location to paste cactus obstacle onto the screen
	cactus.dst.x = 600;
	cactus.dst.y = 383; //531 - cactusObstDst.h 
	cactus.dst.w = cactus.src.w;
	cactus.dst.h = cactus.src.h - 100;

}

void Input()
{

}

void Update()
{
	//playerProjDst.x += 10; // Shooting the bullet at the enemy
	//enemyProjDst.x -= 10; //Shooting the poison ball at the player
}

void Draw()
{
	SDL_SetRenderDrawColor(pRenderer, 5, 5, 15, 255);
	SDL_RenderClear(pRenderer);
	desertBackground.Draw(pRenderer);
	enemyScorpion.Draw(pRenderer);
	enemyPoison.Draw(pRenderer);
	playerSoldier.Draw(pRenderer);
	playerBullet.Draw(pRenderer);
	cactus.Draw(pRenderer);
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

