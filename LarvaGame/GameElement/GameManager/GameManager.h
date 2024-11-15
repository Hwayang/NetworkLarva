#pragma once

#include <Windows.h>
#include <vector>

#include "../Larva/Larva.h"
#include "BoardPiece/BoardPiece.h"

//��ü Ÿ���� ������ ��� �ִ� Ŭ����
class GameBoard
{
public:
    std::vector<std::vector<BoardPiece*>> boardLoc;

    int score = 0;
};

//������ ��ü �帧�� �Ѱ��ϴ� Ŭ����
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



