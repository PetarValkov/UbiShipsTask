//Using SDL, SDL_image, standard IO, math, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>
#include <list>

#include "LTexture.h"
#include "LTimer.h"
#include "Dot.h"
#include "Enemy.h"
#include "Bullet.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int NumberEnemies = 4;
int POINTS = 0;

std::vector<Bullet*> bulletsUp;
std::vector<Bullet*> bulletsLeft;
std::vector<Bullet*> bulletsRight;
std::vector<Bullet*> enemyBullets;

float random();

int randomRange(int low, int high);

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//Box set collision detector
bool checkCollision( std::vector<SDL_Rect>& a, std::vector<SDL_Rect>& b );

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Ship textures
LTexture gDotTexture;

//Enemy textures
LTexture gEnemyTexture;

//Enemy textures
LTexture gBulletTexture;

//BG texture
LTexture gBGTexture;


LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile( std::string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}
		
void LTexture::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}



Dot::Dot( int x, int y )
{
    //Initialize the offsets
    mPosX = x;
    mPosY = y;

	hp = 100;

    //Create the necessary SDL_Rects
    mColliders.resize( 11 );

    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;

    //Initialize the collision boxes' width and height
    mColliders[ 0 ].w = 6;
    mColliders[ 0 ].h = 1;

    mColliders[ 1 ].w = 10;
    mColliders[ 1 ].h = 1;

    mColliders[ 2 ].w = 14;
    mColliders[ 2 ].h = 1;

    mColliders[ 3 ].w = 16;
    mColliders[ 3 ].h = 2;

    mColliders[ 4 ].w = 18;
    mColliders[ 4 ].h = 2;

    mColliders[ 5 ].w = 20;
    mColliders[ 5 ].h = 6;

    mColliders[ 6 ].w = 18;
    mColliders[ 6 ].h = 2;

    mColliders[ 7 ].w = 16;
    mColliders[ 7 ].h = 2;

    mColliders[ 8 ].w = 14;
    mColliders[ 8 ].h = 1;

    mColliders[ 9 ].w = 10;
    mColliders[ 9 ].h = 1;

    mColliders[ 10 ].w = 6;
    mColliders[ 10 ].h = 1;

	//Initialize colliders relative to position
	shiftColliders();
}

void Dot::handleEvent( SDL_Event& e )
{
    //If a key was pressed
	if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_UP: mVelY -= DOT_VEL; break;
            case SDLK_DOWN: mVelY += DOT_VEL; break;
            case SDLK_LEFT: mVelX -= DOT_VEL; break;
            case SDLK_RIGHT: mVelX += DOT_VEL; break;
			case SDLK_SPACE: 
				bulletsUp.push_back(new Bullet(mPosX, mPosY - 20));
				bulletsLeft.push_back(new Bullet(mPosX - 20, mPosY + 20));
				bulletsRight.push_back(new Bullet(mPosX + 20, mPosY + 20));
				break;
        }
    }
    //If a key was released
    else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_UP: mVelY += DOT_VEL; break;
            case SDLK_DOWN: mVelY -= DOT_VEL; break;
            case SDLK_LEFT: mVelX += DOT_VEL; break;
            case SDLK_RIGHT: mVelX -= DOT_VEL; break;
        }
    }
}

void Dot::move( std::vector<SDL_Rect>& otherColliders )
{
    //Move the dot left or right
    mPosX += mVelX;
    shiftColliders();

    //If the dot collided or went too far to the left or right
    if( ( mPosX < 0 ) || ( mPosX + DOT_WIDTH > SCREEN_WIDTH ) || checkCollision( mColliders, otherColliders ) )
    {
		 mPosX -= mVelX;
		shiftColliders();
    }

    //Move the dot up or down
    mPosY += mVelY;
	shiftColliders();

    //If the dot collided or went too far up or down
    if( ( mPosY < 0 ) || ( mPosY + DOT_HEIGHT > SCREEN_HEIGHT ) || checkCollision( mColliders, otherColliders ) )
    {
		mPosY -= mVelY;
		shiftColliders();
    }
}

