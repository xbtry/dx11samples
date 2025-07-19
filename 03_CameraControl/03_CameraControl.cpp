// 03_CameraControl.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "03_CameraControl.h"
#include "DXApp.h"
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    DXApp app(hInstance);
    return app.Run(nCmdShow);
}