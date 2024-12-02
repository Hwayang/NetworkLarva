#pragma once

#include <Windows.h>
#include <vector>
#include <memory>     // 스마트 포인터를 사용하기 위해 추가
#include <mutex>      // 동기화를 위해 추가

#include "GameBoard/GameBoard.h"
#include "BoardPiece/BoardPiece.h"
#include "../Larva/Larva.h"
#include "Item/Item.h"
#include "Client/Client.h"

// 게임의 전체 흐름을 총괄하는 클래스
class GameManager
{
public:
    GameManager(int playerCount);

    void Move();
    void SetDir(int direction);
    void BoardSetting();

    void BoardStateCheck();

    void UpdateFromNetwork(const GameState& gameState); // 네트워크로부터 받은 데이터 업데이트 함수 추가

    std::mutex gameStateMutex; // 게임 상태 동기화를 위한 뮤텍스

public:
    std::vector<std::unique_ptr<GameBoard>> BoardList;

    int playerCount = 2;

    bool isGameOver = false;

    int boardSize = 15;
    int tileSize = 30;

    int gameTime = 1000;

    int itemSpawnTime = 1000;

    Client client;

private:
    int bufSize = 0; // 초기화를 생성자에서 수행하도록 수정

private:
    void GameOver();
};

