#pragma comment(lib, "Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <stdexcept>
#include <mutex>
#include<thread>
#include<queue>

#define BUFFER_SIZE 512
#define PORT 9700
std::string ip = "127.0.0.1";
int timeoutInSeconds = 10;      //timeoutInSeconds초간 응답이 없는 클라이언트는 timeout 처리
SOCKET serverSocket;
bool broadcastNow = true;       //서버 내에서 값이 변하면 전송
int broadcastRate = 30;         //broadcastRate ms(밀리초) 주기로 브로드캐스팅

std::mutex clientMutex;         // 클라이언트 리스트 보호
std::mutex queueMutex;          // 작업 대기열 보호
std::condition_variable cv;     // 대기열 알림


struct Player {
    int playernum;  //1P, 2P를 알려주는 변수
    int playerX;
    int playerY;
    //...
};
struct GameState {     //주고 받을 게임 변수의 구조체 - 서버의 구조체와 동일하게 할 것!
    Player p1;
    Player p2;
    //...
};

GameState gameState;
GameState pGameState;

std::vector<std::pair<sockaddr_in, std::chrono::time_point<std::chrono::system_clock>>> clients;
std::queue<std::pair<sockaddr_in, Player>> workQueue; // 수신된 데이터를 처리할 대기열

bool isRunning = true; // 서버 실행 상태

void printLastError(const std::string& message) {
    int error = WSAGetLastError();
    std::cerr << message << " Error code: " << error << std::endl;
}

