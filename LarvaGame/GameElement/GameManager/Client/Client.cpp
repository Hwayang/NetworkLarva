#pragma comment(lib, "Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Client.h"

#include <thread>

#define BUFFER_SIZE 512
#define PORT 9700
#define TICK_RATE 1000        // ���� Ŭ���� �󵵼��� ������
                              // �󵵼��� �ʹ� ������ �ð����� ����ϴ�. ������ ������Ʈ �ӵ����� 2,3�� ������ ���ִ°� �������ϰ̴ϴ�.

/*
����!

���� Ŭ���̾�Ʈ���� ������ ����Ǵ��� �� �� ����


�����ڵ� ����
10035 - ���ҽ� ��ȭ - �̹� �۾����� ���ҽ��� ����
10054 - ������� ���� - ���� Ȥ�� Ŭ���̾�Ʈ�� �������� ����
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

    //���� �ʱ�ȭ
    WSADATA wsaData;
    result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printLastError("WSAStartup failed");
        return;
    }

    //UDP ���� ����
    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientSocket == INVALID_SOCKET) {
        printLastError("Socket creation failed");
        WSACleanup();
        return;
    }

    //������ non-blocking mode�� ����(���� ���ο� ������� ��� �۽�)
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

    //���� ���� �ʱ�ȭ ������ ��
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

    //���� ���� �� ����
    std::chrono::milliseconds tickInterval(static_cast<int>(TICK_RATE));
    char buffer[BUFFER_SIZE];
    auto nextTick = std::chrono::steady_clock::now() + tickInterval;


    //���� �ּҿ� ��Ʈ
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());

    // ���� ���� ����
    int offset;
    while (true) {
        //�÷��̾� �Է�
        //...


        //TODO : GameManager���� �������
        cGameState.playerList[0]->direction = 1;
        cGameState.playerList[0]->score = 1;

        {//�ڵ����� buffer�� ���鿡 �����͸� memcpy
            //�Է°��� ���ۿ� �߰�
            //...
            offset = 0;
            std::memcpy(&buffer[offset], &cGameState.playerList[0]->playernum, sizeof(cGameState.playerList[0]->playernum));
            offset += sizeof(cGameState.playerList[0]->playernum);

            std::memcpy(&buffer[offset], &cGameState.playerList[0]->direction, sizeof(cGameState.playerList[0]->direction));
            offset += sizeof(cGameState.playerList[0]->direction);

            std::memcpy(&buffer[offset], &cGameState.playerList[0]->score, sizeof(cGameState.playerList[0]->score));
            offset += sizeof(cGameState.playerList[0]->score);

        }

        //������ ����
        result = sendto(clientSocket, buffer, BUFFER_SIZE, 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
        if (result == SOCKET_ERROR) {
            printLastError("input sendto failed");
        }

        //�����κ��� ���� �Է��� ����
        sockaddr_in fromAddr;
        int fromAddrSize = sizeof(fromAddr);
        result = recvfrom(clientSocket, buffer, BUFFER_SIZE, 0, (sockaddr*)&fromAddr, &fromAddrSize);
        if (result == SOCKET_ERROR) {
            printLastError("update recvfrom failed");
            ++recvErrorCount;
        }
        else recvErrorCount = 0;

        //�����κ��� ���� �����͸� �м� �� ���� ����
        //...
        GameState sGameState;       //������ ���� �޴� ���� ����
        offset = 0;

        for (int i = 0; i < cGameState.playerList.size(); i++)
        {
            std::memcpy(&sGameState.playerList[i]->playernum, &buffer[offset], sizeof(sGameState.playerList[i]->playernum));
            offset += sizeof(sGameState.playerList[i]->playernum);

            std::memcpy(&sGameState.playerList[i]->direction, &buffer[offset], sizeof(sGameState.playerList[i]->direction));
            offset += sizeof(sGameState.playerList[i]->direction);

            std::memcpy(&sGameState.playerList[i]->score, &buffer[offset], sizeof(sGameState.playerList[i]->score));
            offset += sizeof(sGameState.playerList[i]->score);

            std::cout << "---------------------------------------------------------\n" << "���� ������Ʈ" << 1 << " : " << cGameState.playerList[i]->direction << ", " << cGameState.playerList[i]->score << std::endl;
        }

        // ���� ƽ�� ��ٸ�
        std::this_thread::sleep_until(nextTick);
        nextTick += tickInterval;

        //�м��� �����͸� ���ӿ� ����
        //...
        if (result != SOCKET_ERROR) {   //�����κ��� �����͸� ���������� �޾Ҵ°�?

            for (int i = 0; i < cGameState.playerList.size(); i++)
            {
                if (cGameState.playerList[i]->direction != sGameState.playerList[i]->direction || cGameState.playerList[i]->score != sGameState.playerList[i]->score)
                {
                    cGameState.playerList[i]->direction = sGameState.playerList[i]->direction;
                    cGameState.playerList[i]->score = sGameState.playerList[i]->score;
                }
            }
        }
        //Ŭ���̾�Ʈ������ ó���� ������ ó��(���� ��)
        //...

        //���� ���� ������Ʈ ���
        //...

        for (int i = 0; i < cGameState.playerList.size(); i++)
        {
            std::cout << "Ŭ���̾�Ʈ ������Ʈ �÷��̾�" << i << " : " << cGameState.playerList[i]->direction << ",  " << gameState.playerList[i]->score << std::endl;
        }

        if (recvErrorCount >= maxRecvErrorCount) {
            printLastError("too much recvError");
            break;     //���� ������ �ǽɵǸ� �ݺ��� ����
        }
    }


}


