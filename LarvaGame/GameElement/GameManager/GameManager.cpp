#include "GameManager.h"
#include <functional>
#include <iostream>

#include"../../engine/random/random.h"
#include "GameBoard/GameBoard.h"

void GameManager::BoardSetting()
{
    for (int i = 0; i < this->playerCount; i++)
    {
        this->BoardList[i]->BoardSetting();
    }
}

void GameManager::BoardStateCheck()
{
    for (int i = 0; i < playerCount; i++)
    {
        if (BoardList[i]->StateCheck() == 0)
        {
            this->GameOver();
            break;
        }
    }
}

void GameManager::GameOver()
{
    isGameOver = true;
}

void GameManager::Move()
{
    for (int i = 0; i < this->playerCount; i++)
    {
        this->BoardList[i]->ObjectMove();
    }
}

GameManager::GameManager(int PlayerCount)
{
    Random<int> gen(0, 10000); // 시드를 사용한 랜덤 객체 생성

    int seed = gen();

    this->playerCount = PlayerCount;

    for (int i = 0; i < this->playerCount; i++)
    {
        GameBoard* target = new GameBoard(this->tileSize, this->boardSize, this->bufSize, i, seed);

        this->BoardList.push_back(target);
    }
}

void GameManager::SetDir(int direction)
{
    //TODO: 서버에서 받아온 데이터로 조정

    for (int i = 0; i < this->playerCount; i++)
    {
        this->BoardList[i]->SetDir(direction);
    }
}
