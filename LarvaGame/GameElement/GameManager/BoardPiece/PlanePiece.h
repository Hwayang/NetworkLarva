#pragma once

#include "BoardPiece.h"

class Item;

class PlanePiece : public BoardPiece
{
public:
	PlanePiece(RECT pieceLoc) : BoardPiece(pieceLoc) { this->color = RGB(255, 255, 255); TileJudge = 1; }
	virtual void PieceAction() override;
	virtual void PieceAction(int itemJudge) override;

	int GetItemJudge();

private:
	Item* itemSlot;
};