#include "stdafx.h"

bool FullScreen = false;

int WINAPI WinMain(HINSTANCE hInstance,    //Main windows function
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd)
{
    // create the window
    if (!InitializeWindow(hInstance, nShowCmd, FullScreen))
    {
        MessageBox(0, L"Window Initialization - Failed",
            L"Error", MB_OK);
        return 1;
    }

    // initialize direct3d
    if (!InitD3D())
    {
        MessageBox(0, L"Failed to initialize direct3d 12",
            L"Error", MB_OK);
        Cleanup();
        return 1;
    }

    // start the main loop
    mainloop();

    // we want to wait for the gpu to finish executing the command list before we start releasing everything
    WaitForPreviousFrame();

    // close the fence event
    CloseHandle(fenceEvent);

    // clean up everything
    Cleanup();

    return 0;
}
