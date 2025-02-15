#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN    // Exclude rarely-used stuff from Windows headers.
#endif

#include <windows.h>
#include <windowsx.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include "d3dx12.h"
#include <string>

// this will only call release if an object exists (prevents exceptions calling release on non existant objects)
#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }

// Handle to the window
extern HWND hwnd;

// name of the window (not the title)
extern LPCTSTR WindowName;

// title of the window
extern LPCTSTR WindowTitle;

// is window full screen?
extern bool FullScreen;

extern HANDLE fenceEvent; 

// create a window
bool InitializeWindow(HINSTANCE hInstance,
    int ShowWnd,
    bool fullscreen);

// main application loop
void mainloop();

// callback function for windows messages
LRESULT CALLBACK WndProc(HWND hWnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam);

// function declarations
bool InitD3D(); // initializes direct3d 12

void Update(); // update the game logic

void UpdatePipeline(); // update the direct3d pipeline (update command lists)

void Render(); // execute the command list

void Cleanup(); // release com ojects and clean up memory

void WaitForPreviousFrame(); // wait until gpu is finished with command list
