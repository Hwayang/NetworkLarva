#pragma once

#include "../GameState/GameState.h"
#include <winsock2.h>

class Client
{
public:
    void connectServer();
    GameState GetClientInfo();

private:
    GameState cGameState;

    void GameLoop(int playerNumber, SOCKET clientSocket, sockaddr_in serverAddr);
    void printLastError(const std::string& message);
    std::string ip = "127.0.0.1"; // 예시를 위해 로컬호스트로 설정
    static constexpr int maxRecvErrorCount = 5;
    std::atomic<bool> running{ true };
};