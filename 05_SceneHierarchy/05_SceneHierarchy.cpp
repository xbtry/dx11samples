// 05_SceneHierarchy.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "05_SceneHierarchy.h"
#include "DXApp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    //LoadLibraryA("WinPixGpuCapturer.dll");
    DXApp app(hInstance);
    return app.Run(nCmdShow);
}