void receiverThread() {
    while (isRunning) {
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        char buffer[BUFFER_SIZE];

        int result = recvfrom(serverSocket, buffer, BUFFER_SIZE, 0, (sockaddr*)&clientAddr, &clientAddrSize);
        if (result == SOCKET_ERROR) {
            printLastError("recvfrom failed");
            if (isRunning)
                continue; // 에러 발생 시 처음으로
            else break;     //정상종료를 위해 루프 방치 브레이크
            
        }

        // 수신된 데이터를 Player로 변환
        //...
        Player received;
        int offset = 0;
        std::memcpy(&received.playernum, &buffer[offset], sizeof(received.playernum));      //몇 P인지 분석
        offset += sizeof(received.playernum);
        std::memcpy(&received.playerX, &buffer[offset], sizeof(received.playerX));
        offset += sizeof(received.playerX);
        std::memcpy(&received.playerY, &buffer[offset], sizeof(received.playerY));
        offset += sizeof(received.playerY);

        if (received.playernum == 1)
        {
            if (received.playerX != gameState.p1.playerX || received.playerY != gameState.p1.playerY)
            {
                // 작업 대기열에 추가
                std::lock_guard<std::mutex> lock(queueMutex);
                workQueue.push({ clientAddr, received });
                cv.notify_one(); // 처리 스레드에 알림
            }
        }
        else if (received.playernum == 2)
        {
            if (received.playerX != gameState.p2.playerX || received.playerY != gameState.p2.playerY)
            {
                // 작업 대기열에 추가
                std::lock_guard<std::mutex> lock(queueMutex);
                workQueue.push({ clientAddr, received });
                cv.notify_one(); // 처리 스레드에 알림
            }
        }
        //cv.notify_one(); // 처리 스레드에 알림
    }
    std::cout << "receiverThread 종료\n";
}
void processorThread() {
    while (isRunning) {
        std::pair<sockaddr_in, Player> task;

        // 대기열에서 작업 가져오기
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            cv.wait(lock, [] { return !workQueue.empty() || !isRunning; }); // 작업이 있을 때까지 대기

            task = workQueue.front();   //작업이 들어오면 저장후 pop
            workQueue.pop();
        }
        sockaddr_in clientAddr = task.first;

        // 게임 상태 업데이트
        // ...
        Player tPlayer;
        tPlayer.playernum = task.second.playernum;
        tPlayer.playerX = task.second.playerX;
        tPlayer.playerY = task.second.playerY;

        if (tPlayer.playernum == 1)     //1P
        {
            gameState.p1.playerX = tPlayer.playerX;
            gameState.p1.playerY = tPlayer.playerY;

            broadcastNow = true;    //값이 변했다
        }
        else if (tPlayer.playernum == 2)     //2P
        {
            gameState.p2.playerX = tPlayer.playerX;
            gameState.p2.playerY = tPlayer.playerY;

            broadcastNow = true;    //값이 변했다
        }
        std::cout << "서버 업데이트 플레이어 1 : " << gameState.p1.playerX << ",  " << gameState.p1.playerY << std::endl;
        std::cout << "서버 업데이트 플레이어 2 : " << gameState.p2.playerX << ",  " << gameState.p2.playerY << std::endl << std::endl;

        {
            //클라이언트 리스트 업데이트
            std::lock_guard<std::mutex> lock(clientMutex);
            bool clientExists = false;

            for (auto& client : clients) {
                if (client.first.sin_addr.s_addr == clientAddr.sin_addr.s_addr &&
                    client.first.sin_port == clientAddr.sin_port) {
                    client.second = std::chrono::system_clock::now();    // 마지막 수신 시간 업데이트
                    clientExists = true;
                    break;
                }
            }
            if (!clientExists) {
                clients.push_back({ clientAddr, std::chrono::system_clock::now() });
            }
        }
    }
    std::cout << "processorThread 종료\n";
}
void broadcasterThread(GameState& gameState) {
    while (isRunning) {
        char buffer[BUFFER_SIZE];
        int offset = 0;

        if (broadcastNow)
        {
            //게임 상태를 버퍼에 복사
            //...
            //1P
            std::memcpy(&buffer[offset], &gameState.p1.playernum, sizeof(gameState.p1.playernum));
            offset += sizeof(gameState.p1.playernum);
            std::memcpy(&buffer[offset], &gameState.p1.playerX, sizeof(gameState.p1.playerX));
            offset += sizeof(gameState.p1.playerX);
            std::memcpy(&buffer[offset], &gameState.p1.playerY, sizeof(gameState.p1.playerY));
            offset += sizeof(gameState.p1.playerY);

            //2P
            std::memcpy(&buffer[offset], &gameState.p2.playernum, sizeof(gameState.p2.playernum));
            offset += sizeof(gameState.p2.playernum);
            std::memcpy(&buffer[offset], &gameState.p2.playerX, sizeof(gameState.p2.playerX));
            offset += sizeof(gameState.p2.playerX);
            std::memcpy(&buffer[offset], &gameState.p2.playerY, sizeof(gameState.p2.playerY));
            offset += sizeof(gameState.p2.playerY);

            broadcastNow = false;
        }
        else if(isRunning) continue;  //값이 변하지 않았음!

        {// 클라이언트 리스트에 브로드캐스팅
            std::lock_guard<std::mutex> lock(clientMutex);
            for (auto& client : clients) {
                int result = sendto(serverSocket, buffer, offset, 0, (sockaddr*)&client.first, sizeof(client.first));
                if (result == SOCKET_ERROR) {
                    printLastError("sendto failed");
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(broadcastRate)); // broadcastRate ms 주기로 전송
    }

    std::cout << "broadcasterThread 종료\n";
}
void timeoutCheckerThread() {
    while (isRunning) {
        auto currentTime = std::chrono::system_clock::now();

        {
            std::lock_guard<std::mutex> lock(clientMutex);
            for (auto it = clients.begin(); it != clients.end();) {
                if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - it->second).count() >= timeoutInSeconds) {
                    std::cout << "Client timed out: " << inet_ntoa(it->first.sin_addr) << std::endl;
                    it = clients.erase(it);
                }
                else {
                    ++it;
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(1)); // 1초 주기로 타임아웃 확인
    }
    std::cout << "timeoutCheckerThread 종료\n";
}

int main() {
    std::cout << "Starting server..." << std::endl;

    try {
        //윈속 초기화
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0) {
            throw std::runtime_error("WSAStartup failed");
        }

        //소켓 생성 및 바인딩
        serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (serverSocket == INVALID_SOCKET) {
            throw std::runtime_error("socket failed");
        }
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET; // IPv4
        serverAddr.sin_port = htons(PORT); // 포트 번호 (네트워크 바이트 순서로 변환)
        serverAddr.sin_addr.s_addr = inet_addr(ip.c_str()); // IP 주소 설정

        if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            throw std::runtime_error("bind failed");
        }

        // 게임 상태 초기화
        //....
        //1P
        gameState.p1.playernum = 1;
        gameState.p1.playerX = 70;
        gameState.p1.playerY = 100;

        //2P
        gameState.p2.playernum = 2;
        gameState.p2.playerX = 270;
        gameState.p2.playerY = 100;

        // 스레드 시작
        std::thread recvThread(receiverThread);
        std::thread procThread(processorThread);
        std::thread broadThread(broadcasterThread, std::ref(gameState));
        std::thread timeoutThread(timeoutCheckerThread);


        // 종료 절차
        std::cin.get(); // 사용자 입력 대기
        isRunning = false;

        // 스레드 종료 대기
        recvThread.join();
        procThread.join();
        broadThread.join();
        timeoutThread.join();

        // Cleanup
        closesocket(serverSocket);
        WSACleanup();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    return 0;
}
