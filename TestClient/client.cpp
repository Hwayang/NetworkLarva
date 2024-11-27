#pragma comment(lib, "Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <stdexcept>

#define BUFFER_SIZE 512
#define PORT 9700
#define TICK_RATE 500        // ���� Ŭ���� �󵵼��� ������
                             // �󵵼��� �ʹ� ������ �ð����� ����ϴ�. ������ ������Ʈ �ӵ����� 2,3�� ������ ���ִ°� �������ϰ̴ϴ�.
std::string ip = "127.0.0.1";
int maxRecvErrorCount = 20; //recv������ maxRecvErrorCount�̻��̸� ���� ����

/*
����!

���� Ŭ���̾�Ʈ���� ������ ����Ǵ��� �� �� ����


�����ڵ� ����
10035 - ���ҽ� ��ȭ - �̹� �۾����� ���ҽ��� ����
10054 - ������� ���� - ���� Ȥ�� Ŭ���̾�Ʈ�� �������� ����
*/

//�ְ� ���� ���� ������ ����ü - ������ ����ü�� �����ϰ� �� ��!
struct Player {
    int playernum;  //1P, 2P�� �˷��ִ� ����
    int playerX;
    int playerY;
    //...
};
struct GameState {    //���� �ޱ⸸ �Ѵ�.
    Player p1;
    Player p2;
    //...
};

void printLastError(const std::string& message) {
    int error = WSAGetLastError();
    std::cerr << message << " Error code: " << error << std::endl;
}

int main(int argc, char* argv[]) {
    int result;
    int recvErrorCount = 0;

    std::cout << "Client started" << std::endl;

    //���� �ʱ�ȭ
    WSADATA wsaData;
    result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printLastError("WSAStartup failed");
    }

    //UDP ���� ����
    SOCKET CLIENT_LISTocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (CLIENT_LISTocket == INVALID_SOCKET) {
        printLastError("socket failed");
    }

    //������ non-blocking mode�� ����(���� ���ο� ������� ��� �۽�)
    u_long nonBlockingMode = 1;
    if (ioctlsocket(CLIENT_LISTocket, FIONBIO, &nonBlockingMode) != 0) {
        printLastError("ioctlsocket failed");
    }

    //���� �ּҿ� ��Ʈ
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());

    //���� ���� �ʱ�ȭ ������ ��
    //...
    GameState cGameState;

    //1P
    cGameState.p1.playernum = 1;
    cGameState.p1.playerX = 70;
    cGameState.p1.playerY = 100;

    //2P
    cGameState.p2.playernum = 2;
    cGameState.p2.playerX = 270;
    cGameState.p2.playerY = 100;


    //���� ���� �� ����
    std::chrono::milliseconds tickInterval(static_cast<int>(TICK_RATE));
    char buffer[BUFFER_SIZE];
    auto nextTick = std::chrono::steady_clock::now() + tickInterval;

    //�׽�Ʈ�� �÷��̾� ����
    int p;
    std::cin >> p;

    // ���� ���� ����
    int offset;
    while (true) {
        //�÷��̾� �Է�
        //...
        if (p == 1) {   //1P
            cGameState.p1.playerX += 1;
            cGameState.p1.playerY += 1;

            {//�ڵ����� buffer�� ���鿡 �����͸� memcpy
                //�Է°��� ���ۿ� �߰�
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

            {//�ڵ����� buffer�� ���鿡 �����͸� memcpy
                //�Է°��� ���ۿ� �߰�
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

        //������ ����
        result = sendto(CLIENT_LISTocket, buffer, BUFFER_SIZE, 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
        if (result == SOCKET_ERROR) {
            printLastError("input sendto failed");
        }

        //�����κ��� ���� �Է��� ����
        sockaddr_in fromAddr;
        int fromAddrSize = sizeof(fromAddr);
        result = recvfrom(CLIENT_LISTocket, buffer, BUFFER_SIZE, 0, (sockaddr*)&fromAddr, &fromAddrSize);
        if (result == SOCKET_ERROR) {
            printLastError("update recvfrom failed");
            ++recvErrorCount;
        }
        else recvErrorCount = 0;

        //�����κ��� ���� �����͸� �м� �� ���� ����
        //...
        GameState sGameState;       //������ ���� �޴� ���� ����
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


        std::cout << "---------------------------------------------------------\n" << "���� ������Ʈ 1 : " << cGameState.p1.playerX << ",    " << cGameState.p1.playerY << std::endl;
        std::cout << "���� ������Ʈ 2 : " << cGameState.p2.playerX << ",    " << cGameState.p2.playerY << std::endl << std::endl;

        // ���� ƽ�� ��ٸ�
        std::this_thread::sleep_until(nextTick);
        nextTick += tickInterval;

        //�м��� �����͸� ���ӿ� ����
        //...
        if (result != SOCKET_ERROR) {   //�����κ��� �����͸� ���������� �޾Ҵ°�?
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
        //Ŭ���̾�Ʈ������ ó���� ������ ó��(���� ��)
        //...

        //���� ���� ������Ʈ ���
        //...

        std::cout << "���� ���� ������Ʈ 1 : " << cGameState.p1.playerX << ",    " << cGameState.p1.playerY << std::endl;
        std::cout << "���� ���� ������Ʈ 2 : " << cGameState.p2.playerX << ",    " << cGameState.p2.playerY << std::endl << std::endl;

        if (recvErrorCount >= maxRecvErrorCount) {
            printLastError("too much recvError");
            break;     //���� ������ �ǽɵǸ� �ݺ��� ����
        }
    }


    closesocket(CLIENT_LISTocket);
    WSACleanup();
    return 0;
}