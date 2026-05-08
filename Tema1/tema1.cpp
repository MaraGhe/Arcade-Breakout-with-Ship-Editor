#include "lab_m1/Tema1/tema1.h"
#include "lab_m1/Tema1/transform2D.h"
#include "lab_m1/Tema1/shapes.h"

#include <vector>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema1::Tema1()
{
}


Tema1::~Tema1()
{
}


void Tema1::Init()
{
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)res.x, 0, (float)res.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    unit = (res.x * res.y) / 23000;

    logicSpace.x = unit / 4;       // logic x
    logicSpace.y = unit / 4;       // logic y
    logicSpace.width = res.x - unit / 2;   // logic width
    logicSpace.height = res.y - unit / 2;  // logic height

    // draws a red rectangle around the world border
    Mesh* world_border = shapes::rectangle(glm::vec3(logicSpace.x, logicSpace.y, 0),
        logicSpace.width, logicSpace.height, "world_border", glm::vec3(1, 0, 0), false);
    AddMeshToList(world_border);

    editorScreen.Init(logicSpace.x, logicSpace.y, logicSpace.width, 
        logicSpace.height, unit, meshes);
}

void Tema1::FrameStart()
{
    glLineWidth(1.0f);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Tema1::Update(float deltaTimeSeconds)
{
    viewSpace = ViewportSpace(0, 0, res.x, res.y);
    SetViewportArea(viewSpace, glm::vec3(0, 0, 0), true);

    RenderMesh(meshes["world_border"], shaders["VertexColor"], glm::vec3(0, 0, 0));

    if (!gameStarted)
        EditorUpdate();
    else
        GameUpdate(deltaTimeSeconds);

    glLineWidth(1.0f);
}

void Tema1::EditorUpdate()
{
    // the selection tab on the left
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(editorScreen.selectBlockX - unit, 
        editorScreen.selectBlockY - unit);
    RenderMesh2D(meshes["block_selection_border"], shaders["VertexColor"], modelMatrix);

    modelMatrix *= transform2D::Translate(unit, unit);
    RenderMesh2D(meshes["block"], shaders["VertexColor"], modelMatrix);

    // the upper tab showing how many more blocks can be placed
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(5 * unit, editorScreen.height - 3.5 * unit);
    RenderMesh2D(meshes["green_square_border"], shaders["VertexColor"], modelMatrix);

    modelMatrix *= transform2D::Translate(unit, unit);
    for (int i = 0; i < editorScreen.blocksLeft; i++)
    {
        RenderMesh2D(meshes["green_square"], shaders["VertexColor"], modelMatrix);
        modelMatrix *= transform2D::Translate(2 * unit, 0);
    }

    // the start button
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(editorScreen.startButonX, editorScreen.startButonY);
    if (editorScreen.validBuild)
        RenderMesh2D(meshes["start_button_green"], shaders["VertexColor"], modelMatrix);
    else
        RenderMesh2D(meshes["start_button_red"], shaders["VertexColor"], modelMatrix);

    // the grid
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(editorScreen.gridX - 0.5 * unit,
        editorScreen.gridY - 0.5 * unit);
    RenderMesh2D(meshes["grid_border"], shaders["VertexColor"], modelMatrix);

    modelMatrix *= transform2D::Translate(0.5 * unit, 0.5 * unit);
    glm::mat3 scaleMatrix = glm::mat3(1);
    scaleMatrix *= transform2D::Translate(unit / 2, unit / 2);
    scaleMatrix *= transform2D::Scale(0.8, 0.8);
    scaleMatrix *= transform2D::Translate(- unit / 2, - unit / 2);
    for (int i = 0; i < gridRows; i++)
    {
        for (int j = 0; j < gridColumns; j++)
        {
            if (editorScreen.grid[i][j] == 0)
                RenderMesh2D(meshes["grid_square"], shaders["VertexColor"], 
                    modelMatrix * scaleMatrix);
            else
                RenderMesh2D(meshes["block"], shaders["VertexColor"], modelMatrix);

            modelMatrix *= transform2D::Translate(unit, 0);
        }
        modelMatrix *= transform2D::Translate(-(gridColumns * unit), unit);
    }

    // the moving block
    if (editorScreen.blockMoving)
    {
        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(editorScreen.blockMovingX,
            editorScreen.blockMovingY);
        RenderMesh2D(meshes["block"], shaders["VertexColor"], modelMatrix);
    }
}

void Tema1::GameUpdate(float deltaTimeSeconds)
{
    // the upper section border
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(game.upperX, game.upperY);
    RenderMesh2D(meshes["upper_border"], shaders["VertexColor"], modelMatrix);

    // the hearts in the top right
    modelMatrix *= transform2D::Translate(game.width - unit, 0.15 * unit);
    for (int i = 0; i < 3; i++)
    {
        if (i < game.livesLeft)
            RenderMesh2D(meshes["filledHeart"], shaders["VertexColor"], modelMatrix);
        else
        {
            glLineWidth(4.0f);
            RenderMesh2D(meshes["emptyHeart"], shaders["VertexColor"], modelMatrix);
        }

        modelMatrix *= transform2D::Translate(-unit, 0);
    }

    // the ship
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(game.shipX, game.shipY);
    RenderMesh2D(meshes["ship"], shaders["VertexColor"], modelMatrix);

    // the ball
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(game.ballX, game.ballY);
    RenderMesh2D(meshes["ball"], shaders["VertexColor"], modelMatrix);

    // the bricks
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(game.brickGridX, game.brickGridY);
    glm::mat3 scaleMatrix = glm::mat3(1);
    scaleMatrix *= transform2D::Translate(game.brickWidth / 2, game.brickHeight / 2);
    scaleMatrix *= transform2D::Scale(0.95, 0.95);
    scaleMatrix *= transform2D::Translate(-game.brickWidth / 2, -game.brickHeight / 2);
    for (int i = 0; i < brickRows; i++)
    {
        for (int j = 0; j < brickColumns; j++)
        {
            if (game.brickGrid[i][j] <= 0 && game.brickGrid[i][j] > -1)
            {
                glm::mat3 shrinkMatrix= glm::mat3(1);
                shrinkMatrix *= transform2D::Translate(game.brickWidth / 2, game.brickHeight / 2);
                shrinkMatrix *= transform2D::Scale(1 + game.brickGrid[i][j], 1 + game.brickGrid[i][j]);
                shrinkMatrix *= transform2D::Translate(-game.brickWidth / 2, -game.brickHeight / 2);
                RenderMesh2D(meshes["brick1"], shaders["VertexColor"], modelMatrix * shrinkMatrix * scaleMatrix);
            }
            else if (game.brickGrid[i][j] == 1)
            {
                RenderMesh2D(meshes["brick1"], shaders["VertexColor"], modelMatrix * scaleMatrix);
            }
            else if (game.brickGrid[i][j] == 2)
            {
                RenderMesh2D(meshes["brick2"], shaders["VertexColor"], modelMatrix * scaleMatrix);
            }
            else if (game.brickGrid[i][j] == 3)
            {
                RenderMesh2D(meshes["brick3"], shaders["VertexColor"], modelMatrix * scaleMatrix);
            }
            else if (game.brickGrid[i][j] == 4)
            {
                RenderMesh2D(meshes["brick4"], shaders["VertexColor"], modelMatrix * scaleMatrix);
            }
            else if (game.brickGrid[i][j] == 5)
            {
                RenderMesh2D(meshes["brick5"], shaders["VertexColor"], modelMatrix * scaleMatrix);
            }

            modelMatrix *= transform2D::Translate(game.brickWidth, 0);
        }
        modelMatrix *= transform2D::Translate(-(brickColumns * game.brickWidth), game.brickHeight);
    }


    game.gameLogicUpdate(deltaTimeSeconds);
    if (game.livesLeft == 0)
    {
        gameStarted = false;
        cout << "Game lost! \n Final score: " << game.score << 
            "\n Highest score: " << game.highScore << '\n';
    }

    if (!game.bricksLeft)
    {
        gameStarted = false;
        cout << "Game won! \n Final score: " << game.score <<
            "\n Highest score: " << game.highScore << '\n';
    }
}

void Tema1::SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor, bool clear)
{
    glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    glEnable(GL_SCISSOR_TEST);
    glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
    GetSceneCamera()->Update();
}

