#include "GameManager.h"
#include <functional>
#include <iostream>

#include <../LarvaGame/LarvaGame/engine/random/random.h>
#include <../LarvaGame/LarvaGame/GameElement/GameManager/BoardPiece/PlanePiece.h>
#include <../LarvaGame/LarvaGame/GameElement/GameManager/BoardPiece/WallPiece.h>
#include <../LarvaGame/LarvaGame/GameElement/GameManager/BoardPiece/ForestPiece.h>
#include <../LarvaGame/LarvaGame/GameElement/GameManager/BoardPiece/WaterPiece.h>

void GameManager::BoardSetting()
{

    for (int i = 0; i < this.playerCount; i++)
    {
        BoardLocationSetting(BoardList[i], i);
    }
}

GameManager::GameManager(int PlayerCount)
{
    this->playerCount = PlayerCount;

    this->BoardList = new GameBoard*(new GameBoard (playergameBoard));
    this->BoardList = new GameBoard*(new GameBoard (enemyGameBoard));
}

void GameManager::BoardLocationSetting(GameBoard* targetBoard, int locBuf)
{
    //게임 보드의 위치 정보를 담고 있는 리스트에 보드 피스를 담을 포인터를 생성 
    targetBoard->boardLoc.resize(boardSize, std::vector<BoardPiece*>(boardSize, nullptr));


    for (int i = 0; i < boardSize; i++)
    {
        //외곽에 벽 설치

        for (int j = 0; j < boardSize; j++)
        {
            //각 보드 피스마다의 위치 설정
            int left = (locBuf * bufSize) + (i * tileSize);
            int top = (locBuf * bufSize) + (j * tileSize);
            int right = (locBuf * bufSize) + (i * tileSize) + tileSize;
            int bottom = (locBuf * bufSize) + (j * tileSize) + tileSize;

            //외곽에 벽 설치
            if (i == 0 || i == boardSize - 1 || j == 0 || j == boardSize - 1)
            {
                targetBoard->boardLoc[i][j] = new WallPiece(RECT{ left, top, right, bottom });
            }
            else
            {
                int tileType = BoardContentsSetting(targetBoard);

                switch (tileType)
                {
                case 1:
                    targetBoard->boardLoc[i][j] = new PlanePiece(RECT{ left, top, right, bottom });
                    break;
                case 2:
                    targetBoard->boardLoc[i][j] = new ForestPiece(RECT{ left, top, right, bottom });
                    break;
                case 3:
                    targetBoard->boardLoc[i][j] = new WaterPiece(RECT{ left, top, right, bottom });
                    break;
                }

            }

        }
    }

    this->larvaList.push_back(new Larva(tileSize, RECT{0, 0, tileSize, tileSize}));

    LarvaSetting(targetBoard);
}

int GameManager::BoardContentsSetting(GameBoard* targetBoard)
{
    //1 : Plane, 2 : Forest, 3 : Water
    Random<int> gen(1, 3);

    return gen();
}

void GameManager::LarvaSetting(GameBoard* targetBoard)
{
    larvaList[0]->SetLoc(targetBoard->boardLoc[1][1]->GetLocation());
}

void GameManager::ObjectMove(GameBoard* targetBoard)
{
    RECT curLarvaLoc = this->larvaList[0]->GetLoc();
    
    int xLoc = curLarvaLoc.left / this->tileSize;
    int yLoc = curLarvaLoc.top / this->tileSize;

    //Direction mapping
    // 0 : left, 1 : up, 2 : right, 3 : bottom
    switch (this->Direction)
    {
    case Left:
        if (xLoc - 1 <= 0) { xLoc = 0; }
        else { xLoc -= 1; }
        break;
    case Top:
        if (yLoc - 1 <= 0) { yLoc = 0; }
        else { yLoc -= 1; }
        break;
    case Right:
        if (xLoc + 1 >= this->boardSize - 1) { xLoc = this->boardSize - 1; }
        else { xLoc += 1; }
        break;
    case Bottom:
        if (yLoc + 1 >= this->boardSize - 1) { yLoc = this->boardSize - 1; }
        else { yLoc += 1; }
        break;
    }

    this->larvaList[0]->SetLoc(targetBoard->boardLoc[xLoc][yLoc]->GetLocation());
    
    this->actionTarget = targetBoard->boardLoc[xLoc][yLoc]->GetTileJudge();

    this->TileAction(targetBoard, actionTarget);
}

void GameManager::CreateNewLarva(GameBoard* targetBoard)
{
    RECT lastLarvaLoc = this->larvaList[larvaLen - 1]->GetLoc();
    int direction = this->larvaList[larvaLen - 1]->GetDir();

    int xLoc = lastLarvaLoc.left / this->tileSize;
    int yLoc = lastLarvaLoc.top / this->tileSize;

    switch (direction)
    {
    case 0:
        if (xLoc + 1 >= this->boardSize - 1)
        {
            if (yLoc == 0)
            {
                xLoc = this->boardSize - 1;
                yLoc += 1;
            }
            else
            {
                xLoc = this->boardSize - 1;
                yLoc -= 1;
            }
        }
        else xLoc += 1;
        break;

    case 1:
        if (yLoc >= this->boardSize - 1)
        {
            if (xLoc - 1 == 0)
            {
                xLoc += 1;
                yLoc = this->boardSize - 1;
            }
            else
            {
                xLoc -= 1;
                yLoc = this->boardSize - 1;
            }
        }
        else yLoc += 1;
        break;

    case 2:
        if (xLoc <= 0)
        {
            if (yLoc - 1 == 0)
            {
                xLoc = 0;
                yLoc += 1;
            }
            else
            {
                xLoc = 0;
                yLoc -= 1;
            }
        }
        else xLoc -= 0;
        break;

    case 3:
        if (yLoc <= 0)
        {
            if (xLoc - 1 == 0)
            {
                xLoc += 1;
                yLoc = 0;
            }
            else
            {
                xLoc -= 1;
                yLoc = 0;
            }
        }
        else yLoc -= 0;
        break;
    }

    // 새로운 Larva 생성
    Larva* newLarva = new Larva(this->tileSize, targetBoard->boardLoc[xLoc][yLoc]->GetLocation());

    // 이전 Larva와 연결
    this->larvaList.back()->setPrevLarva(newLarva);

    // 새로운 Larva를 리스트에 추가
    this->larvaList.push_back(newLarva);

    larvaLen++;
}

void GameManager::SetDir(int direction)
{
    this->Direction = direction;
}

void GameManager::TileAction(GameBoard* targetBoard, int tileJudge)
{
    enum tile
    {
        Plane = 1,
        Wall,
        Water,
        Forest
    };

    switch (tileJudge)
    {
    case Plane:
        break;
        
    case Wall:
        this->GameOver();
        break;

    case Water:
        break;

    case Forest:
        this->CreateNewLarva(targetBoard);
        break;
    }

}

void GameManager::GameOver()
{
    for (int i = larvaList.size() - 1; 0 < i; i++)
    {
        larvaList[i]->StateChange(0);
    }

}
