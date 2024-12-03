#pragma once

#define WIN32_LEAN_AND_MEAN
#include <winsock.h>
#include <iostream>
#include <chrono>

#include "../GameState/GameState.h"

class Client
{
public:
    Client(int playerCount);
    ~Client();

    void connectServer();
    GameState GetClientInfo();
    void SendThisPlayerInfo(int direction, int score);

private:

    void GameLoop(SOCKET clientSocket, sockaddr_in serverAddr);
    void printLastError(const std::string& message);

private:
    GameState cGameState;
    std::string ip = "127.0.0.1"; // 예시를 위해 로컬호스트로 설정
    
    static constexpr int maxRecvErrorCount = 5;
    std::atomic<bool> running{ true };
};