#pragma once

#define WIN32_LEAN_AND_MEAN
#include <winsock.h>
#include <iostream>
#include <chrono>

#include "../GameState/GameState.h"

class Client
{
public:
    void connectServer();
    GameState GetClientInfo();
    void SendThisPlayerInfo(int direction, int score);

private:

    void GameLoop(int playerNumber, SOCKET clientSocket, sockaddr_in serverAddr);
    void printLastError(const std::string& message);

private:
    GameState cGameState;
    std::string ip = "127.0.0.1"; // ���ø� ���� ����ȣ��Ʈ�� ����
    
    static constexpr int maxRecvErrorCount = 5;
    std::atomic<bool> running{ true };
};