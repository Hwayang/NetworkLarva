#include "GameManager.h"

#include <functional>
#include <iostream>

#include "../../engine/random/random.h"
#include "GameBoard/GameBoard.h"
#include "GameState/GameState.h"

GameManager::GameManager()
{
    client = new Client(this->playerCount);

    Random<int> gen(0, 10000); // 시드를 사용한 랜덤 객체 생성
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
    //std::lock_guard<std::mutex> lock(gameStateMutex); // 동기화 처리

    for (auto& board : this->BoardList)
    {
        board->ObjectMove();
    }
}

void GameManager::SetDir(int direction)
{
    // TODO: 서버에서 받아온 데이터로 조정

    std::lock_guard<std::mutex> lock(gameStateMutex); // 동기화 처리
    for (auto& board : this->BoardList)
    {
        board->SetDir(direction);
    }
}

void GameManager::UpdateFromNetwork()
{
    gs = client->GetClientInfo();
    //std::lock_guard<std::mutex> lock(this->gameStateMutex);

    // gameState를 사용하여 GameManager의 상태를 업데이트
    // 예를 들어, 플레이어 위치를 업데이트합니다.
    // 이 부분은 실제 GameState 구조체에 따라 구현해야 합니다.
}

