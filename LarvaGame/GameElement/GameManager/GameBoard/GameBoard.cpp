#include "GameBoard.h"

#include "../../../engine/random/random.h"

#include "../BoardPiece/WallPiece.h"
#include "../BoardPiece/ForestPiece.h"
#include "../BoardPiece/PlanePiece.h"
#include "../BoardPiece/WaterPiece.h"

#include "../../Larva/Larva.h"


void GameBoard::BoardSetting()
{
    BoardLocationSetting();
}

GameBoard::GameBoard(int tileSize, int boardSize, int sizebuf, int boardNum)
{
    this->tileSize = tileSize;
    this->boardSize = boardSize;
    this->sizeBuf = sizebuf;
    this->boardNum = boardNum;
}

void GameBoard::BoardLocationSetting()
{
    //게임 보드의 위치 정보를 담고 있는 리스트에 보드 피스를 담을 포인터를 생성 
    this->boardLoc.resize(boardSize, std::vector<BoardPiece*>(boardSize, nullptr));


    for (int i = 0; i < boardSize; i++)
    {
        for (int j = 0; j < boardSize; j++)
        {
            //각 보드 피스마다의 위치 설정
            int left = (sizeBuf * boardNum)  + (i * tileSize);
            int top = (j * tileSize);
            int right = (sizeBuf * boardNum) + (i * tileSize) + tileSize;
            int bottom = (j * tileSize) + tileSize;

            //외곽에 벽 설치
            if (i == 0 || i == boardSize - 1 || j == 0 || j == boardSize - 1)
            {
                this->boardLoc[i][j] = new WallPiece(RECT{ left, top, right, bottom });
            }
            else
            {
                int tileType = BoardTileContentsSetting();

                switch (tileType)
                {
                case 1:
                {
                    this->boardLoc[i][j] = new PlanePiece(RECT{ left, top, right, bottom });
                    this->boardLoc[i][j]->PieceAction(this->BoardItemContentsSetting());

                    PlanePiece* itemTarget = dynamic_cast<PlanePiece*>(this->boardLoc[i][j]);

                    if (itemTarget->GetItem() != nullptr)
                    {
                        ItemList.push_back(itemTarget->GetItem());
                    }

                    break;
                }
                case 2:
                    this->boardLoc[i][j] = new ForestPiece(RECT{ left, top, right, bottom });
                    break;
                case 3:
                    this->boardLoc[i][j] = new WaterPiece(RECT{ left, top, right, bottom });
                    break;
                }

            }

        }
    }

    this->larvaList.push_back(new Larva(tileSize, RECT{ 0, 0, tileSize, tileSize }));

    this->LarvaSetting();
}

int GameBoard::BoardTileContentsSetting()
{
    //1 : Plane, 2 : Forest, 3 : Water
    Random<int> gen(1, 3);

    return gen();
}

int GameBoard::BoardItemContentsSetting()
{
    Random<int> gen(0, 2);

    return gen();
}

void GameBoard::LarvaSetting()
{
    this->larvaList[0]->SetLoc(this->boardLoc[this->boardSize / 2][this->boardSize / 2]->GetLocation());
}

void GameBoard::ObjectMove()
{
    RECT curLarvaLoc = this->larvaList[0]->GetLoc();

    int xLoc = (curLarvaLoc.left - (boardNum * this->sizeBuf)) / this->tileSize;
    int yLoc = curLarvaLoc.top  / this->tileSize;

    //Direction mapping
    // 0 : left, 1 : up, 2 : right, 3 : bottom
    switch (this->Direction)
    {
    case Left:
        if (xLoc - 1 <= 0) { xLoc = 0; }
        else { xLoc -= 1; }
        break;
    case Top:
        if (yLoc - 1 <= 0) { yLoc = 0; }
        else { yLoc -= 1; }
        break;
    case Right:
        if (xLoc + 1 >= this->boardSize - 1) { xLoc = this->boardSize - 1; }
        else { xLoc += 1; }
        break;
    case Bottom:
        if (yLoc + 1 >= this->boardSize - 1) { yLoc = this->boardSize - 1; }
        else { yLoc += 1; }
        break;
    }

    this->larvaList[0]->SetLoc(this->boardLoc[xLoc][yLoc]->GetLocation());

    this->actionTarget = this->boardLoc[xLoc][yLoc];

    this->TileAction(actionTarget);
}

void GameBoard::CreateNewLarva()
{
    RECT lastLarvaLoc = this->larvaList[larvaLen - 1]->GetLoc();
    int direction = this->larvaList[larvaLen - 1]->GetDir();

    int xLoc = (lastLarvaLoc.left - (boardNum * this->sizeBuf)) / this->tileSize;
    int yLoc = lastLarvaLoc.top / this->tileSize;

    switch (direction)
    {
    case 0:
        if (xLoc + 1 >= this->boardSize - 1)
        {
            if (yLoc == 0)
            {
                xLoc = this->boardSize - 1;
                yLoc += 1;
            }
            else
            {
                xLoc = this->boardSize - 1;
                yLoc -= 1;
            }
        }
        else xLoc += 1;
        break;

    case 1:
        if (yLoc >= this->boardSize - 1)
        {
            if (xLoc - 1 == 0)
            {
                xLoc += 1;
                yLoc = this->boardSize - 1;
            }
            else
            {
                xLoc -= 1;
                yLoc = this->boardSize - 1;
            }
        }
        else yLoc += 1;
        break;

    case 2:
        if (xLoc <= 0)
        {
            if (yLoc - 1 > 0)
            {
                yLoc -= 1;
            }
        }
        else
        {
            xLoc -= 1;
        }
        break;

    case 3:
        if (yLoc <= 0)
        {
            if (xLoc - 1 == 0)
            {
                xLoc += 1;
                yLoc = 0;
            }
            else
            {
                xLoc -= 1;
                yLoc = 0;
            }
        }
        else yLoc -= 0;
        break;
    }

    // 새로운 Larva 생성
    Larva* newLarva = new Larva(this->tileSize, this->boardLoc[xLoc][yLoc]->GetLocation());

    // 이전 Larva와 연결
    this->larvaList.back()->SetPrevLarva(newLarva);
    // 새로운 Larva를 리스트에 추가
    this->larvaList.push_back(newLarva);

    larvaLen++;
    // 생성된 Larva를 마지막 Larva에 연결
    this->larvaList[larvaLen - 1]->SetNextLarva(larvaList.back());
}

void GameBoard::DeleteBackLarva(int count)
{
    for (int i = 0; i < count; i++)
    {
        this->larvaList.back()->StateChange(0);
    }
}

void GameBoard::SetDir(int direction)
{
    this->Direction = direction;
}

void GameBoard::TileAction(BoardPiece* target)
{
    enum tile
    {
        Plane = 1,
        Wall,
        Water,
        Forest
    };

    switch (target->GetTileJudge())
    {
    case Plane:
        this->ItemAction(dynamic_cast<PlanePiece*>(target));
        break;

    case Wall:
        this->GameOver();
        break;

    case Water:
        break;

    case Forest:
        break;
    }

}

void GameBoard::ItemAction(PlanePiece* targetPiece)
{
    enum Item
    {
        Item_LarvaPlus = 1,
        Item_LarvaReduce
    };

    switch (targetPiece->GetItemJudge())
    {
    case Item_LarvaPlus:
        this->CreateNewLarva();
        break;
    case Item_LarvaReduce:
        this->DeleteBackLarva(1);
        break;
    }
}

void GameBoard::GameOver()
{
    for (int i = larvaList.size() - 1; 0 < i; i--)
    {
        larvaList[i]->StateChange(0);
    }
}