void Tema1::FrameEnd()
{

}

void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    if (gameStarted && game.gameRunning)
    {
        if (window->KeyHold(GLFW_KEY_RIGHT))
            game.moveShip("right");
        if (window->KeyHold(GLFW_KEY_LEFT))
            game.moveShip("left");
    }
}


void Tema1::OnKeyPress(int key, int mods)
{
    if (gameStarted && !game.gameRunning && key == GLFW_KEY_SPACE)
    {
        game.gameRunning = true;
    }
}


void Tema1::OnKeyRelease(int key, int mods)
{
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    if (!gameStarted && editorScreen.blockMoving)
    {
        editorScreen.blockMovingX = mouseX - 0.7 * unit;
        editorScreen.blockMovingY = res.y - mouseY - 0.7 * unit;
    }
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    if (!gameStarted)
    {
        if (button == 1)
        {
            if (editorScreen.blockSelected(mouseX, mouseY))
            {
                editorScreen.blockMoving = true;
                editorScreen.blockMovingX = mouseX - 0.7 * unit;
                editorScreen.blockMovingY = res.y - mouseY - 0.7 * unit;
            }
            else if (editorScreen.startSelected(mouseX, res.y - mouseY) && editorScreen.validBuild)
            {
                editorScreen.createShipMesh(meshes, game.shipWidth, game.shipHeight, 0.75);
                gameStarted = true;
                game.Init(logicSpace.x, logicSpace.y, logicSpace.width - logicSpace.x,
                    logicSpace.height - logicSpace.y, unit, meshes);
            }
        }
        if (button == 2)
            editorScreen.removeBlock(mouseX, res.y - mouseY);
    }
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    if (!gameStarted && button == 1 && editorScreen.blockMoving)
    {
        editorScreen.blockMoving = false;
        editorScreen.placeBlock(mouseX, res.y - mouseY);
    }
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema1::OnWindowResize(int width, int height)
{
}
