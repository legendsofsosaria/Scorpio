// Core Libraries
#include <crtdbg.h>
#include <iostream>
#include <Windows.h>
#include <SDL.h> //Allows us to use features of SDL Library
#include <SDL_Image.h> 
#include <vector> //std::vector is an array with variable size

/*
Use SDL to open window, render some sprites at given locations and scale
*/

//Global variables
constexpr float FPS = 60.0f;
constexpr float DELAY_TIME = 1000.0f / FPS; // target deltaTime in ms
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 600;
float deltaTime = 1.0f / FPS; //time passed between frames in secs

SDL_Window* pWindow = nullptr; //This is a point to SDL_Window. It stores a memory location which we can use later.
SDL_Renderer* pRenderer = nullptr;
bool isGameRunning = true;

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

//Creating sprite objects
Scorpio::Sprite playerSoldier;
Scorpio::Sprite enemyScorpion;
Scorpio::Sprite enemyPoison;
Scorpio::Sprite desertBackground;
Scorpio::Sprite cactus;
std::vector<Scorpio::Sprite> playerBulletContainer; //std::vector is a class that allows changing size. This is a dynamic array of Scorpio::Sprite

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

//Load textures to be displayed on the screen
void Load()
{
	desertBackground = Scorpio::Sprite(pRenderer, "../Assets/textures/background.bmp");
	enemyScorpion = Scorpio::Sprite(pRenderer, "../Assets/textures/Scorpion_walk_sheet.gif");
	enemyPoison = Scorpio::Sprite(pRenderer, "../Assets/textures/PoisonProjectile.png");
	playerSoldier = Scorpio::Sprite(pRenderer, "../Assets/textures/charsprite.png");
	cactus = Scorpio::Sprite(pRenderer, "../Assets/textures/cactus1_00.png");

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
bool isShootPressed = false;
float playerMoveSpeedPxPerSec = 120.0f; //Pixels per second, px/sec * sec = px
float playerFireRepeatDelaySec = 0.2f; //seconds
float playerFireCooldownTimerSec = 0.0f; //seconds
float bulletSpeed = 600.0f; //seconds

void Input() //take player input
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
					case(SDL_SCANCODE_UP):
					{
						isUpPressed = true;
						break;
					}
					case(SDL_SCANCODE_S):
					{
						isDownPressed = true;
						break;
					}
					case(SDL_SCANCODE_DOWN):
					{
						isDownPressed = true;
						break;
					}
					case(SDL_SCANCODE_A):
					{
						isLeftPressed = true;
						break;
					}
					case(SDL_SCANCODE_LEFT):
					{
						isLeftPressed = true;
						break;
					}
					case(SDL_SCANCODE_D):
					{
						isRightPressed = true;
						break;
					}
					case(SDL_SCANCODE_RIGHT):
					{
						isRightPressed = true;
						break;
					}
					case(SDL_SCANCODE_SPACE):
					{
						isShootPressed = true;
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
					case(SDL_SCANCODE_UP):
					{
						isUpPressed = false;
						break;
					}
					case(SDL_SCANCODE_S):
					{
						isDownPressed = false;
						break;
					}
					case(SDL_SCANCODE_DOWN):
					{
						isDownPressed = false;
						break;
					}
					case(SDL_SCANCODE_A):
					{
						isLeftPressed = false;
						break;
					}
					case(SDL_SCANCODE_LEFT):
					{
						isLeftPressed = false;
						break;
					}
					case(SDL_SCANCODE_D):
					{
						isRightPressed = false;
						break;
					}
					case(SDL_SCANCODE_RIGHT):
					{
						isRightPressed = false;
						break;
					}
					case(SDL_SCANCODE_SPACE):
					{
						isShootPressed = false;
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
void Update() // called every frame at FPS..FPS is declared at the top
{
	// Define screen boundaries
	const int SCREEN_LEFT = 0;
	const int SCREEN_RIGHT = SCREEN_WIDTH - playerSoldier.dst.w;
	const int SCREEN_TOP = 155;
	const int SCREEN_BOTTOM = 435;

	if (isUpPressed && playerSoldier.dst.y > SCREEN_TOP)
	{
		playerSoldier.dst.y -= playerMoveSpeedPxPerSec * deltaTime;
	}
	if (isDownPressed && playerSoldier.dst.y < SCREEN_BOTTOM)
	{
		playerSoldier.dst.y += playerMoveSpeedPxPerSec * deltaTime;
	}
	if (isLeftPressed && playerSoldier.dst.x > SCREEN_LEFT)
	{
		playerSoldier.dst.x -= playerMoveSpeedPxPerSec * deltaTime;
	}
	if (isRightPressed && playerSoldier.dst.x < SCREEN_RIGHT)
	{
		playerSoldier.dst.x += playerMoveSpeedPxPerSec * deltaTime;
	}
	if (isShootPressed && playerFireCooldownTimerSec < 0.0f)
	{
		//Create a new bullet
		Scorpio::Sprite playerBullet = Scorpio::Sprite(pRenderer, "../Assets/textures/playerprojectile.png");
		
		//set bullet size to be displayed
		playerBullet.dst.w = playerSoldier.src.w / 4;
		playerBullet.dst.h = playerSoldier.src.h / 4;

		//start bullet at player sprite position
		playerBullet.dst.x = playerSoldier.dst.x + playerSoldier.dst.w;
		playerBullet.dst.y = playerSoldier.dst.y + playerSoldier.dst.h * 0.7;
		
		//add bullet to container (to the end of the array)
		playerBulletContainer.push_back(playerBullet);
		
		//reset cooldown
		playerFireCooldownTimerSec = playerFireRepeatDelaySec;
		
	}
	//tick down the time for our firing cooldown
	playerFireCooldownTimerSec -= deltaTime;

	//move all bullets on the screen
	for (int i = 0; i <  playerBulletContainer.size(); i++)
	{
		//get a reference to the bullet in the container
		Scorpio::Sprite* playerBullet = &playerBulletContainer[i];
		playerBullet->dst.x += bulletSpeed * deltaTime;
	}
}

void Draw() // draw to screen to show new game state to player
{
	SDL_SetRenderDrawColor(pRenderer, 5, 5, 15, 255);
	SDL_RenderClear(pRenderer);
	desertBackground.Draw(pRenderer);
	enemyScorpion.Draw(pRenderer);
	//draw all bullets onto the screen
	for (int i = 0; i < playerBulletContainer.size(); i++)
	{
		Scorpio::Sprite* playerBullet = &playerBulletContainer[i];
		playerBullet->Draw(pRenderer);
	}
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

		Input(); 

		Update(); 

		Draw(); 

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

