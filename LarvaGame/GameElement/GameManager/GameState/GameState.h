#pragma once

class Player
{
public:
    int playernum;  //1P, 2P�� �˷��ִ� ����
    int playerX;
    int playerY;
    //...
};

class GameState //�ְ� ���� ���� ������ ����ü - ������ ����ü�� �����ϰ� �� ��!
{
public: 
        Player p1;
        Player p2;
        //...
};

