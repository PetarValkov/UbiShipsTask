#ifndef _BULLET_H_
    #define _BULLET_H_

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <vector>

//The dot that will move around on the screen
class Bullet
{
    public:
        //The dimensions of the dot
        static const int DOT_WIDTH = 16;
        static const int DOT_HEIGHT = 16;

        //Maximum axis velocity of the dot
        static const int DOT_VEL = 1;

        //Initializes the variables
        Bullet( int x, int y );

        //Moves the dot and checks collision
        void moveUp( std::vector<SDL_Rect>& otherColliders );
		void moveLeft( std::vector<SDL_Rect>& otherColliders );
		void moveRight( std::vector<SDL_Rect>& otherColliders );
		void enemyMove( std::vector<SDL_Rect>& otherColliders );

        //Shows the dot on the screen
        void render();

        //Gets the collision boxes
        std::vector<SDL_Rect>& getColliders();

    private:
        //The X and Y offsets of the dot
        int mPosX, mPosY;

        //The velocity of the dot
        int mVelX, mVelY;
        
        //Dot's collision boxes
        std::vector<SDL_Rect> mColliders;

        //Moves the collision boxes relative to the dot's offset
        void shiftColliders();
};

#endif