#pragma once

#include "Item.h"

class Item_LarvaReduce : public Item
{
public:
	Item_LarvaReduce(RECT pieceLoc) : Item(pieceLoc) { this->color = RGB(255, 0, 0); itemJudge = 2; maxCount = 5; }

private:

};