void Dot::render()
{
    //Show the dot
	gDotTexture.render( mPosX, mPosY );
}

void Dot::shiftColliders()
{
    //The row offset
    int r = 0;

    //Go through the dot's collision boxes
    for( int set = 0; set < mColliders.size(); ++set )
    {
        //Center the collision box
        mColliders[ set ].x = mPosX + ( DOT_WIDTH - mColliders[ set ].w ) / 2;

        //Set the collision box at its row offset
        mColliders[ set ].y = mPosY + r;

        //Move the row offset down the height of the collision box
        r += mColliders[ set ].h;
    }
}

std::vector<SDL_Rect>& Dot::getColliders()
{
	return mColliders;
}



Bullet::Bullet( int x, int y )
{
    //Initialize the offsets
    mPosX = x;
    mPosY = y;

    //Create the necessary SDL_Rects
    mColliders.resize( 11 );

    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;

    //Initialize the collision boxes' width and height
    mColliders[ 0 ].w = 6;
    mColliders[ 0 ].h = 1;

    mColliders[ 1 ].w = 10;
    mColliders[ 1 ].h = 1;

    mColliders[ 2 ].w = 14;
    mColliders[ 2 ].h = 1;

    mColliders[ 3 ].w = 16;
    mColliders[ 3 ].h = 2;

    mColliders[ 4 ].w = 18;
    mColliders[ 4 ].h = 2;

    mColliders[ 5 ].w = 20;
    mColliders[ 5 ].h = 6;

    mColliders[ 6 ].w = 18;
    mColliders[ 6 ].h = 2;

    mColliders[ 7 ].w = 16;
    mColliders[ 7 ].h = 2;

    mColliders[ 8 ].w = 14;
    mColliders[ 8 ].h = 1;

    mColliders[ 9 ].w = 10;
    mColliders[ 9 ].h = 1;

    mColliders[ 10 ].w = 6;
    mColliders[ 10 ].h = 1;

	//Initialize colliders relative to position
	shiftColliders();
}

void Bullet::moveUp( std::vector<SDL_Rect>& otherColliders )
{
	mVelY++;

    //Move the dot left or right
    mPosX += mVelX;
    shiftColliders();

    //If the dot collided or went too far to the left or right
    if( ( mPosX < 0 ) || ( mPosX + DOT_WIDTH > SCREEN_WIDTH ) || checkCollision( mColliders, otherColliders ) )
    {
        if (!bulletsUp.empty())
		{
			bulletsUp.pop_back();
		}
		mVelY = 0;
    }

    //Move the dot up or down
    mPosY -= mVelY;
	shiftColliders();

    //If the dot collided or went too far up or down
    if( ( mPosY < 0 ) || ( mPosY + DOT_HEIGHT > SCREEN_HEIGHT ) || checkCollision( mColliders, otherColliders ) )
    {
        if (!bulletsUp.empty())
		{
			bulletsUp.pop_back();
			mVelY = 0;
		}
    }
}

void Bullet::moveLeft( std::vector<SDL_Rect>& otherColliders )
{
	mVelX++;
	mVelY++;

    //Move the dot left or right
    mPosX -= mVelX;
    shiftColliders();

    //If the dot collided or went too far to the left or right
    if( ( mPosX < 0 ) || ( mPosX + DOT_WIDTH > SCREEN_WIDTH ) || checkCollision( mColliders, otherColliders ) )
    {
        if (!bulletsLeft.empty())
		{
			bulletsLeft.pop_back();
			mVelX = 0;
			mVelY = 0;
		}
    }

    //Move the dot up or down
    mPosY += mVelY;
	shiftColliders();

    //If the dot collided or went too far up or down
    if( ( mPosY < 0 ) || ( mPosY + DOT_HEIGHT > SCREEN_HEIGHT ) || checkCollision( mColliders, otherColliders ) )
    {
        if (!bulletsLeft.empty())
		{
			bulletsLeft.pop_back();
			mVelX = 0;
			mVelY = 0;
		}
    }
}

