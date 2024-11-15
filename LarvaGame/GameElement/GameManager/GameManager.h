#pragma once

#include <Windows.h>
#include <vector>

#include "GameBoard/GameBoard.h"
#include "BoardPiece/BoardPiece.h"
#include "../Larva/Larva.h"

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

private:
    void GameOver();

public:
    std::vector<GameBoard*> BoardList;

    int playerCount = 2;

    int boardSize = 10;
    int tileSize = 30;

    int gameTime = 500;

    int itemSpawnTime = 1000;

private:
    int bufSize = boardSize * tileSize + 100;

};



