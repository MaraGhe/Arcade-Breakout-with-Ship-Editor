#include "lab_m1/Tema1/editor.h"

using namespace editor;

Editor::Editor()
{
}


Editor::~Editor()
{
}


void Editor::Init(float X, float Y, float width, float height, float unit,
    std::unordered_map<std::string, Mesh*>& meshes)
{
    this->X = X;
    this->Y = Y;
    this->width = width;
    this->height = height;
    this->unit = unit;


    meshes["block_selection_border"] = shapes::rectangle(glm::vec3(X, Y, 0),
        unit * 3, unit * 3, "block_selection_border", glm::vec3(1, 0, 0), false);

    selectBlockX = 2.5 * unit;
    selectBlockY = height / 2 - 0.5 * unit;
    meshes["block"] = shapes::rectangle(glm::vec3(X, Y, 1),
        unit, unit, "block", glm::vec3(0.8, 0.8, 0.8), true);

    meshes["green_square_border"] = shapes::rectangle(glm::vec3(X, Y, 0),
        unit * 21, unit * 3, "green_square_border", glm::vec3(0, 1, 0), false);

    meshes["green_square"] = shapes::rectangle(glm::vec3(X, Y, 0),
        unit, unit, "green_square", glm::vec3(0, 1, 0), true);

    startButonX = width - 2.5 * unit;
    startButonY = height - 2.5 * unit;
    meshes["start_button_red"] = shapes::start_button(glm::vec3(X, Y, 0),
        unit, unit, "start_button_red", true, false);
    meshes["start_button_green"] = shapes::start_button(glm::vec3(X, Y, 0),
        unit, unit, "start_button_green", true, true);

    // grid creation
    gridX = 8 * unit;
    gridY = 2.5 * unit;
    meshes["grid_border"] = shapes::rectangle(glm::vec3(X, Y, 0), (gridColumns + 1) * unit,
        (gridRows + 1) * unit, "grid_border", glm::vec3(0, 0, 1), false);

    meshes["grid_square"] = shapes::rectangle(glm::vec3(X, Y, 0),
        unit, unit, "grid_square", glm::vec3(0.2, 0.2, 0.9), true);
}

bool Editor::startSelected(int mouseX, int mouseY)
{
    return mouseX >= startButonX && mouseX <= startButonX + unit &&
        mouseY >= startButonY && mouseY <= startButonY + unit;
}

bool Editor::blockSelected(int mouseX, int mouseY)
{
    return mouseX >= selectBlockX && mouseX <= selectBlockX + unit &&
        mouseY >= selectBlockY && mouseY <= selectBlockY + unit;
}

void Editor::modifyGrid(int blockX, int blockY, int operation)
{
    // check if a grid block was actually selected
    if (blockX < gridX || blockX > gridX + gridColumns * unit ||
        blockY < gridY || blockY > gridY + gridRows * unit)
        return;
    
    int i, j;
    i = (blockY - gridY) / unit;
    j = (blockX - gridX) / unit;

    if (operation == 0 && grid[i][j] == 1)
    {
        grid[i][j] = 0;
        blocksLeft++;
    }

    if (operation == 1 && grid[i][j] == 0)
    {
        grid[i][j] = operation;
        blocksLeft--;
    }
    
    // check if the updated build is valid
    validateBuild();
}

void Editor::removeBlock(int blockX, int blockY)
{
    modifyGrid(blockX, blockY, 0);
}

void Editor::placeBlock(int blockX, int blockY)
{
    modifyGrid(blockX, blockY, 1);
}

