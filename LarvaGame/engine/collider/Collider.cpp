#include "Collider.h"

Collider::Collider(RECT Range) : colliderRange(Range) { }

bool Collider::Collision(Collider targetCollider)
{
	//���ʰ� ������ ��ħ
	if (this->colliderRange.left > targetCollider.GetRect().right)
	{
		return false;
	}
	
	//�����ʰ� ���� ��ħ

	if (targetCollider.GetRect().left > this->colliderRange.right)
	{
		return false;
	}

	//���ʰ� �Ʒ��� ��ħ

	if (targetCollider.GetRect().bottom < this->colliderRange.top)
	{
		return false;
	}
	//�Ʒ��ʰ� ���� ��ħ

	if (this->colliderRange.bottom < targetCollider.GetRect().top)
	{
		return false;
	}

	return true;
}