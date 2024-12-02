#pragma once
#include <vector>

class Player
{
public:
    int playernum;  //1P, 2P�� �˷��ִ� ����
    
    int defaultPlayerX;
    int defaultPlayerY;
    
    int direction;
    int score;
    //...
};

class GameState //�ְ� ���� ���� ������ ����ü - ������ ����ü�� �����ϰ� �� ��!
{
public: 
    std::vector<Player*> playerList;
    
    /*
    * GameStateInfo
        GameOver  : 0
        GameStart : 1
    */
    int gameState;
};

