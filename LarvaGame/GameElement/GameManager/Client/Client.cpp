#pragma comment(lib, "Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Client.h"

#include <thread>

#define BUFFER_SIZE 512
#define PORT 9700
#define TICK_RATE 1000        // 수가 클수록 빈도수가 적어짐
                              // 빈도수가 너무 낮으면 시간차가 생깁니다. 게임의 업데이트 속도보다 2,3배 빠르게 해주는게 안정적일겁니다.

/*
주의!

현재 클라이언트측은 서버가 종료되더라도 알 수 없음


에러코드 정리
10035 - 리소스 포화 - 이미 작업중인 리소스가 있음
10054 - 연결되지 않음 - 서버 혹은 클라이언트가 켜져있지 않음
*/

void Client::printLastError(const std::string& message) {
    int error = WSAGetLastError();
    std::cerr << message << " Error code: " << error << std::endl;
}

void Client::connectServer()
{
    int result;
    int recvErrorCount = 0;

    std::cout << "Client started" << std::endl;

    //윈속 초기화
    WSADATA wsaData;
    result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printLastError("WSAStartup failed");
        return;
    }

    //UDP 소켓 생성
    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientSocket == INVALID_SOCKET) {
        printLastError("Socket creation failed");
        WSACleanup();
        return;
    }

    //소켓을 non-blocking mode로 수행(수신 여부와 상관없이 계속 송신)
    u_long nonBlockingMode = 1;
    if (ioctlsocket(clientSocket, FIONBIO, &nonBlockingMode) != 0) {
        printLastError("ioctlsocket failed");
    }

    // Server address and port
    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());

    // Select player number
    int playerNumber;
    std::cout << "Enter player number (1 or 2): ";
    std::cin >> playerNumber;

    //게임 최초 초기화 수행할 것
    //...
    
    for (int i = 0; i < cGameState.playerList.size(); i++)
    {
        cGameState.playerList[i]->playernum = i;
    }

    // Start the game loop in a separate thread
    std::thread gameThread(&Client::GameLoop, this, playerNumber, clientSocket, serverAddr);

    // Main thread can handle other tasks here
    // For example, handling user inputs or GUI updates

    // Wait for the game thread to finish
    gameThread.join();

    // Cleanup
    closesocket(clientSocket);
    WSACleanup();
}

GameState Client::GetClientInfo()
{
    return cGameState;
}

void Client::SendThisPlayerInfo(int direction, int score)
{
    this->cGameState.playerList[0]->direction = direction;
    this->cGameState.playerList[0]->score = score;
}


