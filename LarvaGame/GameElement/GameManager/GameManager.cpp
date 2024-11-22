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

void GameManager::Move()
{
    for (int i = 0; i < this->playerCount; i++)
    {
        this->BoardList[i]->ObjectMove();
    }
}

GameManager::GameManager(int PlayerCount)
{
    Random<int> gen(0, 10000); // �õ带 ����� ���� ��ü ����

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
    //TODO: �������� �޾ƿ� �����ͷ� ����

    for (int i = 0; i < this->playerCount; i++)
    {
        this->BoardList[i]->SetDir(direction);
    }
}
