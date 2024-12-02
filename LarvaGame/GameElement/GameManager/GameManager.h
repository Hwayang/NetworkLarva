#pragma once

#include "Client/Client.h"

#include <memory>     
#include <mutex>      

#include "GameBoard/GameBoard.h"
#include "BoardPiece/BoardPiece.h"
#include "../Larva/Larva.h"
#include "Item/Item.h"


// 게임의 전체 흐름을 총괄하는 클래스
class GameManager
{
public:
    GameManager();

    void Move();
    void SetDir(int direction);
    void BoardSetting();

    void BoardStateCheck();

    void UpdateFromNetwork(); // 네트워크로부터 받은 데이터 업데이트 함수 추가

    std::mutex gameStateMutex; // 게임 상태 동기화를 위한 뮤텍스

public:
    std::vector<std::unique_ptr<GameBoard>> BoardList;

    int playerCount = 2;

    bool isGameOver = false;

    int boardSize = 15;
    int tileSize = 30;

    int gameTime = 1000;

    int itemSpawnTime = 1000;

    Client* client;
    GameState gs;

private:
    int bufSize = 0; // 초기화를 생성자에서 수행하도록 수정

private:
    void GameOver();
};

