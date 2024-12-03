#pragma comment(lib, "Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Client.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <cstring>
#include <vector>

#define BUFFER_SIZE 512
#define PORT 9700
#define TICK_RATE 1000 // 게임 업데이트 속도 (ms)

void Client::printLastError(const std::string& message) {
    int error = WSAGetLastError();
    std::cerr << message << " Error code: " << error << std::endl;
}

Client::Client(int playerCount) {
    for (int i = 0; i < playerCount; i++) {
        this->cGameState.playerList.push_back(new Player{});
    }
}

Client::~Client() {
    for (auto player : cGameState.playerList) {
        delete player;
    }
    cGameState.playerList.clear();
}

void Client::connectServer() {
    int result;

    std::cout << "Client started" << std::endl;

    // 윈속 초기화
    WSADATA wsaData;
    result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printLastError("WSAStartup failed");
        return;
    }

    // UDP 소켓 생성
    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientSocket == INVALID_SOCKET) {
        printLastError("Socket creation failed");
        WSACleanup();
        return;
    }

    // 소켓 비동기 모드 설정
    u_long nonBlockingMode = 1;
    if (ioctlsocket(clientSocket, FIONBIO, &nonBlockingMode) != 0) {
        printLastError("Failed to set non-blocking mode");
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    // 서버 주소 설정
    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());

    // 게임 상태 초기화
    for (size_t i = 0; i < cGameState.playerList.size(); i++) {
        cGameState.playerList[i]->playernum = static_cast<int>(i);
    }

    // 게임 루프 스레드 시작
    std::thread gameThread(&Client::GameLoop, this, clientSocket, serverAddr);

    // 메인 스레드에서 추가 작업 수행 가능
    gameThread.join();

    // 리소스 정리
    closesocket(clientSocket);
    WSACleanup();
}

GameState Client::GetClientInfo() {
    return cGameState;
}

void Client::SendThisPlayerInfo(int direction, int score) {
    cGameState.playerList[0]->direction = direction;
    cGameState.playerList[0]->score = score;
}

void Client::GameLoop(SOCKET clientSocket, sockaddr_in serverAddr) {
    char buffer[BUFFER_SIZE] = {};
    std::chrono::milliseconds tickInterval(TICK_RATE);
    auto nextTick = std::chrono::steady_clock::now() + tickInterval;

    int recvErrorCount = 0;

    while (true) {
        int offset = 0;

        // 클라이언트의 현재 상태를 버퍼에 복사
        std::memcpy(&buffer[offset], &cGameState.playerList[0]->playernum, sizeof(cGameState.playerList[0]->playernum));
        offset += sizeof(cGameState.playerList[0]->playernum);

        std::memcpy(&buffer[offset], &cGameState.playerList[0]->direction, sizeof(cGameState.playerList[0]->direction));
        offset += sizeof(cGameState.playerList[0]->direction);

        std::memcpy(&buffer[offset], &cGameState.playerList[0]->score, sizeof(cGameState.playerList[0]->score));
        offset += sizeof(cGameState.playerList[0]->score);

        // 서버로 데이터 전송
        if (sendto(clientSocket, buffer, offset, 0, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            printLastError("Failed to send data");
        }

        // 서버로부터 데이터 수신
        sockaddr_in fromAddr;
        int fromAddrSize = sizeof(fromAddr);
        int result = recvfrom(clientSocket, buffer, BUFFER_SIZE, 0, (sockaddr*)&fromAddr, &fromAddrSize);

        if (result == SOCKET_ERROR) {
            printLastError("Failed to receive data");
            recvErrorCount++;
        }
        else {
            recvErrorCount = 0;

            // 서버 데이터를 클라이언트 상태에 적용
            int offset = 0;
            GameState sGameState;
            for (size_t i = 0; i < cGameState.playerList.size(); i++) {
                std::memcpy(&sGameState.playerList[i]->playernum, &buffer[offset], sizeof(sGameState.playerList[i]->playernum));
                offset += sizeof(sGameState.playerList[i]->playernum);

                std::memcpy(&sGameState.playerList[i]->direction, &buffer[offset], sizeof(sGameState.playerList[i]->direction));
                offset += sizeof(sGameState.playerList[i]->direction);

                std::memcpy(&sGameState.playerList[i]->score, &buffer[offset], sizeof(sGameState.playerList[i]->score));
                offset += sizeof(sGameState.playerList[i]->score);

                cGameState.playerList[i]->direction = sGameState.playerList[i]->direction;
                cGameState.playerList[i]->score = sGameState.playerList[i]->score;
            }
        }

        // 오류가 일정 횟수 이상 발생하면 종료
        if (recvErrorCount >= maxRecvErrorCount) {
            std::cerr << "Too many receive errors, exiting.\n";
            break;
        }

        // 틱 간격 대기
        std::this_thread::sleep_until(nextTick);
        nextTick += tickInterval;
    }
}
