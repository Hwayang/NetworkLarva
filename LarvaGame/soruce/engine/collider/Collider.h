#pragma once

#include <windows.h>

class Collider
{
public:
	Collider(RECT Range);
	RECT GetRect() { return colliderRange; }
	void Move();

private:
	bool Collision(Collider targetCollider);

public:

private:
	RECT colliderRange;
};