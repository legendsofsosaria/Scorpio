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
int backgroundX = 0;

float deltaTime = 1.0f / FPS; //time passed between frames in secs

SDL_Window* pWindow = nullptr; //This is a point to SDL_Window. It stores a memory location which we can use later.
SDL_Renderer* pRenderer = nullptr;
bool isGameRunning = true;
SDL_Texture* desertBackground = nullptr;

float enemySpawnDelay = 1.0f;
float enemySpawnTimer = 0.0f;

namespace Scorpio
{
	struct Vec2 //2 dimension vector
	{
		float x = 0;
		float y = 0;
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
			AddFrameTime(0.1f);
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

		void SetSize(int w, int h)
		{
			dst.w = w;
			dst.h = h;
		}

		SDL_Rect GetRect() const
		{
			SDL_Rect returnValue = dst;
			returnValue.x = position.x;
			returnValue.y = position.y;
			return dst;
		}

		Vec2 GetSize() const
		{
			Vec2 returnVec = { dst.w, dst.h };
			return returnVec;
		}
	}; //struct Sprite

	class Bullet
	{
	public:
		Sprite sprite;
		Vec2 velocity;

		//move bullet
		void Update()
		{
			sprite.position.x += velocity.x * deltaTime;
			sprite.position.y += velocity.y * deltaTime;
		}
	};

	class Character
	{
	public:
		Sprite sprite;
		float moveSpeedPx = 120.0f;
		float fireRepeatDelay = 0.5f;

	private:
		float fireRepeatTimer = 0.0f;

	public:
		void Move(Vec2 input)
		{
			sprite.position.x += input.x * (moveSpeedPx * deltaTime);
			sprite.position.y += input.y * (moveSpeedPx * deltaTime);
		}
		
		//only handles left and right shooting
		void Shoot(bool towardRight, std::vector<Bullet>& container, Scorpio::Vec2 velocity)
		{
			//Create a new bullet
			Scorpio::Sprite bulletSprite;
			if (towardRight)
			{
				bulletSprite = Scorpio::Sprite(pRenderer, "../Assets/textures/playerprojectile.png");
				bulletSprite.SetSize(125 / 4, 100 / 4);
				bulletSprite.position.x = sprite.position.x;
				bulletSprite.position.x += sprite.GetSize().x;
				bulletSprite.position.y = sprite.position.y + (sprite.GetSize().y * 0.7);
			}
			else
			{
				int width = 50, height = 35, frames = 2;
				bulletSprite = Scorpio::Sprite(pRenderer, "../Assets/textures/poisonprojectile.png", width, height, frames);
				bulletSprite.SetSize(125 / 4, 100 / 4);
				bulletSprite.position.x = sprite.position.x;
				bulletSprite.position.y = sprite.position.y + ((sprite.GetSize().y * 0.5) + (bulletSprite.GetSize().y * 1.2));
			}
			
			//set up our bullet class instance
			Bullet playerBullet;
			playerBullet.sprite = bulletSprite;
			playerBullet.velocity = velocity;
			
			//add bullet to container (to the end of the array)
			container.push_back(playerBullet);

			//reset cooldown
			fireRepeatTimer = fireRepeatDelay;
		}
		
		void Update()
		{
			//tick down the time for our firing cooldown
			fireRepeatTimer -= deltaTime;
		}

		bool CanShoot()
		{
			return (fireRepeatTimer <= 0.0f);
		}

	}; //character class

	//part of AABB collision detection. Returns true if the bounds defined overlap
	bool AreBoundsOverlapping(int minA, int maxA, int minB, int maxB)
	{
		bool isOverlapping = false;
		if (maxA >= minB && maxA <= maxB) // check if max of A is contained inside B
		{
			isOverlapping = true;
		}
		if (minA <= maxB && minA >= maxB) // check if min of A is contained inside B
		{
			isOverlapping = true;
		}

		return isOverlapping;
	}

	//check collision between two sprites
	bool  AreSpritesOverlapping(const Sprite& A, const Sprite& B)
	{
		//get bounds of each sprite x and y
		int minAx, maxAx, minBx, maxBx;
		int minAy, maxAy, minBy, maxBy;

		SDL_Rect boundsA = A.GetRect();
		SDL_Rect boundsB = B.GetRect();

		SDL_bool isColliding = SDL_HasIntersection(&boundsA, &boundsB);
		return (bool)isColliding;

		//minAx = boundsA.x;
		//minAy = boundsA.y;
		//minBx = boundsB.x;
		//minBy = boundsB.y;
		//
		//maxAx = boundsA.x + boundsA.w;
		//maxAy = boundsA.y + boundsA.h;
		//maxBx = boundsB.x + boundsB.w;
		//maxBy = boundsB.y + boundsB.h;
		//
		//bool overlapOnX = AreBoundsOverlapping(minAx, maxAx, minBx, maxBx);
		//bool overlapOnY = AreBoundsOverlapping(minAy, maxAy, minBy, maxBy);
		//
		////check if bounds overlap. only when both axis have overlap is there a collision
		//return overlapOnX && overlapOnY;

	}
}

