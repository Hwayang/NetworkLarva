#pragma once

#include "Item.h"

class Item_LarvaPlus : public Item
{
public:
	Item_LarvaPlus(RECT pieceLoc) : Item(pieceLoc) { this->color = RGB(93, 89, 83); itemJudge = 1; maxCount = 5; }

private:
	
};

