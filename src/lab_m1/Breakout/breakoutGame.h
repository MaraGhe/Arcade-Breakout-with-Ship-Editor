#include <vector>
#include <utility>

#include "utils/gl_utils.h"
#include "components/simple_scene.h"
#include "lab_m1/Breakout/shapes.h"

#define brickRows 6
#define brickColumns 10
#define ballSpeed 7
#define shipSpeed 7
#define brickShrinkSpeed 3

namespace breakoutGame
{
	class BreakoutGame : public gfxc::SimpleScene
	{
	public:
		BreakoutGame();
		~BreakoutGame();

		// sets coordinates & creates the meshes needed to render the editor
		void Init(float X, float Y, float width, float height, float unit,
			std::unordered_map<std::string, Mesh*>& meshes);

		// updates brick destruction and ball movement & collision logic
		void gameLogicUpdate(float deltaTimeSceonds);

		// moves the ship right or left
		void moveShip(const std::string& direction);

		// game screen coordinates & size
		float X, Y, width, height;

		// ship coordinates and size
		float shipX, shipY, shipWidth, shipHeight;

		// ball coordinates and size
		float ballX, ballY, ballRadius;
		float ballAngle;

		/*
		* the brick grid storing the durability of each brick
		* 0 - destroyed brick that hasn't shrunk yet
		* -1 - destroyed brick that no longer appears on screen
		*/ 
		float brickGrid[brickRows][brickColumns]{};
		float brickGridX, brickGridY;
		float brickWidth, brickHeight;

		// coordinates for the upper row of the game screen
		float upperX, upperY;
		int livesLeft;
		int score, highScore = 0;
		int bricksLeft = brickColumns * brickRows;

		bool gameRunning = false;

	protected:

		// changes the ball's angle when hitting a horizontal or vertical surface
		void changeAngleHorizontal();
		void changeAngleVertical();

		// resets the ship and ball after a life is lost
		void resetPlayer();

		// game logic for the ball hitting a wall
		void wallHit();

		// game logic for the ball hitting the ship
		void shipHit();

		// game logic for the ball hitting a brick
		void brickHit();

		// rasies the score and updates the high score
		void raiseScore();

		float unit;
		glm::mat3 modelMatrix;
	};
}