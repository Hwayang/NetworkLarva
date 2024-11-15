#pragma once

#include "BoardPiece.h"

class WallPiece : public BoardPiece
{
public:
    WallPiece(RECT pieceLoc) : BoardPiece(pieceLoc) { this->color = RGB(93, 89, 83); TileJudge = 2;}
    void PieceAction() override;

private:
};