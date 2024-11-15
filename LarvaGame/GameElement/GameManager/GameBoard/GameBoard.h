#pragma once

#include <vector>

#include "GameBoard.h"

class BoardPiece;
class Larva;

//��ü Ÿ���� ������ ��� �ִ� Ŭ����
class GameBoard
{
public:
    GameBoard(int tileSize, int boardSize, int sizebuf);
    std::vector<std::vector<BoardPiece*>> boardLoc;

    int score = 0;

    void LarvaSetting();
    void ObjectMove();
    void BoardSetting();

    void CreateNewLarva();
    void SetDir(int direction);

    void GameOver();
    void TileAction(int tileJudge);

    //TODO : Getter���� ����
    std::vector<Larva*> larvaList;

private:

    void BoardLocationSetting();
    int BoardContentsSetting();

private:
    int actionTarget;
    int Direction;

    int larvaLen = 1;

    int boardSize;
    int tileSize;
    int sizeBuf;

    enum Direction
    {
        Left,
        Top,
        Right,
        Bottom
    };

};