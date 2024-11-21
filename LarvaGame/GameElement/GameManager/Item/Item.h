#pragma once

#include <Windows.h>

class Item
{
public:
    Item(RECT pieceLoc) { this->itemLoc = pieceLoc; }

    virtual ~Item() {};

    RECT GetLocation() { return this->itemLoc; }
    void SetLocation(RECT location);

    COLORREF GetColor() { return this->color; }

    void SetItemJudge(int judgeValue);
    int GetItemJudge() { return this->itemJudge; }

protected:
    int itemJudge = 0;
    int maxCount = 0;
    RECT itemLoc;
    COLORREF color = RGB(0, 0, 0);

};

