#pragma once

#include <vector>

#include "GameBoard.h"

class BoardPiece;
class Larva;
class Item;
class PlanePiece;

//전체 타일의 정보를 담고 있는 클래스
class GameBoard
{
public:
    GameBoard(int tileSize, int boardSize, int sizebuf, int boardNum);
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

    int larvaLen = 1;

    int boardSize;
    int tileSize;
    int sizeBuf;
    int boardNum;

    enum Direction
    {
        Left,
        Top,
        Right,
        Bottom
    };

};