//Creating sprite objects


Scorpio::Sprite enemyPoison;
Scorpio::Sprite cactus;

Scorpio::Character playerSoldier;
std::vector<Scorpio::Bullet> playerBulletContainer; //std::vector is a class that allows changing size. This is a dynamic array of Scorpio::Sprite

std::vector<Scorpio::Character> enemyContainer; //container of all enemy ships
std::vector<Scorpio::Bullet> enemyBulletContainer; //container of all enemy bullets(poison)

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
	desertBackground = IMG_LoadTexture(pRenderer, "../Assets/textures/background.bmp");
	int playerWidth = 131, playerHeight = 100, playerFrameCount = 4;
	playerSoldier.sprite = Scorpio::Sprite(pRenderer, "../Assets/textures/playerWalk.png", playerWidth, playerHeight, playerFrameCount);

	//Set size and location of player soldier
	playerSoldier.sprite.SetSize(125, 100);
	playerSoldier.sprite.position.x = 100;
	playerSoldier.sprite.position.y = 430;

}

//input variables
bool isUpPressed = false;
bool isDownPressed = false;
bool isLeftPressed = false;
bool isRightPressed = false;
bool isShootPressed = false;

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

// Define screen boundaries
const int SCREEN_LEFT = 0;
const int SCREEN_RIGHT = SCREEN_WIDTH - playerSoldier.sprite.position.x;
const int SCREEN_TOP = 155;
const int SCREEN_BOTTOM = 435;

void SpawnEnemy()
{
	Scorpio::Sprite enemyScorpion;
	int scorpionWidth = 130, scorpionHeight = 96, scorpionFrameCount = 4;
	enemyScorpion = Scorpio::Sprite(pRenderer, "../Assets/textures/Scorpion_walk_sheet.gif", scorpionWidth, scorpionHeight, scorpionFrameCount);
	//Set size and location of enemy scorpion1
	enemyScorpion.SetSize(125, 100);
	//spawning at random position along y, right side of x
	int maxY = SCREEN_HEIGHT - SCREEN_TOP - (int)enemyScorpion.GetSize().y;
	enemyScorpion.position = { SCREEN_WIDTH,(float)(rand() % maxY + SCREEN_TOP) };

	Scorpio::Character enemy;
	enemy.sprite = enemyScorpion;
	enemy.fireRepeatDelay = 3.5;
	enemy.moveSpeedPx = 80;
	
	//add to list of enemies
	enemyContainer.push_back(enemy);
	
	//rest timer
	enemySpawnTimer = enemySpawnDelay;
}

