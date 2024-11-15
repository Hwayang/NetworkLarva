#pragma once
#include "BoardPiece.h"


class ForestPiece : public BoardPiece
{
public:
	ForestPiece(RECT pieceLoc) : BoardPiece(pieceLoc) { this->color = RGB(0, 255, 0); TileJudge = 4;}
	virtual void PieceAction() override;

private:
};

