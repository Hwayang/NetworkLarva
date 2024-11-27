#pragma once

#include <vector>

#include "GameBoard.h"
#include "../../../engine/random/random.h"

class BoardPiece;
class Larva;
class Item;
class PlanePiece;

//��ü Ÿ���� ������ ��� �ִ� Ŭ����
class GameBoard
{
public:
    GameBoard(int tileSize, int boardSize, int sizebuf, int boardNum, unsigned int seed);
    std::vector<std::vector<BoardPiece*>> boardLoc;

    int score = 0;

    void LarvaSetting();
    void ObjectMove();
    void BoardSetting();

    void CreateNewLarva();
    void DeleteBackLarva(int count);
    void SetDir(int direction);

    void GameOver();
    void TileAction(BoardPiece* target);
    void ItemAction(PlanePiece* targetPiece);

    int StateCheck();

    //TODO : Getter���� ����
    std::vector<Larva*> larvaList;
    std::vector<Item*> ItemList;

private:

    void BoardLocationSetting();
    int BoardTileContentsSetting();
    int BoardItemContentsSetting();

private:
    BoardPiece* actionTarget;
    int Direction;

    bool isGameOver = false;

    int larvaLen = 0;

    int boardSize;
    int tileSize;
    int sizeBuf;
    int boardNum;

    Random<int> tileRandom; // Ÿ�� ���� ��ü
    Random<int> itemRandom; // ������ ���� ��ü
    unsigned randomSeed;

    enum Direction
    {
        Left,
        Top,
        Right,
        Bottom
    };

};