void Editor::validateBuild()
{
    // check if there are too many blocks
    if (blocksLeft < 0)
    {
        validBuild = false;
        return;
    }

    // number of blocks left to count
    int blocksToCount = 10 - blocksLeft;
    if (blocksToCount == 0)
    {
        validBuild = false;
        return;
    }

    bool visited[gridRows][gridColumns]{};

    std::queue<std::pair<int, int>> q;

    // find the starting block
    bool foundStartBlock = false;
    for (int i = 0; i < gridRows && !foundStartBlock; i++)
        for (int j = 0; j < gridColumns && !foundStartBlock; j++)
            if (grid[i][j] == 1)
            {
                q.push({ i, j });
                visited[i][j] = true;
                foundStartBlock = true;
            }

    // bfs algorithm to check if the blocks are all connected
    while (!q.empty())
    {
        std::pair<int, int> current = q.front();
        q.pop();
        blocksToCount--;

        int i = current.first;
        int j = current.second;

        // check upper cell
        if (i + 1 < gridRows && grid[i + 1][j] && !visited[i + 1][j])
        {
            visited[i + 1][j] = true;
            q.push({ i + 1, j });
        }

        // check lower cell
        if (i - 1 >= 0 && grid[i - 1][j] && !visited[i - 1][j])
        {
            visited[i - 1][j] = true;
            q.push({ i - 1, j });
        }

        // check left cell
        if (j - 1 >= 0 && grid[i][j - 1] && !visited[i][j - 1])
        {
            visited[i][j - 1] = true;
            q.push({ i, j - 1 });
        }

        // check right cell
        if (j + 1 < gridColumns && grid[i][j + 1] && !visited[i][j + 1])
        {
            visited[i][j + 1] = true;
            q.push({ i, j + 1 });
        }
    }

    if (blocksToCount == 0)
        validBuild = true;
    else
        validBuild = false;
}

void Editor::createShipMesh(std::unordered_map<std::string, Mesh*>& meshes, 
    float& width, float& height, float scale)
{
    std::vector<VertexFormat> vertices = {};
    std::vector<unsigned int> indices = {};

    // the first block will be at (X, Y)
    glm::vec3 bottomLeftCorner = glm::vec3(X, Y, 0);
    int firstBlockI = -1, firstBlockJ = -1;
    int minI, maxI, minJ, maxJ; // to determine the size of the AABB

    glm::vec3 color = glm::vec3(0.8, 0.8, 0.8);
    int blocks = 0;

    float shipUnit = scale * unit;
    for (int i = 0; i < gridRows; i++)
    {
        for (int j = 0; j < gridColumns; j++)
            if (grid[i][j])
            {
                if (firstBlockI == -1 && firstBlockJ == -1)
                {
                    firstBlockI = i;
                    firstBlockJ = j;
                    minI = i; maxI = i; minJ = j; maxJ = j;
                }
                else {

                    bottomLeftCorner = glm::vec3(
                        X + (j - firstBlockJ) * shipUnit,
                        Y + (i - firstBlockI) * shipUnit,
                        0);
                }

                // add vertices
                vertices.push_back(VertexFormat(bottomLeftCorner, color));
                vertices.push_back(VertexFormat(bottomLeftCorner + glm::vec3(shipUnit, 0, 0), color));
                vertices.push_back(VertexFormat(bottomLeftCorner + glm::vec3(shipUnit, shipUnit, 0), color));
                vertices.push_back(VertexFormat(bottomLeftCorner + glm::vec3(0, shipUnit, 0), color));
                
                // add indices
                int index = blocks * 4;
                indices.push_back(index);
                indices.push_back(index + 1);
                indices.push_back(index + 2);

                indices.push_back(index);
                indices.push_back(index + 2);
                indices.push_back(index + 3);

                // update dimensions
                if (i < minI)
                    minI = i;
                if (i > maxI)
                    maxI = i;
                if (j < minJ)
                    minJ = j;
                if (j > maxJ)
                    maxJ = j;

                blocks++;
            }
    }
    
    Mesh* ship = new Mesh("ship");
    ship->InitFromData(vertices, indices);
    meshes["ship"] = ship;

    width = (maxJ - minJ + 1) * shipUnit;
    height = (maxI - minI + 1) * shipUnit;
}

