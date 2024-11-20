#include "Larva.h"
#include "../../GameElement/GameManager/GameManager.h"

Larva::Larva(int scale, RECT loc)
{
	this->SetLoc(loc);
}

void Larva::Move()
{
	
}

void Larva::StateChange(int state)
{
	switch (state)
	{
	case 0:
		this->Death();
	}
}

void Larva::SetPrevLarva(Larva* target)
{
	this->prevLarva = target;
}

void Larva::SetNextLarva(Larva* target)
{
	this->nextLarva = target;
}

void Larva::Stop()
{

}

void Larva::Death()
{
	if (!(this->prevLarva == nullptr))
	{
		this->nextLarva->prevLarva = nullptr;
		delete(this);
	}
}

void Larva::SetLoc(RECT loc)
{
	if(this->prevLarva != nullptr)
		prevLarva->SetLoc(this->curLoc);
	 
	this->curLoc = loc;
}

void Larva::SetDir(int direction)
{
	this->direction = direction;
}