void Client::GameLoop(int playerNumber, SOCKET clientSocket, sockaddr_in Addr)
{
    int result;
    int recvErrorCount = 0;

    //로직 수행 전 설정
    std::chrono::milliseconds tickInterval(static_cast<int>(TICK_RATE));
    char buffer[BUFFER_SIZE];
    auto nextTick = std::chrono::steady_clock::now() + tickInterval;


    //서버 주소와 포트
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());

    // 게임 로직 수행
    int offset;
    while (true) {
        //플레이어 입력
        //...
        if (p == 1) {   //1P
            cGameState.p1.playerX += 1;
            cGameState.p1.playerY += 1;

            {//자동으로 buffer의 여백에 데이터를 memcpy
                //입력값을 버퍼에 추가
                //...
                offset = 0;
                std::memcpy(&buffer[offset], &cGameState.p1.playernum, sizeof(cGameState.p1.playernum));
                offset += sizeof(cGameState.p1.playernum);
                std::memcpy(&buffer[offset], &cGameState.p1.playerX, sizeof(cGameState.p1.playerX));
                offset += sizeof(cGameState.p1.playerX);
                std::memcpy(&buffer[offset], &cGameState.p1.playerY, sizeof(cGameState.p1.playerY));
                offset += sizeof(cGameState.p1.playerY);

            }
        }
        else if (p == 2) {    //2P
            cGameState.p2.playerX += -1;
            cGameState.p2.playerY += -1;

            {//자동으로 buffer의 여백에 데이터를 memcpy
                //입력값을 버퍼에 추가
                //...
                offset = 0;
                std::memcpy(&buffer[offset], &cGameState.p2.playernum, sizeof(cGameState.p2.playernum));
                offset += sizeof(cGameState.p2.playernum);
                std::memcpy(&buffer[offset], &cGameState.p2.playerX, sizeof(cGameState.p2.playerX));
                offset += sizeof(cGameState.p2.playerX);
                std::memcpy(&buffer[offset], &cGameState.p2.playerY, sizeof(cGameState.p2.playerY));
                offset += sizeof(cGameState.p2.playerY);

            }
        }

        //서버에 전송
        result = sendto(clientSocket, buffer, BUFFER_SIZE, 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
        if (result == SOCKET_ERROR) {
            printLastError("input sendto failed");
        }

        //서버로부터 게임 입력을 받음
        sockaddr_in fromAddr;
        int fromAddrSize = sizeof(fromAddr);
        result = recvfrom(clientSocket, buffer, BUFFER_SIZE, 0, (sockaddr*)&fromAddr, &fromAddrSize);
        if (result == SOCKET_ERROR) {
            printLastError("update recvfrom failed");
            ++recvErrorCount;
        }
        else recvErrorCount = 0;

        //서버로부터 받은 데이터를 분석 후 따로 저장
        //...
        GameState sGameState;       //서버로 부터 받는 게임 정보
        offset = 0;

        //1P
        std::memcpy(&sGameState.p1.playernum, &buffer[offset], sizeof(sGameState.p1.playernum));
        offset += sizeof(sGameState.p1.playernum);
        std::memcpy(&sGameState.p1.playerX, &buffer[offset], sizeof(sGameState.p1.playerX));
        offset += sizeof(sGameState.p1.playerX);
        std::memcpy(&sGameState.p1.playerY, &buffer[offset], sizeof(sGameState.p1.playerY));
        offset += sizeof(sGameState.p1.playerY);

        //2P
        std::memcpy(&sGameState.p2.playernum, &buffer[offset], sizeof(sGameState.p2.playernum));
        offset += sizeof(sGameState.p2.playernum);
        std::memcpy(&sGameState.p2.playerX, &buffer[offset], sizeof(sGameState.p2.playerX));
        offset += sizeof(sGameState.p2.playerX);
        std::memcpy(&sGameState.p2.playerY, &buffer[offset], sizeof(sGameState.p2.playerY));
        offset += sizeof(sGameState.p2.playerY);


        std::cout << "---------------------------------------------------------\n" << "서버 업데이트 1 : " << cGameState.p1.playerX << ",    " << cGameState.p1.playerY << std::endl;
        std::cout << "서버 업데이트 2 : " << cGameState.p2.playerX << ",    " << cGameState.p2.playerY << std::endl << std::endl;

        // 다음 틱을 기다림
        std::this_thread::sleep_until(nextTick);
        nextTick += tickInterval;

        //분석한 데이터를 게임에 적용
        //...
        if (result != SOCKET_ERROR) {   //서버로부터 데이터를 성공적으로 받았는가?
            if (cGameState.p1.playerX != sGameState.p1.playerX ||
                cGameState.p1.playerY != sGameState.p1.playerY)
            {
                cGameState.p1.playerX = sGameState.p1.playerX;
                cGameState.p1.playerY = sGameState.p1.playerY;
            }

            if (cGameState.p2.playerX != sGameState.p2.playerX ||
                cGameState.p2.playerY != sGameState.p2.playerY)
            {
                cGameState.p2.playerX = sGameState.p2.playerX;
                cGameState.p2.playerY = sGameState.p2.playerY;
            }

        }
        //클라이언트에서만 처리할 변경점 처리(점수 등)
        //...

        //로컬 게임 업데이트 출력
        //...

        std::cout << "게임 내부 업데이트 1 : " << cGameState.p1.playerX << ",    " << cGameState.p1.playerY << std::endl;
        std::cout << "게임 내부 업데이트 2 : " << cGameState.p2.playerX << ",    " << cGameState.p2.playerY << std::endl << std::endl;

        if (recvErrorCount >= maxRecvErrorCount) {
            printLastError("too much recvError");
            break;     //연결 끊김이 의심되면 반복문 종료
        }
    }


}


