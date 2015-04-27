#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <vector>

#include "LTexture.h"
#include "LTimer.h"

//The enemy that will move around on the screen
class Enemy
{
       public:
		//The dimensions of the enemy
		static const int ENEMY_WIDTH = 64;
		static const int ENEMY_HEIGHT = 64;

		//Maximum axis velocity of the dot
		static const int ENEMY_VEL = 2;

		//Initializes the variables
		Enemy( int x, int y );

		//Moves the enemy and checks collision
		void move( std::vector<SDL_Rect>& otherColliders );

		//Shoot bullets
		void shoot(int x, int y);

		//Shows the enemy on the screen
		void render(double degrees, SDL_RendererFlip flipType);

		//Gets the collision boxes
		std::vector<SDL_Rect>& getColliders();


    public:
		//The X and Y offsets of the enemy
		int mPosX, mPosY;

		//The velocity of the enemy
		int mVelX, mVelY;
		
		//Dot's collision boxes
	    std::vector<SDL_Rect> mColliders;

		//Moves the collision boxes relative to the enemy's offset
		void shiftColliders();
};