void Bullet::moveRight( std::vector<SDL_Rect>& otherColliders )
{
	mVelX++;
	mVelY++;

    //Move the dot left or right
    mPosX += mVelX;
    shiftColliders();

    //If the dot collided or went too far to the left or right
    if( ( mPosX < 0 ) || ( mPosX + DOT_WIDTH >= SCREEN_WIDTH ) || checkCollision( mColliders, otherColliders ) )
    {
		if (!bulletsRight.empty())
		{
			bulletsRight.pop_back();
			mVelY = 0;
			mVelX = 0;
		}
    }

    //Move the dot up or down
    mPosY += mVelY;
	shiftColliders();

    //If the dot collided or went too far up or down
    if( ( mPosY < 0 ) || ( mPosY + DOT_HEIGHT >= SCREEN_HEIGHT ) || checkCollision( mColliders, otherColliders ) )
    {
        if (!bulletsRight.empty())
		{
			bulletsRight.pop_back();
			mVelX = 0;
			mVelY = 0;
		}
    }
}

void Bullet::enemyMove( std::vector<SDL_Rect>& otherColliders )
{
	mVelX++;
	mVelY++;

    //Move the dot left or right
    mPosX -= mVelX;
    shiftColliders();

    //If the dot collided or went too far to the left or right
    if( ( mPosX < 0 ) || ( mPosX + DOT_WIDTH >= SCREEN_WIDTH ) || checkCollision( mColliders, otherColliders ) )
    {
		if (!enemyBullets.empty())
		{
			enemyBullets.pop_back();
		}
    }

    //Move the dot up or down
    mPosY += mVelY;
	shiftColliders();

    //If the dot collided or went too far up or down
    if( ( mPosY < 0 ) || ( mPosY + DOT_HEIGHT >= SCREEN_HEIGHT ) || checkCollision( mColliders, otherColliders ) )
    {
        if (!enemyBullets.empty())
		{
			enemyBullets.pop_back();
		}
    }
}

void Bullet::render()
{
    //Show the dot
	gBulletTexture.render( mPosX, mPosY );
}

void Bullet::shiftColliders()
{
    //The row offset
    int r = 0;

    //Go through the dot's collision boxes
    for( int set = 0; set < mColliders.size(); ++set )
    {
        //Center the collision box
        mColliders[ set ].x = mPosX + ( DOT_WIDTH - mColliders[ set ].w ) / 2;

        //Set the collision box at its row offset
        mColliders[ set ].y = mPosY + r;

        //Move the row offset down the height of the collision box
        r += mColliders[ set ].h;
    }
}

std::vector<SDL_Rect>& Bullet::getColliders()
{
	return mColliders;
}



Enemy::Enemy(int x, int y)
{
    //Initialize the offsets
    mPosX = x;
    mPosY = y;

    //Create the necessary SDL_Rects
    mColliders.resize( 11 );

    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;

    //Initialize the collision boxes' width and height
    mColliders[ 0 ].w = 30;
    mColliders[ 0 ].h = 10;

    mColliders[ 1 ].w = 30;
    mColliders[ 1 ].h = 10;

    mColliders[ 2 ].w = 30;
    mColliders[ 2 ].h = 10;

    mColliders[ 3 ].w = 30;
    mColliders[ 3 ].h = 10;

    mColliders[ 4 ].w = 30;
    mColliders[ 4 ].h = 10;

    mColliders[ 5 ].w = 10;
    mColliders[ 5 ].h = 1;

    mColliders[ 6 ].w = 10;
    mColliders[ 6 ].h = 1;

    mColliders[ 7 ].w = 10;
    mColliders[ 7 ].h = 1;

    mColliders[ 8 ].w = 10;
    mColliders[ 8 ].h = 1;

    mColliders[ 9 ].w = 10;
    mColliders[ 9 ].h = 1;

    mColliders[ 10 ].w = 10;
    mColliders[ 10 ].h = 1;

	//Initialize colliders relative to position
	shiftColliders();
}

