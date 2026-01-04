#include "framework.h"
#include "Gobang1.0.h"

#include <vector>
#include <algorithm>
#include <limits>
#include <cmath>
#include <chrono>

#define MAX_LOADSTRING 100

// Game constants
const int BOARD_SIZE = 15;
const int CELL_SIZE = 36; // pixels per cell
const int MARGIN = 20; // left/top margin
const int INF = 1000000000;

// small helpers to avoid Win32 min/max macro conflicts
static inline int my_max(int a, int b) { return a > b ? a : b; }
static inline int my_min(int a, int b) { return a < b ? a : b; }

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Board:0 empty,1 human (black),2 ai (white)
static int board[BOARD_SIZE][BOARD_SIZE];
static bool gameOver = false;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void DrawBoard(HDC hdc, RECT& rc);
void DrawStones(HDC hdc);
bool IsInside(int x, int y) { return x >= 0 && y >= 0 && x < BOARD_SIZE && y < BOARD_SIZE; }
int CheckWin(); //0 none,1 human,2 ai

struct Move { int x, y; int score; };

int EvaluatePosition();
int AlphaBeta(int depth, int alpha, int beta, int player);
std::vector<Move> GenerateMoves();

void ResetBoard()
{
    for (int i = 0; i < BOARD_SIZE; ++i)
        for (int j = 0; j < BOARD_SIZE; ++j)
            board[i][j] = 0;
    gameOver = false;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GOBANG10, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GOBANG10));

    MSG msg;

    // initialize board
    ResetBoard();

    // Main message loop:
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



// Registers the window class.
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GOBANG10));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_GOBANG10);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

// Simple directional vectors
static const int DX[4] = { 1,0,1,1 };
static const int DY[4] = { 0,1,1, -1 };

// Evaluate board with simple pattern heuristic
int EvaluatePosition()
{
    // Positive score favors AI (player2), negative favors human (player1)
    auto lineScore = [](int count, int openEnds) -> int {
        if (count >= 5) return 1000000;
        if (count == 4 && openEnds == 2) return 100000;
        if (count == 4 && openEnds == 1) return 10000;
        if (count == 3 && openEnds == 2) return 5000;
        if (count == 3 && openEnds == 1) return 500;
        if (count == 2 && openEnds == 2) return 200;
        if (count == 2 && openEnds == 1) return 50;
        if (count == 1 && openEnds == 2) return 10;
        return 0;
        };

    int score = 0;

    // For both players, scan lines
    for (int player = 1; player <= 2; ++player)
    {
        int playerFactor = (player == 2) ? 1 : -1; // AI positive
        int acc = 0;
        for (int x = 0; x < BOARD_SIZE; ++x)
        {
            for (int y = 0; y < BOARD_SIZE; ++y)
            {
                if (board[x][y] != player) continue;
                // For each direction, count contiguous and open ends
                for (int d = 0; d < 4; ++d)
                {
                    int cnt = 1;
                    int nx = x + DX[d], ny = y + DY[d];
                    while (IsInside(nx, ny) && board[nx][ny] == player) { ++cnt; nx += DX[d]; ny += DY[d]; }
                    int openEnds = 0;
                    if (IsInside(nx, ny) && board[nx][ny] == 0) ++openEnds;
                    nx = x - DX[d]; ny = y - DY[d];
                    if (IsInside(nx, ny) && board[nx][ny] == 0) ++openEnds;
                    acc += lineScore(cnt, openEnds);
                }
            }
        }
        score += acc * playerFactor;
    }
    return score;
}

std::vector<Move> GenerateMoves()
{
    // Generate moves near existing stones to reduce branching
    const int RANGE = 2;
    bool mark[BOARD_SIZE][BOARD_SIZE] = { 0 };
    std::vector<Move> moves;
    for (int x = 0; x < BOARD_SIZE; ++x)
    {
        for (int y = 0; y < BOARD_SIZE; ++y)
        {
            if (board[x][y] != 0)
            {
                for (int dx = -RANGE; dx <= RANGE; ++dx)
                    for (int dy = -RANGE; dy <= RANGE; ++dy)
                    {
                        int nx = x + dx, ny = y + dy;
                        if (IsInside(nx, ny) && board[nx][ny] == 0 && !mark[nx][ny])
                        {
                            mark[nx][ny] = true;
                            moves.push_back({ nx, ny,0 });
                        }
                    }
            }
        }
    }
    // If board is empty, play center
    if (moves.empty()) moves.push_back({ BOARD_SIZE / 2, BOARD_SIZE / 2,0 });

    // quick heuristic ordering: evaluate move immediately and sort descending
    for (auto& m : moves)
    {
        board[m.x][m.y] = 2; // pretend AI
        int s2 = EvaluatePosition();
        board[m.x][m.y] = 1; // pretend human
        int s1 = EvaluatePosition();
        board[m.x][m.y] = 0;
        m.score = s2 - s1;
    }
    std::sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) { return a.score > b.score; });
    return moves;
}

int CheckWin()
{
    for (int x = 0; x < BOARD_SIZE; ++x)
        for (int y = 0; y < BOARD_SIZE; ++y)
            if (board[x][y] != 0)
            {
                int player = board[x][y];
                for (int d = 0; d < 4; ++d)
                {
                    int cnt = 1;
                    int nx = x + DX[d], ny = y + DY[d];
                    while (IsInside(nx, ny) && board[nx][ny] == player) { ++cnt; nx += DX[d]; ny += DY[d]; }
                    nx = x - DX[d]; ny = y - DY[d];
                    while (IsInside(nx, ny) && board[nx][ny] == player) { ++cnt; nx -= DX[d]; ny -= DY[d]; }
                    if (cnt >= 5) return player;
                }
            }
    return 0;
}

