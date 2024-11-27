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


private:
	void Stop();
	void Move();
	
private:
	RECT targetLoc;
	RECT curLoc;

	COLORREF color = RGB(255, 255, 255);

	int direction = 3;
};