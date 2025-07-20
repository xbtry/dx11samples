#pragma once
#ifndef PCH_H
#define PCH_H

#include <windows.h>
#include <windowsx.h>
#include <WinUser.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include "BufferHelpers.h"
#include "CommonStates.h"
#include "DDSTextureLoader.h"
#include "DirectXHelpers.h"
#include "Effects.h"
#include "GamePad.h"
#include "GeometricPrimitive.h"
#include "GraphicsMemory.h"
#include "Keyboard.h"
#include "Model.h"
#include "Mouse.h"
#include "PostProcess.h"
#include "PrimitiveBatch.h"
#include "ScreenGrab.h"
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"
#include "WICTextureLoader.h"
#include <memory>
#include "DirectXHelpers.h"


inline std::wstring StringToWString(const std::string& s)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
    if (len == 0) return L"";
    std::wstring ws(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, &ws[0], len);
    ws.resize(len - 1); // remove null terminator
    return ws;
}


inline std::string WStringToString(const std::wstring& wstr)
{
    if (wstr.empty()) return {};

    // Get required buffer size for UTF-8 string
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
        (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    if (size_needed == 0) return {};

    std::string str(size_needed, 0);

    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(),
        &str[0], size_needed, nullptr, nullptr);
    return str;
}
#endif