int AlphaBeta(int depth, int alpha, int beta, int player)
{
    int winner = CheckWin();
    if (winner == 2) return 1000000;
    if (winner == 1) return -1000000;
    if (depth == 0) return EvaluatePosition();

    auto moves = GenerateMoves();
    if (moves.empty()) return 0;

    if (player == 2) // AI maximizing
    {
        int value = -INF;
        for (auto& m : moves)
        {
            board[m.x][m.y] = 2;
            int score = AlphaBeta(depth - 1, alpha, beta, 1);
            board[m.x][m.y] = 0;
            value = my_max(value, score);
            alpha = my_max(alpha, value);
            if (alpha >= beta) break; // prune
        }
        return value;
    }
    else // human minimizing
    {
        int value = INF;
        for (auto& m : moves)
        {
            board[m.x][m.y] = 1;
            int score = AlphaBeta(depth - 1, alpha, beta, 2);
            board[m.x][m.y] = 0;
            value = my_min(value, score);
            beta = my_min(beta, value);
            if (alpha >= beta) break; // prune
        }
        return value;
    }
}

Move FindBestMove(int depth)
{
    Move best = { -1,-1, -INF };
    auto moves = GenerateMoves();
    for (auto& m : moves)
    {
        board[m.x][m.y] = 2;
        int score = AlphaBeta(depth - 1, -INF + 1, INF - 1, 1);
        board[m.x][m.y] = 0;
        if (score > best.score)
        {
            best = m;
            best.score = score;
        }
    }
    return best;
}

void DrawBoard(HDC hdc, RECT& rc)
{
    // Draw grid
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HGDIOBJ old = SelectObject(hdc, hPen);
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        int x = MARGIN + i * CELL_SIZE;
        MoveToEx(hdc, x, MARGIN, NULL);
        LineTo(hdc, x, MARGIN + (BOARD_SIZE - 1) * CELL_SIZE);
        int y = MARGIN + i * CELL_SIZE;
        MoveToEx(hdc, MARGIN, y, NULL);
        LineTo(hdc, MARGIN + (BOARD_SIZE - 1) * CELL_SIZE, y);
    }
    SelectObject(hdc, old);
    DeleteObject(hPen);
}

void DrawStones(HDC hdc)
{
    for (int x = 0; x < BOARD_SIZE; ++x)
        for (int y = 0; y < BOARD_SIZE; ++y)
        {
            if (board[x][y] == 0) continue;
            int cx = MARGIN + x * CELL_SIZE;
            int cy = MARGIN + y * CELL_SIZE;
            int r = CELL_SIZE / 2 - 2;
            HBRUSH brush = CreateSolidBrush(board[x][y] == 1 ? RGB(0, 0, 0) : RGB(255, 255, 255));
            HBRUSH old = (HBRUSH)SelectObject(hdc, brush);
            Ellipse(hdc, cx - r, cy - r, cx + r, cy + r);
            SelectObject(hdc, old);
            DeleteObject(brush);
            // draw border for white
            if (board[x][y] == 2)
            {
                HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
                HGDIOBJ oldp = SelectObject(hdc, pen);
                Arc(hdc, cx - r, cy - r, cx + r, cy + r, 0, 0, 0, 0);
                SelectObject(hdc, oldp);
                DeleteObject(pen);
            }
        }
}

// Message handler for about box.
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

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    // compute desired client area to fit the board
    int clientWidth = MARGIN *2 + (BOARD_SIZE -1) * CELL_SIZE;
    int clientHeight = MARGIN *2 + (BOARD_SIZE -1) * CELL_SIZE;

    DWORD style = WS_OVERLAPPEDWINDOW;
    RECT wr = {0,0, clientWidth, clientHeight };
    // menu is defined in class, pass TRUE
    AdjustWindowRect(&wr, style, TRUE);
    int winWidth = wr.right - wr.left;
    int winHeight = wr.bottom - wr.top;

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, style,
        CW_USEDEFAULT,0, winWidth, winHeight, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
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
    case WM_LBUTTONDOWN:
        if (!gameOver)
        {
            int mx = (int)(short)(lParam & 0xffff);
            int my = (int)(short)((lParam >> 16) & 0xffff);
            // find nearest grid point
            int gx = (mx - MARGIN + CELL_SIZE / 2) / CELL_SIZE;
            int gy = (my - MARGIN + CELL_SIZE / 2) / CELL_SIZE;
            if (IsInside(gx, gy) && board[gx][gy] ==0)
            {
                board[gx][gy] =1; // human
                InvalidateRect(hWnd, NULL, TRUE);
                // Force immediate paint so human move is visible before AI thinking
                UpdateWindow(hWnd);
                int w = CheckWin();
                if (w ==1) { gameOver = true; MessageBox(hWnd, L"你赢了！", L"游戏结束", MB_OK); break; }

                // AI move
                Move aiMove = FindBestMove(4);
                if (aiMove.x >= 0)
                {
                    board[aiMove.x][aiMove.y] = 2;
                    InvalidateRect(hWnd, NULL, TRUE);
                    int w2 = CheckWin();
                    if (w2 == 2) { gameOver = true; MessageBox(hWnd, L"AI 胜利！", L"游戏结束", MB_OK); }
                }
            }
        }
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc;
        GetClientRect(hWnd, &rc);
        // clear background
        HBRUSH bg = CreateSolidBrush(RGB(230, 200, 160));
        FillRect(hdc, &rc, bg);
        DeleteObject(bg);

        DrawBoard(hdc, rc);
        DrawStones(hdc);

        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
