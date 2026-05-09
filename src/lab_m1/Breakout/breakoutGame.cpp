#include <random>
#include "lab_m1/Breakout/breakoutGame.h"
#include <iostream>

using namespace breakoutGame;
using namespace std;

BreakoutGame::BreakoutGame()
{
}

BreakoutGame::~BreakoutGame()
{
}

void BreakoutGame::Init(float X, float Y, float width, float height, float unit,
    std::unordered_map<std::string, Mesh*>& meshes)
{
    this->X = X;
    this->Y = Y;
    this->width = width;
    this->height = height;
    this->unit = unit;

    shipX = 0.5 * (width - shipWidth);
    shipY = Y + 0.5 * unit;

    ballRadius = 0.2 * unit;
    ballX = 0.5 * width;
    ballY = shipY + shipHeight + ballRadius;
    ballAngle = RADIANS(45);

    meshes["ball"] = shapes::circle(glm::vec3(X, Y, 1), ballRadius, 360, "ball", glm::vec3(0.2, 0.7, 0.5), true);

    brickWidth = width / brickColumns;
    brickHeight = (height - unit) / 2 / brickRows;
    brickGridX = X;
    brickGridY = Y + height - brickRows * brickHeight - unit;

    // red border around the top text and hearts
    upperX = X;
    upperY = Y + height - unit;
    meshes["upper_border"] = shapes::rectangle(glm::vec3(0, 0, 0),
        width, unit, "upper_border", glm::vec3(1, 0, 0), false);

    // brickX = brick with a durability of X
    meshes["brick1"] = shapes::rectangle(glm::vec3(0, 0, 0), brickWidth, brickHeight,
        "brick1", glm::vec3(1, 1, 0), true);
    meshes["brick2"] = shapes::rectangle(glm::vec3(0, 0, 0), brickWidth, brickHeight,
        "brick2", glm::vec3(1, 0.5, 0), true);
    meshes["brick3"] = shapes::rectangle(glm::vec3(0, 0, 0), brickWidth, brickHeight,
        "brick3", glm::vec3(0, 1, 1), true);
    meshes["brick4"] = shapes::rectangle(glm::vec3(0, 0, 0), brickWidth, brickHeight,
        "brick4", glm::vec3(0, 0.5, 1), true);
    meshes["brick5"] = shapes::rectangle(glm::vec3(0, 0, 0), brickWidth, brickHeight,
        "brick5", glm::vec3(1, 0, 1), true);

    /*
    * bricks are assigned a durability level based on their location:
    * first row: 1
    * first third of rows: 1 or 2
    * middle third of rows: 2 or 3
    * last third of rows: 3 or 4
    * last row: 4 or 5
    */
    random_device rd;
    mt19937 gen(rd());
    for (int i = 0; i < brickRows; i++)
    {
        for (int j = 0; j < brickColumns; j++)
        {
            if (i == 0)
                brickGrid[i][j] = 1;
            else if (i < brickRows / 3)
            {
                uniform_int_distribution<> distrib(1, 2);
                brickGrid[i][j] = distrib(gen);
            }
            else if (i < 2 * brickRows / 3)
            {
                uniform_int_distribution<> distrib(2, 3);
                brickGrid[i][j] = distrib(gen);
            }
            else if (i < brickRows - 1)
            {
                uniform_int_distribution<> distrib(3, 4);
                brickGrid[i][j] = distrib(gen);
            }
            else
            {
                uniform_int_distribution<> distrib(4, 5);
                brickGrid[i][j] = distrib(gen);
            }
        }
    }

    meshes["filledHeart"] = shapes::heart(glm::vec3(0, 0, 0), 0.75 * unit, 0.75 * unit,
        "filledHeart", glm::vec3(0.8, 0, 0), true);

    meshes["emptyHeart"] = shapes::heart(glm::vec3(0, 0, 0), 0.75 * unit, 0.75 * unit,
        "filledHeart", glm::vec3(0.8, 0, 0), false);

    livesLeft = 3;
    score = 0;
}

void BreakoutGame::gameLogicUpdate(float deltaTimeSceonds)
{
    if (!gameRunning)
        return;

    wallHit();

    shipHit();

    brickHit();

    // updates brick shrinking logic
    for (int i = 0; i < brickRows; i++)
        for (int j = 0; j < brickColumns; j++)
        {
            if (brickGrid[i][j] <= 0)
            {
                if (brickGrid[i][j] > -1)
                    brickGrid[i][j] -= brickShrinkSpeed * deltaTimeSceonds;
                else
                    brickGrid[i][j] = -1;
            }
        }

    ballX += cos(ballAngle) * ballSpeed;
    ballY += sin(ballAngle) * ballSpeed;
}

