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
	struct Vec2 //2 dimension vector
	{
		float x;
		float y;
	};
	//Declaring a struct declares a new type of object we can make
	//After we can make sprites that contain all the contained data fields and functions
	struct Sprite
	{

	private:
		//Can't be accessed outside the struct or class
		SDL_Texture* pTexture;
		SDL_Rect src;
		SDL_Rect dst;
		int animationFrameCount = 1;
		float animationCurrentFrame = 0;

	public:
		double rotationDegrees = 0.0;
		SDL_RendererFlip flipState = SDL_FLIP_NONE;
		Vec2 position; //where the sprite will draw on the screen

		Sprite()
		{
			std::cout << "Sprite Default Constructor!" << std::endl;
			pTexture = nullptr;
			src = SDL_Rect{ 0,0,0,0 };
			dst = SDL_Rect{ 0,0,0,0 };
		}

		Sprite(SDL_Renderer* renderer, const char* filePathToLoad)
		{
			std::cout << "Sprite Constructor!" << std::endl;

			pTexture = IMG_LoadTexture(renderer, filePathToLoad);
			if (pTexture == NULL)
			{
				std::cout << "image failed to load: " << SDL_GetError << std::endl;
			}
			else
			{
				std::cout << "Image load success: " << filePathToLoad << std::endl;
			}
			//Query for our images height and width to set a default source rect value spanning the whole texture
			if (SDL_QueryTexture(pTexture, NULL, NULL, &src.w, &src.h) != 0)
			{
				std::cout << "Query Texture Failed!" << SDL_GetError << std::endl;
			}

			//At this point, SDLQueryTexture has used the addresses for src.w and src.h to place width and height into memory

			src.x = 0;
			src.y = 0;

			dst.x = 0;
			dst.y = 0;
			dst.w = src.w;
			dst.h = src.h;
		}

		//A constructor for animated sprites
		Sprite(SDL_Renderer* renderer, const char* filePathToLoad, int frameWidth, int frameHeight, int numberOfFrames) : Sprite(renderer, filePathToLoad)
		{
			src.w = frameWidth;
			src.h = frameHeight;
			SetSize(frameWidth, frameHeight);
			animationFrameCount = numberOfFrames;
		}

		void Draw(SDL_Renderer* renderer)
		{
			dst.x = position.x;
			dst.y = position.y;
			src.x = (int)animationCurrentFrame * src.w; //find current frame position in source image
			int result = SDL_RenderCopyEx(renderer, pTexture, &src, &dst, rotationDegrees, NULL, flipState);
			if (result != 0)
			{
				std::cout << "Render Failed! " << SDL_GetError() << std::endl;
			}
		}

		void SetAnimationFrameDimensions(int frameWidth, int frameHeight)
		{
			src.w = frameWidth;
			src.h = frameHeight;
		}

		void NextFrame()
		{
			AddFrameTime(1.0f);
		}

		void AddFrameTime(float frames)
		{
			animationCurrentFrame += frames;
			if (animationCurrentFrame >= animationFrameCount)
			{
				animationCurrentFrame = 0;
			}
		}

		void SetPosition(int x, int y)
		{
			position.x = x;
			position.y = y;
		}

		void SetSize(int x, int y)
		{
			dst.w = x;
			dst.h = y;
		}

		Vec2 GetSize()
		{
			Vec2 sizeXY = { dst.w, dst.h };
			return sizeXY;
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
	int scorpionWidth = 130, scorpionHeight = 96, scorpionFrameCount = 4;
	enemyScorpion = Scorpio::Sprite(pRenderer, "../Assets/textures/Scorpion_walk_sheet.gif", scorpionWidth, scorpionHeight, scorpionFrameCount);
	enemyPoison = Scorpio::Sprite(pRenderer, "../Assets/textures/PoisonProjectile.png");
	int playerWidth = 131, playerHeight = 100, playerFrameCount = 4;
	playerSoldier = Scorpio::Sprite(pRenderer, "../Assets/textures/playerWalk.png", playerWidth, playerHeight, playerFrameCount);

	//Set size and location of background
	desertBackground.SetSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	desertBackground.position.x = 0;
	desertBackground.position.y = 0;

	//Set size and location of enemy scorpion
	enemyScorpion.SetSize(180, 166);
	enemyScorpion.position.x = 1000;
	enemyScorpion.position.y = 365;

	//Set size and location of player soldier
	playerSoldier.SetSize(125, 100);
	playerSoldier.position.x = 100;
	playerSoldier.position.y = 430;


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
	const int SCREEN_RIGHT = SCREEN_WIDTH - playerSoldier.position.x;
	const int SCREEN_TOP = 155;
	const int SCREEN_BOTTOM = 435;

	if (isUpPressed && playerSoldier.position.y > SCREEN_TOP)
	{
		playerSoldier.position.y -= playerMoveSpeedPxPerSec * deltaTime;
	}
	if (isDownPressed && playerSoldier.position.y < SCREEN_BOTTOM)
	{
		playerSoldier.position.y += playerMoveSpeedPxPerSec * deltaTime;
	}
	if (isLeftPressed && playerSoldier.position.x > SCREEN_LEFT)
	{
		playerSoldier.position.x -= playerMoveSpeedPxPerSec * deltaTime;
	}
	if (isRightPressed && playerSoldier.position.x < SCREEN_RIGHT)
	{
		playerSoldier.position.x += playerMoveSpeedPxPerSec * deltaTime;
	}
	if (isShootPressed && playerFireCooldownTimerSec < 0.0f)
	{
		//Create a new bullet
		Scorpio::Sprite playerBullet = Scorpio::Sprite(pRenderer, "../Assets/textures/playerprojectile.png");

		//set bullet size to be displayed
		//playerBullet.dst.w = playerSoldier.src.w / 4;
		//playerBullet.position.y = playerSoldier / 4;

		//start bullet at player sprite position
		playerBullet.position.x = playerSoldier.position.x + playerSoldier.position.y;
		playerBullet.position.y = playerSoldier.position.y + playerSoldier.position.y * 0.7;

		//add bullet to container (to the end of the array)
		playerBulletContainer.push_back(playerBullet);

		//reset cooldown
		playerFireCooldownTimerSec = playerFireRepeatDelaySec;

	}
	//tick down the time for our firing cooldown
	playerFireCooldownTimerSec -= deltaTime;

	//move all bullets on the screen
	for (int i = 0; i < playerBulletContainer.size(); i++)
	{
		//get a reference to the bullet in the container
		Scorpio::Sprite* playerBullet = &playerBulletContainer[i];
		playerBullet->position.x += bulletSpeed * deltaTime;
	}
	playerSoldier.AddFrameTime(0.1);
	enemyScorpion.AddFrameTime(0.1);
}

void Draw() // draw to screen to show new game state to player
{
	SDL_SetRenderDrawColor(pRenderer, 5, 5, 15, 255);
	SDL_RenderClear(pRenderer);
	
	desertBackground.Draw(pRenderer);
	enemyScorpion.Draw(pRenderer);
	playerSoldier.Draw(pRenderer);
	//draw all bullets onto the screen
	/*for (int i = 0; i < playerBulletContainer.size(); i++)
	{
		Scorpio::Sprite* playerBullet = &playerBulletContainer[i];
		playerBullet->Draw(pRenderer);
	}*/

	//Show the hidden space we were drawing-to called the backbuffer.
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

