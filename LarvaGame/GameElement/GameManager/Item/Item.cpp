#include "Item.h"

void Item::SetLocation(RECT location)
{
	this->itemLoc = location;
}

void Item::SetItemJudge(int judgeValue)
{
	this->itemJudge = judgeValue;
}
