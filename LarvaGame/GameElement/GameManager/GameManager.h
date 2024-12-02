#pragma once

#include <Windows.h>
#include <vector>
#include <memory>     // ����Ʈ �����͸� ����ϱ� ���� �߰�
#include <mutex>      // ����ȭ�� ���� �߰�

#include "GameBoard/GameBoard.h"
#include "BoardPiece/BoardPiece.h"
#include "../Larva/Larva.h"
#include "Item/Item.h"
#include "Client/Client.h"

// ������ ��ü �帧�� �Ѱ��ϴ� Ŭ����
class GameManager
{
public:
    GameManager(int playerCount);

    void Move();
    void SetDir(int direction);
    void BoardSetting();

    void BoardStateCheck();

    void UpdateFromNetwork(const GameState& gameState); // ��Ʈ��ũ�κ��� ���� ������ ������Ʈ �Լ� �߰�

    std::mutex gameStateMutex; // ���� ���� ����ȭ�� ���� ���ؽ�

public:
    std::vector<std::unique_ptr<GameBoard>> BoardList;

    int playerCount = 2;

    bool isGameOver = false;

    int boardSize = 15;
    int tileSize = 30;

    int gameTime = 1000;

    int itemSpawnTime = 1000;

    Client client;

private:
    int bufSize = 0; // �ʱ�ȭ�� �����ڿ��� �����ϵ��� ����

private:
    void GameOver();
};

