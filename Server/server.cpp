#pragma comment(lib, "Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <stdexcept>
#include <mutex>
#include <thread>
#include <queue>
#include <cstring>

#define BUFFER_SIZE 512
#define PORT 9700  // 변경 시 다른 포트 번호 사용 가능
std::string ip = "127.0.0.1";
int timeoutInSeconds = 10;
SOCKET serverSocket;
bool broadcastNow = true;
int broadcastRate = 30;

std::mutex clientMutex;
std::mutex queueMutex;
std::condition_variable cv;

struct Player {
    int playernum = 0;
    int playerX = 0;
    int playerY = 0;
};

struct GameState {
    Player p1;
    Player p2;
};

GameState gameState;
std::vector<std::pair<sockaddr_in, std::chrono::time_point<std::chrono::system_clock>>> clients;
std::queue<std::pair<sockaddr_in, Player>> workQueue;
bool isRunning = true;

void printLastError(const std::string& message) {
    int error = WSAGetLastError();
    std::cerr << message << " Error code: " << error << std::endl;
}

void broadcasterThread(GameState& gameState) {
    while (isRunning) {
        char buffer[BUFFER_SIZE];
        int offset = 0;

        if (broadcastNow) {
            std::memcpy(&buffer[offset], &gameState.p1, sizeof(Player));
            offset += sizeof(Player);
            std::memcpy(&buffer[offset], &gameState.p2, sizeof(Player));
            offset += sizeof(Player);

            broadcastNow = false;
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(broadcastRate));
            continue;
        }

        {
            std::lock_guard<std::mutex> lock(clientMutex);
            for (auto& client : clients) {
                int result = sendto(serverSocket, buffer, offset, 0, (sockaddr*)&client.first, sizeof(client.first));
                if (result == SOCKET_ERROR) {
                    int errorCode = WSAGetLastError();
                    std::cerr << "sendto failed. Error code: " << errorCode
                        << ", Client IP: " << inet_ntoa(client.first.sin_addr)
                        << ", Port: " << ntohs(client.first.sin_port) << std::endl;
                }
                else {
                    std::cout << "Data sent to client. IP: " << inet_ntoa(client.first.sin_addr)
                        << ", Port: " << ntohs(client.first.sin_port) << std::endl;
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(broadcastRate));
    }
    std::cout << "broadcasterThread 종료\n";
}

int main() {
    std::cout << "Server is starting..." << std::endl;

    try {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw std::runtime_error("WSAStartup failed");
        }

        std::cout << "WSAStartup succeeded." << std::endl;

        serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (serverSocket == INVALID_SOCKET) {
            int errorCode = WSAGetLastError();
            std::cerr << "Socket creation failed. Error code: " << errorCode << std::endl;
            throw std::runtime_error("socket failed");
        }

        std::cout << "Socket created successfully." << std::endl;

        // SO_REUSEADDR 옵션 설정
        int optval = 1;
        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval)) == SOCKET_ERROR) {
            std::cerr << "setsockopt failed. Error code: " << WSAGetLastError() << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }

        sockaddr_in serverAddr = {};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(PORT);

        if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0) {
            throw std::runtime_error("Invalid IP address");
        }

        if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            int errorCode = WSAGetLastError();
            std::cerr << "Bind failed. Error code: " << errorCode << std::endl;
            throw std::runtime_error("bind failed");
        }

        std::cout << "Socket bound to port " << PORT << "." << std::endl;

        gameState.p1 = { 1, 70, 100 };
        gameState.p2 = { 2, 270, 100 };

        std::thread broadThread(broadcasterThread, std::ref(gameState));

        std::cin.get(); // 서버 종료 대기
        isRunning = false;

        broadThread.join();

        closesocket(serverSocket);
        WSACleanup();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    return 0;
}
