#include "GameManager.h"
#include <functional>
#include <iostream>

#include "GameBoard/GameBoard.h"

void GameManager::BoardSetting()
{
    for (int i = 0; i < this->playerCount; i++)
    {
        this->BoardList[i]->BoardSetting();
    }
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
    this->playerCount = PlayerCount;

    for (int i = 0; i < this->playerCount; i++)
    {
        GameBoard* target = new GameBoard(this->tileSize, this->boardSize, this->bufSize);

        this->BoardList.push_back(target);
    }
}


void GameManager::SetDir(int direction)
{
    //TODO: �������� �޾ƿ� �����ͷ� ����

    for (int i = 0; i < this->playerCount; i++)
    {
        this->BoardList[i]->SetDir(direction);
    }
}
