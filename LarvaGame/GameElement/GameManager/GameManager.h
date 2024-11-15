#pragma once

#include <Windows.h>
#include <vector>

#include "../Larva/Larva.h"
#include "BoardPiece/BoardPiece.h"

//전체 타일의 정보를 담고 있는 클래스
class GameBoard
{
public:
    std::vector<std::vector<BoardPiece*>> boardLoc;

    int score = 0;
};

//게임의 전체 흐름을 총괄하는 클래스
//GameBoard, Larva
class GameManager
{
public:
    GameManager(int PlayerCount);

    void BoardSetting();

    void ObjectMove();

    void SetDir(int direction);

    void CreateNewLarva(GameBoard* targetBoard);
    void GameOver();

private:
    void BoardLocationSetting(GameBoard* targetBoard, int locBuf);
    int BoardContentsSetting(GameBoard* targetBoard);
    void LarvaSetting(GameBoard* targetBoard);

public:
    GameBoard** BoardList;

    GameBoard playergameBoard;
    GameBoard enemyGameBoard;

    int playerCount = 2;

    std::vector<Larva*> larvaList;
    int boardSize = 10;
    int gameTime = 500;

    int itemSpawnTime = 1000;

private:
    int actionTarget;
    int Direction;
    int tileSize = 30;
    int bufSize = boardSize * tileSize + 100;


    int larvaLen = 1;

    enum Direction
    {
        Left,
        Top,
        Right,
        Bottom
    };
};



