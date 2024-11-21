#include "PlanePiece.h"

#include "../Item/Item.h"
#include "../Item/Item_LarvaPlus.h"
#include "../Item/Item_LarvaReduce.h"

void PlanePiece::PieceAction()
{

}

void PlanePiece::PieceAction(int itemJudge)
{
	enum ItemEnum
	{
		item_LarvaPlus = 1,
		item_LarvaReduce
	};

	switch (itemJudge)
	{
	case ItemEnum::item_LarvaPlus:
		itemSlot = new Item_LarvaPlus(this->pieceLoc);
		break;

	case ItemEnum::item_LarvaReduce:
		itemSlot = new Item_LarvaReduce(this->pieceLoc);
		break;
	}
}

int PlanePiece::GetItemJudge()
{
	return this->itemSlot->GetItemJudge();
}
