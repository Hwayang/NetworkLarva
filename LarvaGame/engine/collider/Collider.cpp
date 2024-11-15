#include "Collider.h"

Collider::Collider(RECT Range) : colliderRange(Range) { }

bool Collider::Collision(Collider targetCollider)
{
	//왼쪽과 오른쪽 겹침
	if (this->colliderRange.left > targetCollider.GetRect().right)
	{
		return false;
	}
	
	//오른쪽과 왼쪽 겹침

	if (targetCollider.GetRect().left > this->colliderRange.right)
	{
		return false;
	}

	//위쪽과 아래쪽 겹침

	if (targetCollider.GetRect().bottom < this->colliderRange.top)
	{
		return false;
	}
	//아래쪽과 위쪽 겹침

	if (this->colliderRange.bottom < targetCollider.GetRect().top)
	{
		return false;
	}

	return true;
}