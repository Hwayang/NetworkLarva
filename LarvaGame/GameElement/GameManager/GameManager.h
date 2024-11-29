#pragma once

#include <Windows.h>
#include <vector>

#include "GameBoard/GameBoard.h"
#include "BoardPiece/BoardPiece.h"
#include "../Larva/Larva.h"
#include "Item/Item.h"

class GameBoard;

//게임의 전체 흐름을 총괄하는 클래스
//GameBoard, Larva
class GameManager
{
public:
    GameManager(int PlayerCount);
    
    void Move();
    void SetDir(int direction);
    void BoardSetting();

    void BoardStateCheck();

private:
    void GameOver();

public:
    std::vector<GameBoard*> BoardList;

    int playerCount = 2;

    bool isGameOver = false;

    int boardSize = 15;
    int tileSize = 30;

    int gameTime = 1000;

    int itemSpawnTime = 1000;

private:
    int bufSize = boardSize * tileSize + 100;

};



