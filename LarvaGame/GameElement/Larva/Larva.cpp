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
}

void Larva::Stop()
{

}


void Larva::SetLoc(RECT loc)
{
	this->curLoc = loc;
}

void Larva::SetDir(int direction)
{
	this->direction = direction;
}
