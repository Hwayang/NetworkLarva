﻿// WindowsProject1.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

// GIT test comment

#include <iostream>
#include <cassert>
#include <string>
#include <thread>             

// 게임 매니저 include
#include "../GameElement/GameManager/GameManager.h"

#include "framework.h"
#include "LarvaGame.h"


#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LARVAGAME, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LARVAGAME));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_LARVAGAME));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LARVAGAME);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 800, 600, nullptr, nullptr, hInstance, nullptr); // 창 크기 조정

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//

#pragma region AllRegionValue

GameManager gameManager;

HBRUSH MyBrush, OldBrush;

#pragma endregion


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        // 클라이언트 네트워크 통신을 별도의 스레드에서 실행
        std::thread clientThread(&Client::connectServer, gameManager.client);
        clientThread.detach(); // 메인 스레드와 분리하여 백그라운드에서 실행되도록 함

        gameManager.BoardSetting();
        gameManager.SetDir(2);

        SetTimer(hWnd, 1, gameManager.gameTime, NULL);
        SetTimer(hWnd, 2, gameManager.gameTime, NULL);
        SendMessage(hWnd, WM_TIMER, 1, 0);

        break;
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 메뉴 선택을 구문 분석합니다:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        //std::lock_guard<std::mutex> lock(gameManager.gameStateMutex); // 동기화 처리

        for (int i = 0; i < gameManager.playerCount; i++)
        {
            for (int j = 0; j < gameManager.boardSize; j++)
            {
                for (int k = 0; k < gameManager.boardSize; k++)
                {
                    MyBrush = CreateSolidBrush(gameManager.BoardList[i]->boardLoc[j][k]->GetColor());
                    OldBrush = (HBRUSH)SelectObject(hdc, MyBrush);


                    Rectangle(hdc, gameManager.BoardList[i]->boardLoc[j][k]->GetLocation().left,
                        gameManager.BoardList[i]->boardLoc[j][k]->GetLocation().top,
                        gameManager.BoardList[i]->boardLoc[j][k]->GetLocation().right,
                        gameManager.BoardList[i]->boardLoc[j][k]->GetLocation().bottom);

                    SelectObject(hdc, OldBrush);
                    DeleteObject(MyBrush);
                }
            }
        }

        for (int i = 0; i < gameManager.playerCount; i++)
        {
            for (int j = 0; j < gameManager.BoardList[i]->larvaList.size(); j++)
            {
                MyBrush = CreateSolidBrush(gameManager.BoardList[i]->larvaList[j]->GetColor());
                OldBrush = (HBRUSH)SelectObject(hdc, MyBrush);

                Ellipse(hdc, gameManager.BoardList[i]->larvaList[j]->GetLoc().left,
                    gameManager.BoardList[i]->larvaList[j]->GetLoc().top,
                    gameManager.BoardList[i]->larvaList[j]->GetLoc().right,
                    gameManager.BoardList[i]->larvaList[j]->GetLoc().bottom);

                SelectObject(hdc, OldBrush);
                DeleteObject(MyBrush);
            }
        }

        for (int i = 0; i < gameManager.playerCount; i++)
        {
            for (int j = 0; j < gameManager.BoardList[i]->ItemList.size(); j++)
            {
                MyBrush = CreateSolidBrush(gameManager.BoardList[i]->ItemList[j]->GetColor());
                OldBrush = (HBRUSH)SelectObject(hdc, MyBrush);

                Ellipse(hdc, gameManager.BoardList[i]->ItemList[j]->GetLocation().left,
                    gameManager.BoardList[i]->ItemList[j]->GetLocation().top,
                    gameManager.BoardList[i]->ItemList[j]->GetLocation().right,
                    gameManager.BoardList[i]->ItemList[j]->GetLocation().bottom
                );
                SelectObject(hdc, OldBrush);
                DeleteObject(MyBrush);
            }
        }

        int locbuf = 100;

        // 디버그용
        for (int i = 0; i < gameManager.playerCount; i++)
        {
            for (int j = 0; j < gameManager.BoardList[i]->larvaList.size(); j++)
            {
                std::wstring debugText = L"Larva " + std::to_wstring(i) + L": ";

                debugText += L"Left=" + std::to_wstring(gameManager.BoardList[i]->larvaList[j]->GetLoc().left) + L", ";
                debugText += L"Top=" + std::to_wstring(gameManager.BoardList[i]->larvaList[j]->GetLoc().top) + L", ";
                debugText += L"Right=" + std::to_wstring(gameManager.BoardList[i]->larvaList[j]->GetLoc().right) + L", ";
                debugText += L"Bottom=" + std::to_wstring(gameManager.BoardList[i]->larvaList[j]->GetLoc().bottom);

                // DrawText로 화면에 출력
                RECT textRect = { 500, (locbuf * i) + 600, 1500, (locbuf * i) + 700 }; // 텍스트 출력 위치
                DrawText(hdc, debugText.c_str(), -1, &textRect, DT_LEFT | DT_SINGLELINE);

            }
        }

        for (int i = 0; i < gameManager.playerCount; i++)
        {
            std::wstring scoreText = L"Score: " + std::to_wstring(gameManager.BoardList[i]->score);

            RECT textRect = { 500, 0, 1500, 200 };
            DrawText(hdc, scoreText.c_str(), -1, &textRect, DT_CENTER);
        }

        std::wstring debugText = L"GameState: " + std::to_wstring(gameManager.isGameOver);

        RECT textRect = { 500, 500, 1500, 600 };
        DrawText(hdc, debugText.c_str(), -1, &textRect, NULL);

        EndPaint(hWnd, &ps);
    }
    break;

    case WM_TIMER:
    {
        switch (wParam)
        {
            // 전체 게임 타이머
        case 1:
        {
            std::lock_guard<std::mutex> lock(gameManager.gameStateMutex); // 동기화 처리
            gameManager.UpdateFromNetwork();

            gameManager.Move();
            gameManager.BoardStateCheck();
        }
        InvalidateRect(hWnd, NULL, TRUE);
        break;
        // 아이템 세팅 타이머
        case 2:
            break;
        }
        break;
    }
    case WM_KEYDOWN:
    {
        // Direction mapping
        // 0 : left, 1 : up, 2 : right, 3 : bottom
        switch (wParam)
        {
        case VK_LEFT:
            gameManager.SetDir(0);
            break;
        case VK_UP:
            gameManager.SetDir(1);
            break;
        case VK_RIGHT:
            gameManager.SetDir(2);
            break;
        case VK_DOWN:
            gameManager.SetDir(3);
            break;
        }

        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
