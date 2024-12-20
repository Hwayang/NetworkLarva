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

GameBoard::GameBoard(int tileSize, int boardSize, int sizebuf, int boardNum, unsigned int seed)
    : randomSeed(seed), tileRandom(1, 3, seed), itemRandom(0, 2, seed + 1) // 시드 초기화
{
    this->tileSize = tileSize;
    this->boardSize = boardSize;
    this->sizeBuf = sizebuf;
    this->boardNum = boardNum;
}

#pragma region randomCreate

int GameBoard::BoardTileContentsSetting()
{
    return tileRandom(); // 동일한 랜덤 객체를 재사용
}

int GameBoard::BoardItemContentsSetting()
{
    return itemRandom(); // 동일한 랜덤 객체를 재사용
}
#pragma endregion

#pragma region defaultSetting

void GameBoard::LarvaSetting()
{
    larvaLen++;
    this->larvaList[0]->SetLoc(this->boardLoc[this->boardSize / 2][this->boardSize / 2]->GetLocation());
}

void GameBoard::BoardLocationSetting()
{
    // 게임 보드의 위치 정보를 담고 있는 리스트에 보드 피스를 담을 포인터를 생성 
    this->boardLoc.resize(boardSize, std::vector<BoardPiece*>(boardSize, nullptr));

    for (int i = 0; i < boardSize; i++)
    {
        for (int j = 0; j < boardSize; j++)
        {
            // 각 보드 피스마다의 위치 설정
            int left = (sizeBuf * boardNum) + (i * tileSize);
            int top = (j * tileSize);
            int right = (sizeBuf * boardNum) + (i * tileSize) + tileSize;
            int bottom = (j * tileSize) + tileSize;

            // 외곽에 벽 설치
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

#pragma endregion

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

    if (larvaLen > 1)
    {
        for (int i = 1; i < larvaList.size(); i++)
        {
            if (this->boardLoc[xLoc][yLoc]->GetLocation().left == larvaList[i]->GetLoc().left &&
                this->boardLoc[xLoc][yLoc]->GetLocation().top == larvaList[i]->GetLoc().top)
            {
                GameOver();
                return;
            }
        }
    }

    for (int i = larvaList.size() - 1; i > 0 ; i--)
    {
        larvaList[i]->SetLoc(larvaList[i - 1]->GetLoc());
        larvaList[i]->SetDir(larvaList[i - 1]->GetDir());
    }

    this->larvaList[0]->SetLoc(this->boardLoc[xLoc][yLoc]->GetLocation());
    this->larvaList[0]->SetDir(this->Direction);

    this->actionTarget = this->boardLoc[xLoc][yLoc];
    this->TileAction(actionTarget);
}

void GameBoard::CreateNewLarva()
{
    RECT lastLarvaLoc = this->larvaList[larvaList.size() - 1]->GetLoc();
    int direction = this->larvaList[larvaList.size() - 1]->GetDir();

    int xLoc = (lastLarvaLoc.left - (boardNum * this->sizeBuf)) / this->tileSize;
    int yLoc = lastLarvaLoc.top / this->tileSize;

    switch (direction)
    {
        case 0:
        {
            if (xLoc + 1 >= this->boardSize - 1)
            {
                if (yLoc <= 0)
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
        }
        case 1:
        {
            if (yLoc >= this->boardSize - 1)
            {
                if (xLoc - 1 <= 0)
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
        }
        case 2:
        {
            if (xLoc <= 0)
            {
                if (yLoc - 1 > 0)
                {
                    yLoc -= 1;
                    xLoc = 0;
                }
            }
            else xLoc -= 1;
            break;
        } 
        case 3:
        {
            if (yLoc <= 0)
            {
                if (xLoc - 1 > 0)
                {
                    xLoc -= 1;
                    yLoc = 0;
                }
            }
            else yLoc -= 1;
            break;
        }
    }

    // 새로운 Larva 생성
    Larva* newLarva = new Larva(this->tileSize, this->boardLoc[xLoc][yLoc]->GetLocation());

    // 새로운 Larva를 리스트에 추가
    this->larvaList.push_back(newLarva);

    larvaLen++;
}

void GameBoard::DeleteBackLarva(int count)
{
    if(this->larvaList.size() <= 1)
    {
        GameOver();
        return;
    }

    for (int i = 0; i < count; i++)
    {
        this->larvaList.erase(larvaList.end() - 1);
        larvaLen--;
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
        dynamic_cast<PlanePiece*>(target)->DeleteItem();
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

    this->score = this->larvaLen;
}

int GameBoard::StateCheck()
{
    if (isGameOver == true)
    {
        return 0;
    }

    return 1;
}

void GameBoard::GameOver()
{
    for (int i = larvaList.size() - 1; 0 < i; i--)
    {
        larvaList[i]->StateChange(0);
    }

    isGameOver = true;
}