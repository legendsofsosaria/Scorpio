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
SDL_Texture* pPlayer = nullptr;//Player tecture
SDL_Texture* pPlayerProj = nullptr;//Player projectile texture
SDL_Texture* pCactusObst = nullptr;//Castus obstacle texture

SDL_Rect enemySpriteDst;
SDL_Rect enemySpriteSrc;
SDL_Rect enemyProjDst;
SDL_Rect enemyProjSrc;
SDL_Rect playerSpriteSrc;
SDL_Rect playerSpriteDst;
SDL_Rect playerProjSrc;
SDL_Rect playerProjDst;
SDL_Rect cactusObstSrc;
SDL_Rect cactusObstDst;

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
	char* player = "../Assets/textures/charsprite.png";
	char* playerProj = "../Assets/textures/playerprojectile.png";
	char* cactusObst = "../Assets/textures/cactus1_00.png";

	pMyBackground = IMG_LoadTexture(pRenderer, background);
	pEnemy = IMG_LoadTexture(pRenderer, enemy);
	pEnemyProj = IMG_LoadTexture(pRenderer, enemyProj);
	pPlayer = IMG_LoadTexture(pRenderer, player);
	pPlayerProj = IMG_LoadTexture(pRenderer, playerProj);
	pCactusObst = IMG_LoadTexture(pRenderer, cactusObst);

	//null checks for sanity
	if (pMyBackground == NULL)
		std::cout << "background image failed to load: " << background << std::endl;
	if (pEnemy == NULL)
		std::cout << "enemy image failed to load: " << enemy << std::endl;
	if (pEnemyProj == NULL)
		std::cout << "enemy projectile image failed to load: " << enemyProj << std::endl;
	if (pPlayer == NULL)
		std::cout << "player image failed to load: " << player << std::endl;
	if (pPlayerProj == NULL)
		std::cout << "enemy projectile image failed to load: " << playerProj << std::endl;
	if (pCactusObst == NULL)
		std::cout << "cactus obstacle failed to load: " << cactusObst << std::endl;

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

	//location to copy player from texture
	playerSpriteSrc.x = 15;
	playerSpriteSrc.y = 12;
	playerSpriteSrc.w = 130;
	playerSpriteSrc.h = 100;

	//describe location to paste player onto the screen
	playerSpriteDst.x = 100;
	playerSpriteDst.y = 432;
	playerSpriteDst.w = playerSpriteSrc.w;
	playerSpriteDst.h = playerSpriteSrc.h;

	//location to copy player projectile from texture
	playerProjSrc.x = 0;
	playerProjSrc.y = 0;
	playerProjSrc.w = 233;
	playerProjSrc.h = 134;

	//describe location to paste player projectile onto the screen
	playerProjDst.x = playerSpriteDst.x + playerSpriteDst.w;
	playerProjDst.y = playerSpriteDst.y + playerSpriteDst.h - (playerProjSrc.h / 4);
	playerProjDst.w = playerProjSrc.w / 4;
	playerProjDst.h = playerProjSrc.h / 4;

	//location to copy cactus obstacle from texture
	cactusObstSrc.x = 0;
	cactusObstSrc.y = 0;
	cactusObstSrc.w = 84;
	cactusObstSrc.h = 249;

	//describe location to paste cactus obstacle onto the screen
	cactusObstDst.x = 600;
	cactusObstDst.y = 383; //531 - cactusObstDst.h 
	cactusObstDst.w = cactusObstSrc.w;
	cactusObstDst.h = cactusObstSrc.h - 100;

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
	SDL_RenderCopy(pRenderer, pPlayer, &playerSpriteSrc, &playerSpriteDst);
	SDL_RenderCopy(pRenderer, pPlayerProj, &playerProjSrc, &playerProjDst);
	SDL_RenderCopy(pRenderer, pCactusObst, &cactusObstSrc, &cactusObstDst);
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

