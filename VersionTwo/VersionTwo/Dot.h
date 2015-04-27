//The dot that will move around on the screen
class Dot
{
    public:
        //The dimensions of the dot
        static const int DOT_WIDTH = 32;
        static const int DOT_HEIGHT = 32;

        //Maximum axis velocity of the dot
        static const int DOT_VEL = 1;

		int hp;

        //Initializes the variables
        Dot( int x, int y );

        //Takes key presses and adjusts the dot's velocity
        void handleEvent( SDL_Event& e );

        //Moves the dot and checks collision
        void move( std::vector<SDL_Rect>& otherColliders );

        //Shows the dot on the screen
        void render();

        //Gets the collision boxes
        std::vector<SDL_Rect>& getColliders();

    private:
        //The X and Y offsets of the dot
        int mPosX, mPosY;

		//health
		

        //The velocity of the dot
        int mVelX, mVelY;
        
        //Dot's collision boxes
        std::vector<SDL_Rect> mColliders;

        //Moves the collision boxes relative to the dot's offset
        void shiftColliders();
};