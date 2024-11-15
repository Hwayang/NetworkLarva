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

    void LarvaSetting(GameBoard* targetBoard);

    void CreateNewLarva(GameBoard* targetBoard);


    //TODO : Getter으로 빼기
    std::vector<Larva*> larvaList;

private:
    int actionTarget;
    int Direction;

    int larvaLen = 1;

    enum Direction
    {
        Left,
        Top,
        Right,
        Bottom
    };

};

//게임의 전체 흐름을 총괄하는 클래스
//GameBoard, Larva
class GameManager
{
public:
    GameManager(int PlayerCount);
    
    void Move();
    void BoardSetting();
    void SetDir(int direction);

private:

    void TileAction(GameBoard* targetBoard, int tileJudge);
    void ObjectMove(GameBoard* targetBoard);

    void BoardLocationSetting(GameBoard* targetBoard, int locBuf);
    int BoardContentsSetting(GameBoard* targetBoard);
    

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



