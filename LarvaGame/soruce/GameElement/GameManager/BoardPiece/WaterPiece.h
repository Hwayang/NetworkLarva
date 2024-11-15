#pragma once

#include "BoardPiece.h"

class WaterPiece : public BoardPiece
{
public:
	WaterPiece(RECT pieceLoc) : BoardPiece(pieceLoc) { this->color = RGB(0, 0, 255); TileJudge = 3; }
	void PieceAction() override;

private:
};

