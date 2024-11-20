#pragma once

#include <Windows.h>

class Larva
{
public:
	Larva(int scale, RECT loc);
	void StateChange(int state);
	void SetLoc(RECT loc);

	int GetDir() { return this->direction; }
	void SetDir(int direction);

	RECT GetLoc() { return this->curLoc; }
	COLORREF GetColor() { return this->color; }

	void SetPrevLarva(Larva* target);
	void SetNextLarva(Larva* target);

private:
	void Stop();
	void Move();
	void Death();
	
private:
	Larva* prevLarva = nullptr;
	Larva* nextLarva = nullptr;
	
	RECT targetLoc;
	RECT curLoc;

	COLORREF color = RGB(255, 0, 0);

	int direction = 3;
};