void BreakoutGame::wallHit()
{
    // check if the ball hit the bottom wall
    if (ballY - ballRadius <= Y)
    {
        livesLeft--;
        gameRunning = false;

        if (livesLeft > 0)
            resetPlayer();
    }

    // check if the ball hit the upper wall
    else if (ballY + ballRadius >= brickGridY + brickHeight * brickRows)
    {
        changeAngleHorizontal();
        ballY = brickGridY + brickHeight * brickRows - ballRadius;  // prevent sticking

    }
    // check if the ball hit the left wall
    else if (ballX <= X)
    {
        changeAngleVertical();
        ballX = X + ballRadius;  // prevent sticking
    }

    // check if the ball hit the right wall
    else if (ballX + ballRadius > X + width)
    {
        changeAngleVertical();
        ballX = X + width - ballRadius;  // prevent sticking
    }
}

void BreakoutGame::shipHit()
{
    // check if the ball hit the ship's top
    if (ballY - ballRadius < shipY + shipHeight &&
        ballY > shipY &&
        ballX >= shipX &&
        ballX <= shipX + shipWidth)
    {
        ballAngle = (ballX - shipX) / (shipWidth / 2);
        ballY = shipY + shipHeight + ballRadius;  // prevent sticking
        return;
    }

    // check if the ball hit the ship's left side
    if (ballY <= shipY + shipHeight &&
            ballY >= shipY &&
            ballX <= shipX &&
            ballX + ballRadius > shipX)
    {
        changeAngleVertical();
        changeAngleHorizontal();
        ballX = shipX - 1.5 * ballRadius;  // prevent sticking
        return;
    }

    // check if the ball hit the ship's right side
    if (ballY <= shipY + shipHeight &&
            ballY >= shipY &&
            ballX >= shipX + shipWidth &&
            ballX - ballRadius < shipX + shipWidth)
    {
        changeAngleVertical();
        changeAngleHorizontal();
        ballX = shipX + shipWidth + 1.5 * ballRadius;  // prevent sticking
        return;
    }
}

void BreakoutGame::brickHit()
{
    // ball contact points
    float left = ballX - ballRadius;
    float right = ballX + ballRadius;
    float bottom = ballY - ballRadius;
    float top = ballY + ballRadius;

    int i, j;

    // check for bottom collision
    i = (bottom - brickGridY) / brickHeight;
    j = (ballX - brickGridX) / brickWidth;
    if (i >= 0 && i < brickRows && j >= 0 && j < brickColumns && brickGrid[i][j] > 0)
    {
        brickGrid[i][j]--;
        changeAngleHorizontal();

        ballY = (brickGridY + (i + 1) * brickHeight) + ballRadius;  // prevent sticking
        raiseScore();
        return;
    }

    // check for left collision
    i = (ballY - brickGridY) / brickHeight;
    j = (left - brickGridX) / brickWidth;
    if (i >= 0 && i < brickRows && j >= 0 && j < brickColumns && brickGrid[i][j] > 0)
    {
        brickGrid[i][j]--;
        changeAngleVertical();

        ballX = (brickGridX + (j + 1) * brickWidth) + ballRadius;  // prevent sticking
        raiseScore();
        return;
    }

    // check for right collision
    i = (ballY - brickGridY) / brickHeight;
    j = (right - brickGridX) / brickWidth;
    if (i >= 0 && i < brickRows && j >= 0 && j < brickColumns && brickGrid[i][j] > 0)
    {
        brickGrid[i][j]--;
        changeAngleVertical();

        ballX = (brickGridX + j * brickWidth) - ballRadius;  // prevent sticking
        raiseScore();
        return;
    }

    // check for top collision
    i = (top - brickGridY) / brickHeight;
    j = (ballX - brickGridX) / brickWidth;
    if (i >= 0 && i < brickRows && j >= 0 && j < brickColumns && brickGrid[i][j] > 0)
    {
        brickGrid[i][j]--;
        changeAngleHorizontal();

        ballY = (brickGridY + i * brickHeight) - ballRadius;  // prevent sticking
        raiseScore();
        return;
    }

}

void BreakoutGame::moveShip(const std::string& direction)
{
    if (direction == "right" && shipX + shipWidth < X + width)
        shipX += shipSpeed;
    if (direction == "left" && shipX > X)
        shipX -= shipSpeed;
}

void BreakoutGame::changeAngleHorizontal()
{
    ballAngle = RADIANS(360) - ballAngle;
    if (ballAngle >= RADIANS(360))
        ballAngle -= RADIANS(360);
    if (ballAngle < 0)
        ballAngle += RADIANS(360);
}

void BreakoutGame::changeAngleVertical()
{
    ballAngle = RADIANS(180) - ballAngle;
    if (ballAngle >= RADIANS(360))
        ballAngle -= RADIANS(360);
    if (ballAngle < 0)
        ballAngle += RADIANS(360);
}

void BreakoutGame::resetPlayer()
{
    shipX = 0.5 * (width - shipWidth);
    shipY = Y + 0.5 * unit;

    ballX = 0.5 * width;
    ballY = shipY + shipHeight + ballRadius;
    ballAngle = RADIANS(45);
}

void BreakoutGame::raiseScore()
{
    bricksLeft--;
    score++;
    if (score > highScore)
        highScore = score;
}