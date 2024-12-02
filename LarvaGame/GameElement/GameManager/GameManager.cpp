#include "GameManager.h"

#include <functional>
#include <iostream>

#include "../../engine/random/random.h"
#include "GameBoard/GameBoard.h"
#include "GameState/GameState.h"

GameManager::GameManager()
{
    client = new Client(this->playerCount);

    Random<int> gen(0, 10000); // �õ带 ����� ���� ��ü ����
    int seed = gen();

    bufSize = boardSize * tileSize + 100;

    for (int i = 0; i < 2; i++)
    {
        auto target = std::make_unique<GameBoard>(this->tileSize, this->boardSize, this->bufSize, i, seed);
        this->BoardList.push_back(std::move(target));
    }
}

void GameManager::BoardSetting()
{
    for (auto& board : this->BoardList)
    {
        board->BoardSetting();
    }
}

void GameManager::BoardStateCheck()
{
    for (auto& board : BoardList)
    {
        if (board->StateCheck() == 0)
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
    //std::lock_guard<std::mutex> lock(gameStateMutex); // ����ȭ ó��

    for (auto& board : this->BoardList)
    {
        board->ObjectMove();
    }
}

void GameManager::SetDir(int direction)
{
    // TODO: �������� �޾ƿ� �����ͷ� ����

    std::lock_guard<std::mutex> lock(gameStateMutex); // ����ȭ ó��
    for (auto& board : this->BoardList)
    {
        board->SetDir(direction);
    }
}

void GameManager::UpdateFromNetwork()
{
    gs = client->GetClientInfo();
    //std::lock_guard<std::mutex> lock(this->gameStateMutex);

    // gameState�� ����Ͽ� GameManager�� ���¸� ������Ʈ
    // ���� ���, �÷��̾� ��ġ�� ������Ʈ�մϴ�.
    // �� �κ��� ���� GameState ����ü�� ���� �����ؾ� �մϴ�.
}

