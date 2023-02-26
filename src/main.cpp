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
SDL_Texture* pMyBackground = nullptr; //Background texture
SDL_Texture* pEnemy = nullptr; //Enemy Texture
SDL_Texture* pEnemyProj = nullptr; // Enemy Projectile texture

SDL_Rect enemySpriteDst;
SDL_Rect enemySpriteSrc;
SDL_Rect enemyProjDst;
SDL_Rect enemyProjSrc;

//Initialize opens a window and sets up renderer
bool Init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cout << "SDL Init Failed! " << SDL_GetError();
		return false;
	}
	std::cout << "SDL Init Success\n";

	pWindow = SDL_CreateWindow("Elizabeth Gress: 101465946 & ", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
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

	pMyBackground = IMG_LoadTexture(pRenderer, background);
	pEnemy = IMG_LoadTexture(pRenderer, enemy);
	pEnemyProj = IMG_LoadTexture(pRenderer, enemyProj);

	//null checks for sanity
	if (pMyBackground == NULL)
		std::cout << "background image failed to load: " << background << std::endl;
	if (pEnemy == NULL)
		std::cout << "enemy image failed to load: " << enemy << std::endl;
	if (pEnemyProj == NULL)
		std::cout << "enemy projectile image failed to load: " << enemyProj << std::endl;

	//location to copy enemy from texture
	enemySpriteSrc.x = 0;
	enemySpriteSrc.y = 0;
	enemySpriteSrc.w = 144;
	enemySpriteSrc.h = 133;

	//describe location to paste enemy onto the screen
	enemySpriteDst.w = 180;
	enemySpriteDst.h = 166;
	enemySpriteDst.x = 1000;
	enemySpriteDst.y = 365;

	//location to copy enemy projectiles from texture
	enemyProjSrc.x = 0;
	enemyProjSrc.y = 0;
	enemyProjSrc.w = 47;
	enemyProjSrc.h = 37;

	//describe location to paste enemy projectiles onto the screen
	enemyProjDst.w = 47;
	enemyProjDst.h = 37;
	enemyProjDst.x = 970;
	enemyProjDst.y = 500;
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
	SDL_RenderCopy(pRenderer, pMyBackground, NULL, NULL);
	SDL_RenderCopy(pRenderer, pEnemy, &enemySpriteSrc, &enemySpriteDst);
	SDL_RenderCopy(pRenderer, pEnemyProj, &enemyProjSrc, &enemyProjDst);
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