void Enemy::move( std::vector<SDL_Rect>& otherColliders )
{
	mVelY++;

    //Move the dot left or right
    mPosX += mVelX;
    shiftColliders();

    //If the dot collided or went too far to the left or right
    if( ( mPosX < 0 ) || ( mPosX + ENEMY_WIDTH > SCREEN_WIDTH ) || checkCollision( mColliders, otherColliders ) )
    {
        //Move back
		POINTS +=100;
       mPosX = randomRange(0, SCREEN_WIDTH);
	   mVelX = 0;
	   shiftColliders();

    }

    //Move the dot up or down
    mPosY += mVelY - mPosY/2;
	shiftColliders();

    //If the dot collided or went too far up or down
    if( ( mPosY < 0 ) || ( mPosY + ENEMY_HEIGHT > SCREEN_HEIGHT ) || checkCollision( mColliders, otherColliders ) )
    {
        //Move back
		POINTS +=100;
        mPosY = randomRange(0, SCREEN_HEIGHT);
		mVelY = 0;
		shiftColliders();
    }

}

void Enemy::shoot(int x, int y)
{
	enemyBullets.push_back(new Bullet(x,y));
}

void Enemy::render(double degrees, SDL_RendererFlip flipType)
{
    //Show the dot
	gEnemyTexture.render( mPosX, mPosY, NULL, degrees, NULL, flipType );
}

void Enemy::shiftColliders()
{
    //The row offset
    int r = 0;

    //Go through the dot's collision boxes
    for( int set = 0; set < mColliders.size(); ++set )
    {
        //Center the collision box
        mColliders[ set ].x = mPosX + ( ENEMY_WIDTH - mColliders[ set ].w ) / 2;

        //Set the collision box at its row offset
        mColliders[ set ].y = mPosY + r;

        //Move the row offset down the height of the collision box
        r += mColliders[ set ].h;
    }
}

std::vector<SDL_Rect>& Enemy::getColliders()
{
	return mColliders;
}

bool init()
{
	//Initialization flag
	bool success = true;

	TTF_Init();
	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}


		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	if (!gDotTexture.loadFromFile( "gosho.png" ))
	{
		printf( "Failed to load ship texture!\n");
		success = false;
	}

	if( !gEnemyTexture.loadFromFile( "enemy.png" ) )
	{
		printf( "Failed to load arrow texture!\n" );
		success = false;
	}

	if( !gBulletTexture.loadFromFile( "bullet.png" ) )
	{
		printf( "Failed to load arrow texture!\n" );
		success = false;
	}

	if( !gBGTexture.loadFromFile( "background.png" ) )
	{
		printf( "Failed to load arrow texture!\n" );
		success = false;
	}

	return success;
}

