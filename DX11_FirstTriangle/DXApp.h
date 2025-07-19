#pragma once
#ifndef DXAPP_H
#define DXAPP_H
#include <windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include "GameTimer.h"

class DXApp
{
public:
	DXApp(HINSTANCE hInstance);
	~DXApp();
	int Run(int nCmdShow);
private:
	bool InitWindow(int nCmdShow);
	bool InitDirect3D();
	void MainLoop();
	void Render();
	void CalculateFrameStats();
	void BuildShaders();
	void BuildVertexLayout();
	void BuildGeometryBuffers();
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	GameTimer m_timer;
	bool m_isPaused = false;
	Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_d3dContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;

	ID3DBlob* m_vsBlob = nullptr;
	ID3DBlob* m_fsBlob = nullptr;
	ID3DBlob* m_errorBlob = nullptr;
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
};

#endif