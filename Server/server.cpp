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
int timeoutInSeconds = 10;      //timeoutInSeconds�ʰ� ������ ���� Ŭ���̾�Ʈ�� timeout ó��
SOCKET serverSocket;
bool broadcastNow = true;       //���� ������ ���� ���ϸ� ����
int broadcastRate = 30;         //broadcastRate ms(�и���) �ֱ�� ��ε�ĳ����

std::mutex clientMutex;         // Ŭ���̾�Ʈ ����Ʈ ��ȣ
std::mutex queueMutex;          // �۾� ��⿭ ��ȣ
std::condition_variable cv;     // ��⿭ �˸�


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

GameState gameState;
GameState pGameState;

std::vector<std::pair<sockaddr_in, std::chrono::time_point<std::chrono::system_clock>>> clients;
std::queue<std::pair<sockaddr_in, Player>> workQueue; // ���ŵ� �����͸� ó���� ��⿭

bool isRunning = true; // ���� ���� ����

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
                continue; // ���� �߻� �� ó������
            else break;     //�������Ḧ ���� ���� ��ġ �극��ũ
            
        }

        // ���ŵ� �����͸� Player�� ��ȯ
        //...
        Player received;
        int offset = 0;
        std::memcpy(&received.playernum, &buffer[offset], sizeof(received.playernum));      //�� P���� �м�
        offset += sizeof(received.playernum);
        std::memcpy(&received.direction, &buffer[offset], sizeof(received.direction));
        offset += sizeof(received.direction);
        std::memcpy(&received.score, &buffer[offset], sizeof(received.score));
        offset += sizeof(received.score);

        for (int i = 0; i < gameState.playerList.size(); i++)
        {
            if(received.direction != gameState.playerList[i]->direction || received.score != gameState.playerList[i]->score)
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                workQueue.push({ clientAddr, received });
                cv.notify_one(); // ó�� �����忡 �˸�
            }
        }
    }
    std::cout << "receiverThread ����\n";
}
void processorThread() {
    while (isRunning) {
        std::pair<sockaddr_in, Player> task;

        // ��⿭���� �۾� ��������
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            cv.wait(lock, [] { return !workQueue.empty() || !isRunning; }); // �۾��� ���� ������ ���

            task = workQueue.front();   //�۾��� ������ ������ pop
            workQueue.pop();
        }
        sockaddr_in clientAddr = task.first;

        // ���� ���� ������Ʈ
        // ...
        Player tPlayer;
        tPlayer.playernum = task.second.playernum;
        tPlayer.direction = task.second.direction;
        tPlayer.score = task.second.score;

        for (int i = 0; i < gameState.playerList.size(); i++)
        {
            gameState.playerList[i]->direction = tPlayer.direction;
            gameState.playerList[i]->score = tPlayer.score;
            broadcastNow = true;    //���� ���ߴ�

            std::cout << "���� ������Ʈ �÷��̾� 1 : " << gameState.playerList[i]->direction << ",  " << gameState.playerList[i]->score << std::endl;
        }

        {
            //Ŭ���̾�Ʈ ����Ʈ ������Ʈ
            std::lock_guard<std::mutex> lock(clientMutex);
            bool clientExists = false;

            for (auto& client : clients) {
                if (client.first.sin_addr.s_addr == clientAddr.sin_addr.s_addr &&
                    client.first.sin_port == clientAddr.sin_port) {
                    client.second = std::chrono::system_clock::now();    // ������ ���� �ð� ������Ʈ
                    clientExists = true;
                    break;
                }
            }
            if (!clientExists) {
                clients.push_back({ clientAddr, std::chrono::system_clock::now() });
            }
        }
    }
    std::cout << "processorThread ����\n";
}
void broadcasterThread(GameState& gameState) {
    while (isRunning) {
        char buffer[BUFFER_SIZE];
        int offset = 0;

        if (broadcastNow)
        {
            for (int i = 0; i < gameState.playerList.size(); i++)
            {
                std::memcpy(&buffer[offset], &gameState.playerList[i]->playernum, sizeof(gameState.playerList[i]->playernum));
                offset += sizeof(gameState.playerList[i]->playernum);

                std::memcpy(&buffer[offset], &gameState.playerList[i]->direction, sizeof(&gameState.playerList[i]->direction));
                offset += sizeof(&gameState.playerList[i]->direction);

                std::memcpy(&buffer[offset], &gameState.playerList[i]->score, sizeof(gameState.playerList[i]->score));
                offset += sizeof(gameState.playerList[i]->score);
            }

            broadcastNow = false;
        }
        else if(isRunning) continue;  //���� ������ �ʾ���!

        {// Ŭ���̾�Ʈ ����Ʈ�� ��ε�ĳ����
            std::lock_guard<std::mutex> lock(clientMutex);
            for (auto& client : clients) {
                int result = sendto(serverSocket, buffer, offset, 0, (sockaddr*)&client.first, sizeof(client.first));
                if (result == SOCKET_ERROR) {
                    printLastError("sendto failed");
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(broadcastRate)); // broadcastRate ms �ֱ�� ����
    }

    std::cout << "broadcasterThread ����\n";
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

        std::this_thread::sleep_for(std::chrono::seconds(1)); // 1�� �ֱ�� Ÿ�Ӿƿ� Ȯ��
    }
    std::cout << "timeoutCheckerThread ����\n";
}

int main() {
    std::cout << "Starting server..." << std::endl;

    try {
        //���� �ʱ�ȭ
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0) {
            throw std::runtime_error("WSAStartup failed");
        }

        //���� ���� �� ���ε�
        serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (serverSocket == INVALID_SOCKET) {
            throw std::runtime_error("socket failed");
        }
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET; // IPv4
        serverAddr.sin_port = htons(PORT); // ��Ʈ ��ȣ (��Ʈ��ũ ����Ʈ ������ ��ȯ)
        serverAddr.sin_addr.s_addr = inet_addr(ip.c_str()); // IP �ּ� ����

        if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            throw std::runtime_error("bind failed");
        }

        // ���� ���� �ʱ�ȭ
        //....
        //1P
        for (int i = 0; i < gameState.playerList.size(); i++)
        {
            gameState.playerList[i]->playernum = i;
        }

        // ������ ����
        std::thread recvThread(receiverThread);
        std::thread procThread(processorThread);
        std::thread broadThread(broadcasterThread, std::ref(gameState));
        std::thread timeoutThread(timeoutCheckerThread);


        // ���� ����
        std::cin.get(); // ����� �Է� ���
        isRunning = false;

        // ������ ���� ���
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
