#pragma once
#ifndef DXAPP_H
#define DXAPP_H

#include "GameTimer.h"
#include "pch.h"

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
	void LoadTexture(const wchar_t* filename);
	void BuildShaders();
	void BuildVertexLayout();
	void BuildGeometryBuffers();
	void BuildConstantBuffers();
	void BuildCamera();
	void UpdateCamera();
	LRESULT DecodeGesture(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	int m_lastMouseX;
	int m_lastMouseY;
	int m_lastTouchX;
	int m_lastTouchY;
	float m_Yaw;
	float m_Pitch;
	float m_Radius;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	UINT m_width, m_height;
	GameTimer m_timer;
	bool m_isPaused = false;
	Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_d3dContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState;

	ID3DBlob* m_vsBlob = nullptr;
	ID3DBlob* m_fsBlob = nullptr;
	ID3DBlob* m_errorBlob = nullptr;
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;
	DirectX::XMMATRIX m_World;
	DirectX::XMMATRIX m_View;
	DirectX::XMMATRIX m_Proj;
};

#endif