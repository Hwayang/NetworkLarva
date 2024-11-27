#pragma once

#include <vector>

#include "GameBoard.h"
#include "../../../engine/random/random.h"

class BoardPiece;
class Larva;
class Item;
class PlanePiece;

//전체 타일의 정보를 담고 있는 클래스
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

    //TODO : Getter으로 빼기
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

    Random<int> tileRandom; // 타일 랜덤 객체
    Random<int> itemRandom; // 아이템 랜덤 객체
    unsigned randomSeed;

    enum Direction
    {
        Left,
        Top,
        Right,
        Bottom
    };

};