/*  
    ================================================
    =   /----\  \       /|  ------  |-----|        =
    =  |      | | \    / | /      \ |      |       =
    =  |      | |  \  /  | |------| |-----|        =
    =  |      | |   \/   | |      | |     \        =
    =  \ ____ / |        | |      | |      \       =
    =  [ --> OFFSET: 0x0000 | STATUS: OK ]         =
    =                                              =
    =  [ OMAR ENGINE - Custom Win32 Framework ]    =
    =  [ Developed by: Omar Hawash | (C) 2026 ]    =
    ================================================

*/ 


#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <chrono>
#include "inc/gap.h"
using namespace std;

int screenX = 1280;
int screenY = 720;

int selector = 0;

constexpr double MS_PER_FRAME = 1000.0 / 60.0;

HDC backDC;
HBITMAP backBMP;

vector<char> v;

HBRUSH hbrush_1 = CreateSolidBrush(RGB(255, 255, 255));
HBRUSH hbrush_2 = CreateSolidBrush(RGB(195, 195, 195));

int lines_num = 1;

int hline(HDC backDC) {
    string num_txt = "";

    for (int i = 0; i < lines_num; i++){
        num_txt += to_string(i) + '\n';
    }

    SetTextColor(backDC, RGB(0, 0, 0));
    RECT rect = {0, 0, 32, screenY};

    DrawText(backDC, num_txt.c_str(), -1, &rect, DT_LEFT | DT_TOP);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch (uMsg) 
    {
        case WM_SIZE: {
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);

            if (width > 0 && height > 0) {
                DeleteObject(backBMP);

                HDC hdc = GetDC(hwnd);
                backBMP = CreateCompatibleBitmap(hdc, width, height);
                SelectObject(backDC, backBMP);
                ReleaseDC(hwnd, hdc);

                screenX = width;
                screenY = height;
            }

            break;
        }
    	case WM_DESTROY:
            DeleteObject(hbrush_1);
            DeleteObject(hbrush_2);
            DeleteObject(backBMP);
            DeleteDC(backDC);

            PostQuitMessage(0);
            return 0;
 
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam); 
    }
    return 0; 
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow){
	WNDCLASS wc = {};

	wc.lpfnWndProc   = WndProc;
	wc.hInstance     = hInstance;
	wc.lpszClassName = "90s editor";

	RegisterClass(&wc);

	HWND hwnd = CreateWindowEx(
	    0,
	    "90s editor",
	    "90s editor",
	    WS_OVERLAPPEDWINDOW,

	    CW_USEDEFAULT, CW_USEDEFAULT, screenX, screenY,

	    NULL,
	    NULL,
	    hInstance,
	    NULL
	);

	if (hwnd == NULL){
	    return 0;
	}

	ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    HDC hdc = GetDC(hwnd);

    backDC = CreateCompatibleDC(hdc);
    backBMP = CreateCompatibleBitmap(hdc, screenX, screenY);
    SelectObject(backDC, backBMP);

    /*x
    init
    */

    /*
    init
    */


    MSG msg;

    while (true){
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
            if(msg.message == WM_QUIT) return msg.wParam;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        auto frameStart = std::chrono::high_resolution_clock::now();

        if (GetForegroundWindow() == hwnd) {
            for (int i = 0; i < 255; i++) {
                if (GetAsyncKeyState(i) & 0x0001) {
                    if (i == VK_BACK) {
                        if (!v.empty() && v.size() > selector) {
                            if (v.get_char_at(v.size() - selector - 1) == '\n') {
                                lines_num --;
                            }
                            v.erase(v.size() - selector - 1);
                        }
                    }
                    else if (i == VK_RETURN) {
                        v.insert(v.size() - selector, '\n');
                        lines_num ++;
                    }
                    else if (i == VK_TAB) {
                        for (int i = 0; i < 8; i++) {
                            v.insert(v.end() - selector, ' ');
                        }
                    }
                    else if (i == VK_LEFT) {
                        if (selector < v.size()) {
                            selector ++;
                        }
                    }
                    else if (i == VK_RIGHT) {
                        if (selector > 0) {
                            selector --;
                        }
                    }
                    else {
                        WORD char_buf[2] = {0};

                        BYTE keyState[256];
                        GetKeyboardState(keyState);

                        int result = ToAscii(i, MapVirtualKey(i, MAPVK_VK_TO_VSC), keyState, char_buf, 0);

                        if (result == 1) {
                            v.insert(v.end() - selector, char_buf[0]);
                        }
                    }
                }
            }
        }

        SelectObject(backDC, hbrush_1);
        Rectangle(backDC, 32, 0, screenX, screenY);

        SelectObject(backDC, hbrush_2);
        Rectangle(backDC, 0, 0, 32, screenY);

        SetTextColor(backDC, RGB(0, 0, 0));
        SetBkMode(backDC, TRANSPARENT);
        if(!v.empty()) {
            string s = v.to_string();
            size_t pos = s.size() - selector;
            s.insert(pos, "|");
            RECT rect = { 32, 0, screenX, screenY };
            DrawText(backDC, s.c_str(), -1, &rect, DT_LEFT | DT_TOP);
        }

        hline(backDC);


        BitBlt(hdc, 0, 0, screenX, screenY, backDC, 0, 0, SRCCOPY);

        auto frameEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> frameTime = frameEnd - frameStart;

        if (frameTime.count() < MS_PER_FRAME) {
            Sleep(static_cast<DWORD>(MS_PER_FRAME - frameTime.count()));
        }
    }
}
