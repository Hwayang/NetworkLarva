#include "Item.h"

Item::Item(RECT pieceLoc)
{
	this->itemLoc.left = pieceLoc.left + this->sizeCur;
	this->itemLoc.top = pieceLoc.top + this->sizeCur;
	this->itemLoc.right = pieceLoc.right - this->sizeCur;
	this->itemLoc.bottom = pieceLoc.bottom - this->sizeCur;
}

void Item::SetLocation(RECT location)
{
	this->itemLoc = location;
}

void Item::SetItemJudge(int judgeValue)
{
	this->itemJudge = judgeValue;
}
