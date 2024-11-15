#pragma once

#include <Windows.h>

class BoardPiece
{
public:
    BoardPiece(RECT pieceLoc) { this->pieceLoc = pieceLoc; }

    virtual ~BoardPiece() {};
    virtual void PieceAction();

    RECT GetLocation() { return this->pieceLoc; }
    void SetLocation (RECT location);

    COLORREF GetColor() { return this->color; }

    void SetTileJudge(int judgeValue);
    int GetTileJudge() { return this->TileJudge; }

protected:
    int TileJudge = 0;
    RECT pieceLoc;
    COLORREF color = RGB(0,0,0);
};

