#pragma once
#include <vector>

class Player
{
public:
    int playernum;  //1P, 2P를 알려주는 변수
    
    int defaultPlayerX;
    int defaultPlayerY;
    
    int direction;
    int score;
    //...
};

class GameState //주고 받을 게임 변수의 구조체 - 서버의 구조체와 동일하게 할 것!
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