void UpdatePlayer()
{
	Scorpio::Vec2 inputVector;

	if (isUpPressed)
	{
		inputVector.y = -1;
		playerSoldier.sprite.NextFrame();
		if (playerSoldier.sprite.position.y < SCREEN_TOP)
		{
			playerSoldier.sprite.position.y = SCREEN_TOP;
			

		}
	}
	
	if (isDownPressed)
	{
		inputVector.y = 1;
		playerSoldier.sprite.NextFrame();
		if (playerSoldier.sprite.position.y > SCREEN_BOTTOM)
		{
			playerSoldier.sprite.position.y = SCREEN_BOTTOM;
		}
	}
	
	if (isLeftPressed)
	{
		inputVector.x = -1;
		playerSoldier.sprite.NextFrame();
		if (playerSoldier.sprite.position.x < SCREEN_LEFT)
		{
			playerSoldier.sprite.position.x = SCREEN_LEFT;
		}
	}
	
	if (isRightPressed)
	{
		inputVector.x = 1;
		playerSoldier.sprite.NextFrame();
		if (playerSoldier.sprite.position.x > SCREEN_RIGHT)
		{
			playerSoldier.sprite.position.x = SCREEN_RIGHT;
		}
	}
	
	//if shooting and our shooting is off cooldown
	if (isShootPressed && playerSoldier.CanShoot())
	{
		bool toRight = true;
		Scorpio::Vec2 velocity = { 400,0 };
		//passing bulletContainer by reference to add bullets to this container specifically
		playerSoldier.Shoot(toRight, playerBulletContainer, velocity);
	}

	playerSoldier.Move(inputVector);
	playerSoldier.Update();
}
void Update() // called every frame at FPS..FPS is declared at the top
{
	UpdatePlayer();
	
	//update player bullets 
	for (int i = 0; i < playerBulletContainer.size(); i++)
	{
		playerBulletContainer[i].Update();
	}

	//update enemy bullets
	for (int i = 0; i < enemyBulletContainer.size(); i++)
	{
		enemyBulletContainer[i].Update();
	}

	//update enemy scorpions
	for (int i = 0; i < enemyContainer.size(); i++)
	{
		//get reference to enemy at index[i]
		Scorpio::Character& enemy = enemyContainer[i];

		enemy.Move({ -1,0 });
		enemy.sprite.NextFrame();
		enemy.Update();
		if (enemy.CanShoot())
		{
			bool toRight = false;
			Scorpio::Vec2 velocity = { -200, 0 };
			enemy.Shoot(toRight, enemyBulletContainer,velocity);
		}
	}
	
	//spawn enemies on timer and update timer
	if (enemySpawnTimer <= 0)
	{
		SpawnEnemy();
	}
	else
	{
		enemySpawnTimer -= deltaTime;
	}
	
	//collision detection
	//enemy bullets and player
	for (std::vector<Scorpio::Bullet>::iterator bulletIterator = enemyBulletContainer.begin(); bulletIterator != enemyBulletContainer.end();)
	{
		Scorpio::Sprite& enemyBullet = bulletIterator->sprite;
		if (Scorpio::AreSpritesOverlapping(playerSoldier.sprite, enemyBullet))
		{
			std::cout << "Player was hit" << std::endl;
			playerSoldier.sprite.rotationDegrees += 90.0; //the place to decrement HP

			//remove this element from container.
			bulletIterator = enemyBulletContainer.erase(bulletIterator); //erase function returns new index
		}
		if (bulletIterator != enemyBulletContainer.end())  bulletIterator++;
		
	}

	//for every player bullet
	for (std::vector<Scorpio::Bullet>::iterator bulletIterator = playerBulletContainer.begin(); bulletIterator != playerBulletContainer.end();)
	{
		//for every enemy sprite
		for (auto enemyIterator = enemyContainer.begin(); enemyIterator != enemyContainer.end();)
		{
			//test for collision between player bullet and enemy
			if (Scorpio::AreSpritesOverlapping(bulletIterator->sprite, enemyIterator->sprite))
			{
				//destroy bullet
				bulletIterator = playerBulletContainer.erase(bulletIterator);

				//destroy enemy
				enemyIterator = enemyContainer.erase(enemyIterator);

				//if last object is destroyed, then stop comparing
				if (bulletIterator == playerBulletContainer.end())
					break;

			}

			if (enemyIterator != enemyContainer.end())  enemyIterator++;
		}
		if (bulletIterator != playerBulletContainer.end())  bulletIterator++; //continue as long as not the end

	}
}

//background scrolling
void drawBackground()
{
	SDL_Rect dest;
	int x;

	for (x = backgroundX; x < SCREEN_WIDTH; x += SCREEN_WIDTH)
	{
		dest.x = x;
		dest.y = 0;
		dest.w = SCREEN_WIDTH;
		dest.h = SCREEN_HEIGHT;

		SDL_RenderCopy(pRenderer, desertBackground, NULL, &dest);
	}
}

void doBackground()
{
	if (--backgroundX < -SCREEN_WIDTH)
	{
		backgroundX = 0;
	}
}

void Draw() // draw to screen to show new game state to player
{
	SDL_SetRenderDrawColor(pRenderer, 5, 5, 15, 255);
	SDL_RenderClear(pRenderer);
	
	drawBackground();
	
	playerSoldier.sprite.Draw(pRenderer);
	
	//draw all bullets onto the screen
	for (int i = 0; i < playerBulletContainer.size(); i++)
	{
		playerBulletContainer[i].sprite.Draw(pRenderer);
	}

	//draw all enemy bullets onto the screen
	for (int i = 0; i < enemyBulletContainer.size(); i++)
	{
		enemyBulletContainer[i].sprite.Draw(pRenderer);
	}

	//draw all enemy scorpions onto the screen
	for (int i = 0; i < enemyContainer.size(); i++)
	{
		enemyContainer[i].sprite.Draw(pRenderer);
	}

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

		doBackground();

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