void close()
{
	//Free loaded images
	gEnemyTexture.free();
	gBGTexture.free();
	gDotTexture.free();
	gBulletTexture.free();

	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;
	
	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

bool checkCollision( std::vector<SDL_Rect>& a, std::vector<SDL_Rect>& b )
{
    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Go through the A boxes
    for( int Abox = 0; Abox < a.size(); Abox++ )
    {
        //Calculate the sides of rect A
        leftA = a[ Abox ].x;
        rightA = a[ Abox ].x + a[ Abox ].w;
        topA = a[ Abox ].y;
        bottomA = a[ Abox ].y + a[ Abox ].h;

        //Go through the B boxes
        for( int Bbox = 0; Bbox < b.size(); Bbox++ )
        {
            //Calculate the sides of rect B
            leftB = b[ Bbox ].x;
            rightB = b[ Bbox ].x + b[ Bbox ].w;
            topB = b[ Bbox ].y;
            bottomB = b[ Bbox ].y + b[ Bbox ].h;

            //If no sides from A are outside of B
            if( ( ( bottomA <= topB ) || ( topA >= bottomB ) || ( rightA <= leftB ) || ( leftA >= rightB ) ) == false )
            {
                //A collision is detected
                return true;
            }
        }
    }

    //If neither set of collision boxes touched
    return false;
}

float random()
{
    return std::rand() / static_cast<float>(RAND_MAX);
}

int randomRange( int low, int high )
{
    int range = high - low;

    return static_cast<int>(random() * range) + low;
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{	
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//The background scrolling offset
            int scrollingOffset = 0;

			//Angle of rotation
			double degrees = 0;

			//The dot that will be moving around on the screen
			Dot dot ( 100, 300);

			//Opening Fonts and message
			TTF_Font *times;
			times = TTF_OpenFont("arial.ttf", 14);

			SDL_Color white = {255, 255, 255};
			
			std::stringstream health;
			health <<"Health:" << dot.hp << "    Points: " << POINTS;

			const std::string str = health.str();

			SDL_Surface *surface = TTF_RenderText_Solid(times,
									health.str().c_str(), white);


			 SDL_Texture * texture = SDL_CreateTextureFromSurface(gRenderer,
					 surface);

			  int texW = 0;
			  int texH = 0;
			  SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
			  SDL_Rect dstrect = { 0, 0, texW, texH };

			
			//creating enemies
			std::vector<Enemy*> enemies;

			for (int i = 0; i < NumberEnemies; i++)
			{
				enemies.push_back(new Enemy(i*100, 0));
			}
			
			//Flip type
			SDL_RendererFlip flipType = SDL_FLIP_NONE;

			//While application is running
			while( !quit)
			{

				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
				if( e.type == SDL_QUIT )
					{
						quit = true;
					}

					dot.handleEvent(e);
				}

				//rotating
					degrees += 1;


					//collision between enemies and bullets
				for (int i = 0; i < enemies.size(); i++)
				{
					for (int j = 0; j < bulletsUp.size(); j++)
					{
						enemies[i]->move(bulletsUp[j]->getColliders());
					}

					for (int j = 0; j < bulletsLeft.size(); j++)
					{
						enemies[i]->move(bulletsLeft[j]->getColliders());
					}

					for (int j = 0; j < bulletsRight.size(); j++)
					{
						enemies[i]->move(bulletsRight[j]->getColliders());
					}

					enemies[i]->move(dot.getColliders());

					enemies[i]->shoot(enemies[i]->mPosX, enemies[i]->mPosY);

					dot.move( enemies[i]->getColliders());
				}


				//moving of bullets
				for (int i = 0; i < bulletsUp.size(); i++)
				{
					bulletsUp[i]->moveUp(dot.getColliders());
				}

				for (int i = 0; i < bulletsLeft.size(); i++)
				{
					bulletsLeft[i]->moveLeft(dot.getColliders());
				}

				for (int i = 0; i < bulletsRight.size(); i++)
				{
					bulletsRight[i]->moveRight(dot.getColliders());
				}

				for (int i = 0; i < enemyBullets.size(); i++)
				{
					enemyBullets[i]->enemyMove(dot.getColliders());
				}

				//Scroll background
                --scrollingOffset;
                if( scrollingOffset < -gBGTexture.getWidth() )
                {
                    scrollingOffset = 0;
                }

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				SDL_RenderClear( gRenderer );

				//Render background
                gBGTexture.render( scrollingOffset, 0 );
				SDL_RenderCopy(gRenderer, texture, NULL, &dstrect);
                gBGTexture.render( scrollingOffset + gBGTexture.getWidth(), 0 );

				//Render Dot
				dot.render();

				//displaying enemeies and bullets
				for (int i = 0; i < enemies.size(); i++)
				{
					enemies[i]->render(degrees,flipType);
				}

				for (int i = 0; i < bulletsUp.size(); i++)
				{
					bulletsUp[i]->render();
				}

				for (int i = 0; i < bulletsLeft.size(); i++)
				{
					bulletsLeft[i]->render();
				}

				for (int i = 0; i < bulletsRight.size(); i++)
				{
					bulletsRight[i]->render();
				}

				for (int i = 0; i < enemyBullets.size(); i++)
				{
					enemyBullets[i]->render();
				}

				//Update screen
				SDL_RenderPresent( gRenderer );
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}