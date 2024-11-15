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

void Larva::setPrevLarva(Larva* target)
{
	this->prevLarva = target;
}

void Larva::Stop()
{

}

void Larva::Death()
{
	delete(this);
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
