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
    //���� ������ ��ġ ������ ��� �ִ� ����Ʈ�� ���� �ǽ��� ���� �����͸� ���� 
    this->boardLoc.resize(boardSize, std::vector<BoardPiece*>(boardSize, nullptr));


    for (int i = 0; i < boardSize; i++)
    {
        for (int j = 0; j < boardSize; j++)
        {
            //�� ���� �ǽ������� ��ġ ����
            int left = (sizeBuf * boardNum)  + (i * tileSize);
            int top = (j * tileSize);
            int right = (sizeBuf * boardNum) + (i * tileSize) + tileSize;
            int bottom = (j * tileSize) + tileSize;

            //�ܰ��� �� ��ġ
            if (i == 0 || i == boardSize - 1 || j == 0 || j == boardSize - 1)
            {
                this->boardLoc[i][j] = new WallPiece(RECT{ left, top, right, bottom });
            }
            else
            {
                int tileType = BoardContentsSetting();

                switch (tileType)
                {
                case 1:
                    this->boardLoc[i][j] = new PlanePiece(RECT{ left, top, right, bottom });
                    break;
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

int GameBoard::BoardContentsSetting()
{
    //1 : Plane, 2 : Forest, 3 : Water
    Random<int> gen(1, 3);

    return gen();
}

void GameBoard::LarvaSetting()
{
    this->larvaList[0]->SetLoc(this->boardLoc[1][1]->GetLocation());
}

void GameBoard::ObjectMove()
{
    RECT curLarvaLoc = this->larvaList[0]->GetLoc();

    int xLoc = curLarvaLoc.left / this->tileSize;
    int yLoc = curLarvaLoc.top / this->tileSize;

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

    this->actionTarget = this->boardLoc[xLoc][yLoc]->GetTileJudge();

    this->TileAction(actionTarget);
}

void GameBoard::CreateNewLarva()
{
    RECT lastLarvaLoc = this->larvaList[larvaLen - 1]->GetLoc();
    int direction = this->larvaList[larvaLen - 1]->GetDir();

    int xLoc = lastLarvaLoc.left / this->tileSize;
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

    // ���ο� Larva ����
    Larva* newLarva = new Larva(this->tileSize, this->boardLoc[xLoc][yLoc]->GetLocation());

    // ���� Larva�� ����
    this->larvaList.back()->setPrevLarva(newLarva);

    // ���ο� Larva�� ����Ʈ�� �߰�
    this->larvaList.push_back(newLarva);

    larvaLen++;
}

void GameBoard::SetDir(int direction)
{
    this->Direction = direction;
}

void GameBoard::TileAction( int tileJudge)
{
    enum tile
    {
        Plane = 1,
        Wall,
        Water,
        Forest
    };

    switch (tileJudge)
    {
    case Plane:
        break;

    case Wall:
 //      this->GameOver();
        break;

    case Water:
        break;

    case Forest:
        this->CreateNewLarva();
        break;
    }

}

void GameBoard::GameOver()
{
    for (int i = larvaList.size() - 1; 0 < i; i++)
    {
        larvaList[i]->StateChange(0);
    }

}