#include "BoardPiece.h"

void BoardPiece::PieceAction()
{

}

void BoardPiece::SetLocation(RECT location)
{
	this->pieceLoc = location;
}

void BoardPiece::SetTileJudge(int judgeValue)
{
	this->TileJudge = judgeValue;
}
