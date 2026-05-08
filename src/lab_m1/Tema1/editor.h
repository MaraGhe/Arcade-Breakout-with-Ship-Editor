#include <vector>
#include <queue>
#include <utility>

#include "utils/gl_utils.h"
#include "components/simple_scene.h"
#include "lab_m1/Tema1/shapes.h"

#define gridRows 10
#define gridColumns 20

namespace editor
{
	class Editor : public gfxc::SimpleScene
	{
	public:
		Editor();
		~Editor();

		// sets the coordinates & creates the meshes for the editor
		void Init(float X, float Y, float width, float height, float unit,
			std::unordered_map<std::string, Mesh*>& meshes);

		// checks if the player has pressed the start button
		bool startSelected(int mouseX, int mouseY);

		// checks if the player selected a building block
		bool blockSelected(int mouseX, int mouseY);

		// block placement and removal, if possible
		void placeBlock(int blockX, int blockY);
		void removeBlock(int blockX, int blockY);

		// checks if the build is valid
		void validateBuild();
		bool validBuild = false;

		// creates the finished ship mesh and saves its dimensions
		void createShipMesh(std::unordered_map<std::string, Mesh*>& meshes, 
			float& width, float& height, float scale);

		float selectBlockX, selectBlockY;
		bool blockMoving = false;	// if a block is being moved to the grid
		float blockMovingX, blockMovingY;	// coordinates of the moving block

		// editor screen coordinates & size
		float X, Y, width, height;

		// the buildingGrid: 0 - empty slot; 1 - filled slot
		int grid[gridRows][gridColumns]{};
		float gridX, gridY;	// grid coordinates

		int blocksLeft = 10;
		float startButonX, startButonY;	// start button coordinates

	protected:
		// places or removes a block from (X, Y) if possible
		// opeartion = 0 for removal, 1 for placement
		void modifyGrid(int blockX, int blockY, int operation);

		float unit;
		glm::mat3 modelMatrix;
	};

}