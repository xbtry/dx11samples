#include "DXApp.h"
#include <stdexcept>
#include <sstream>
#include <algorithm>
int wmId, wmEvent, i, x, y;

UINT cInputs;
PTOUCHINPUT pInputs;
POINT ptInput;
#define MAXPOINTS 10
// You will use this array to switch the track ids
int idLookup[MAXPOINTS];


// You will use this array to track touch points
int points[MAXPOINTS][2];

int GetContactIndex(int dwID);
BOOL RemoveContactIndex(int index);


#define WINDOW_CLASS_NAME L"DXAppWindowClass"
#define WINDOW_TITLE L"DirectX 11 Application"

struct MatrixBufferType {
	DirectX::XMMATRIX World;
	DirectX::XMMATRIX View;
	DirectX::XMMATRIX Projection;
};

DXApp* g_app = nullptr;

DXApp::DXApp(HINSTANCE hInstance) : m_hInstance(hInstance), m_hWnd(nullptr)
{
	g_app = this;
}

DXApp::~DXApp()
{
	
}

int DXApp::Run(int nCmdShow)
{
	if (!InitWindow(nCmdShow))
	{
		return -1;
	}
	if (!InitDirect3D())
	{
		return -1;
	}
	MainLoop();
	return 0;
}

bool DXApp::InitWindow(int nCmdShow)
{
	WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = m_hInstance;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = WINDOW_CLASS_NAME;
	if (!RegisterClassEx(&wc))
	{
		return false;
	}
	m_hWnd = CreateWindowEx(0, WINDOW_CLASS_NAME, WINDOW_TITLE, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, nullptr, nullptr, m_hInstance, nullptr);
	if (!m_hWnd)
	{
		return false;
	}

	// Check if touch input is available
	int digitizerStatus = GetSystemMetrics(SM_DIGITIZER);
	if (digitizerStatus & NID_READY)
	{
		if (digitizerStatus & NID_MULTI_INPUT)
		{
			OutputDebugString(L"Multi-touch is supported.\n");
		}
		else
		{
			OutputDebugString(L"Touch is supported, but not multi-touch.\n");
		}
	}
	else
	{
		OutputDebugString(L"Touch input not available.\n");
	}

	GESTURECONFIG gestureConfig = {0};
	gestureConfig.dwID = 0; // Enable zoom and pan gestures
	gestureConfig.dwWant = GC_ALLGESTURES;
	gestureConfig.dwBlock = 0;
	if (!SetGestureConfig(m_hWnd, 0, 1, &gestureConfig, sizeof(GESTURECONFIG)))
	{
		OutputDebugString(L"Failed to set gesture configuration.\n");
	}

	ShowWindow(m_hWnd, nCmdShow);
	UpdateWindow(m_hWnd);
	return true;
}

bool DXApp::InitDirect3D()
{
	RECT clientRect;
	GetClientRect(m_hWnd, &clientRect);
	UINT width = clientRect.right - clientRect.left;
	UINT height = clientRect.bottom - clientRect.top;

	HRESULT hr = D3D11CreateDevice(
		nullptr, 
		D3D_DRIVER_TYPE_HARDWARE, 
		nullptr,
		0, // FOR DEBUG -> D3D11_CREATE_DEVICE_DEBUG,
		nullptr, 
		0, 
		D3D11_SDK_VERSION,
		&m_d3dDevice,
		nullptr, 
		&m_d3dContext
	);
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to create Direct3D device.");
	}
	Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
	hr = m_d3dDevice.As(&dxgiDevice);
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to get DXGI device.");
	}
	Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
	hr = dxgiDevice->GetAdapter(&adapter);
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to get DXGI adapter.");
	}
	Microsoft::WRL::ComPtr<IDXGIFactory> factory;
	hr = adapter->GetParent(IID_PPV_ARGS(&factory));
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to get DXGI factory.");
	}
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	D3D11_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_BACK; // disable backface culling
	rasterDesc.DepthClipEnable = TRUE;

	hr = m_d3dDevice->CreateRasterizerState(&rasterDesc, &m_rasterizerState);
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to create rasterizer state");
	}


	// Set it immediately
	m_d3dContext->RSSetState(m_rasterizerState.Get());

	/*D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.StencilEnable = true;
	m_d3dDevice->CreateDepthStencilState(&dsDesc, &m_depthState);
	m_d3dContext->OMSetDepthStencilState(m_depthState.Get(), 0);*/

	/*D3D11_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_d3dDevice->CreateBlendState(&blendDesc, &m_blendState);
	m_d3dContext->OMSetBlendState(m_blendState.Get(), nullptr, 0xFFFFFFFF);*/

	BuildConstantBuffers();
	BuildCamera();

	m_rootNode = std::make_unique<SceneNode>();
	//TriangleNode* triangleNode = new TriangleNode(m_d3dDevice.Get());
	//m_rootNode->AddChild(std::shared_ptr<TriangleNode>(triangleNode));
	CubeNode* cubeNode = new CubeNode(m_d3dDevice.Get());
	m_rootNode->AddChild(std::shared_ptr<CubeNode>(cubeNode));
	//std::make_shared<SphereNode>(m_d3dDevice.Get());
	//m_rootNode->AddChild(std::make_shared<SphereNode>(m_d3dDevice.Get()));
	UINT qualityLevels = 0;
	m_d3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &qualityLevels);
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = width;
	swapChainDesc.BufferDesc.Height = height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = m_hWnd;
	swapChainDesc.SampleDesc.Count = 4;
	swapChainDesc.SampleDesc.Quality = qualityLevels > 0 ? qualityLevels - 1 : 0;
	swapChainDesc.Windowed = TRUE;

	hr = factory->CreateSwapChain(m_d3dDevice.Get(), &swapChainDesc, &m_swapChain);
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to create swap chain.");
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to get back buffer.");
	}

	hr = m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_renderTargetView);
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to create render target view.");
	}

	D3D11_TEXTURE2D_DESC depthStencilDesc = {};
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 4;
	depthStencilDesc.SampleDesc.Quality = qualityLevels > 0 ? qualityLevels - 1 : 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	ID3D11Texture2D* depthStencilBuffer = nullptr;
	ID3D11DepthStencilView* depthStencilView = nullptr;

	hr = m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilBuffer);
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to create depth stencil buffer.");
	}

	hr = m_d3dDevice->CreateDepthStencilView(depthStencilBuffer, nullptr, &depthStencilView);
	if (FAILED(hr))
	{
		depthStencilBuffer->Release();
		throw std::runtime_error("Failed to create depth stencil view.");
	}

	m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), depthStencilView);
	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<FLOAT>(width);
	viewport.Height = static_cast<FLOAT>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	m_d3dContext->RSSetViewports(1, &viewport);
	return true;
}

void DXApp::MainLoop()
{
	MSG msg = {};
	m_timer.Reset();
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			m_timer.Tick();	
			if (m_isPaused == false) {
				CalculateFrameStats();
				Render();
			}
			else {
				Sleep(100); // Sleep to avoid busy waiting when paused
			}
		}
	}
}

void DXApp::Render()
{
	float clearColor[4] = { 0.1f, 0.1f, 0.3f, 1.0f };

	m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
	m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), nullptr);
	m_rootNode->Update(DirectX::XMMatrixIdentity());
	m_rootNode->Draw(m_d3dContext.Get());
	UpdateCamera();
	m_swapChain->Present(1, 0);
}

LRESULT CALLBACK DXApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ACTIVATE:
	{
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			if (g_app)
			{
				g_app->m_isPaused = true;
				g_app->m_timer.Stop();
			}
		}
		else
		{
			if (g_app)
			{
				g_app->m_isPaused = false;
				g_app->m_timer.Start();
			}
		}
		return 0;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		return 0;
	}
	case WM_SIZE:
	{

		UINT newWidth = LOWORD(lParam);
		UINT newHeight = HIWORD(lParam);
		g_app->m_width = newWidth;
		g_app->m_height = newHeight;
		if (g_app && g_app->m_swapChain)
		{
			// Release old render target view
			g_app->m_renderTargetView.Reset();

			// Resize swap chain buffers
			HRESULT hr = g_app->m_swapChain->ResizeBuffers(
				1, newWidth, newHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

			if (SUCCEEDED(hr))
			{
				// Get new back buffer and create new render target view
				Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
				hr = g_app->m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
				if (SUCCEEDED(hr))
				{
					g_app->m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, &g_app->m_renderTargetView);
					g_app->m_d3dContext->OMSetRenderTargets(1, g_app->m_renderTargetView.GetAddressOf(), nullptr);

					// Update viewport
					D3D11_VIEWPORT vp = {};
					vp.Width = static_cast<FLOAT>(newWidth);
					vp.Height = static_cast<FLOAT>(newHeight);
					vp.MinDepth = 0.0f;
					vp.MaxDepth = 1.0f;
					vp.TopLeftX = 0.0f;
					vp.TopLeftY = 0.0f;
					g_app->m_d3dContext->RSSetViewports(1, &vp);
				}
			}
		}
		return 0;
	}
	case WM_LBUTTONDOWN:
		SetCapture(hWnd);
		g_app->m_lastMouseX = GET_X_LPARAM(lParam);
		g_app->m_lastMouseY = GET_Y_LPARAM(lParam);
		break;

	case WM_MOUSEMOVE:
		if (wParam & MK_LBUTTON)
		{
			int dx = GET_X_LPARAM(lParam) - g_app->m_lastMouseX;
			int dy = GET_Y_LPARAM(lParam) - g_app->m_lastMouseY;

			g_app->m_lastMouseX = GET_X_LPARAM(lParam);
			g_app->m_lastMouseY = GET_Y_LPARAM(lParam);

			float sensitivity = 0.01f;
			g_app->m_Yaw += dx * sensitivity;
			g_app->m_Pitch += dy * sensitivity;

			// Clamp pitch to avoid flipping
			const float limit = DirectX::XM_PIDIV2 - 0.01f;
			// use c++20 for std::clamp
			g_app->m_Pitch = std::clamp(g_app->m_Pitch, -limit, limit);
		}
		break;

	case WM_LBUTTONUP:
		ReleaseCapture();
		break;
	case WM_MOUSEWHEEL:
	{
		// Handle mouse wheel for zooming
		int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		float zoomFactor = 1.0f + (delta / 1200.0f); // Adjust sensitivity as needed
		g_app->m_Radius *= zoomFactor; // Adjust camera radius based on zoom factor
		return 0;
	}
	case WM_GESTURE:
	{
		return g_app->DecodeGesture(hWnd, message, wParam, lParam);
	}

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

LRESULT DXApp::DecodeGesture(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	// Create a structure to populate and retrieve the extra message info.
	GESTUREINFO gi;

	ZeroMemory(&gi, sizeof(GESTUREINFO));

	gi.cbSize = sizeof(GESTUREINFO);

	BOOL bResult = GetGestureInfo((HGESTUREINFO)lParam, &gi);
	BOOL bHandled = FALSE;

	if (bResult) {
		// now interpret the gesture
		switch (gi.dwID) {
		case GID_ZOOM: {
			static double lastArgument = 0.0;
			OutputDebugString(L"Zoom gesture detected.\n");
			double argument = static_cast<double>(gi.ullArguments);
			if (gi.dwFlags & GF_BEGIN) {
				lastArgument = argument;
			}
			else {
				// Calculate zoom factor based on the change in argument
				double zoomFactor = argument / lastArgument;
				g_app->m_Radius /= static_cast<float>(zoomFactor); // Zoom in/out
				lastArgument = argument;
			}
			bHandled = TRUE;
			break;
		}
		case GID_PAN: {
			static POINT lastPan = {};
			if (gi.dwFlags & GF_BEGIN) {
				lastPan.x = gi.ptsLocation.x;
				lastPan.y = gi.ptsLocation.y;
				OutputDebugString(L"Pan gesture detected.\n");
			}
			else {
				int dx = gi.ptsLocation.x - lastPan.x;
				int dy = gi.ptsLocation.y - lastPan.y;
				float sensitivity = 0.01f;
				g_app->m_Yaw += dx * sensitivity;
				g_app->m_Pitch += dy * sensitivity;
				// Clamp pitch to avoid flipping
				const float limit = DirectX::XM_PIDIV2 - 0.01f;
				g_app->m_Pitch = std::clamp(g_app->m_Pitch, -limit, limit);
				lastPan.x = gi.ptsLocation.x;
				lastPan.y = gi.ptsLocation.y;
			}
			bHandled = TRUE;
			break;
		}
		case GID_ROTATE: {
			// Code for rotation goes here
			bHandled = TRUE;
			break;
		}
		case GID_TWOFINGERTAP: {
			// Code for two-finger tap goes here
			bHandled = TRUE;
			break;
		}
		case GID_PRESSANDTAP: {
			// Code for roll over goes here
			bHandled = TRUE;
			break;
		}
		default: {
			// A gesture was not recognized
			break;
		}
		}
	}
	else {
		DWORD dwErr = GetLastError();
		if (dwErr > 0) {
			//MessageBoxW(hWnd, L"Error!", L"Could not retrieve a GESTUREINFO structure.", MB_OK);
		}
	}
	if (bHandled) {
		return 0;
	}
	else {
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

// This function is used to return an index given an ID
int GetContactIndex(int dwID) {
	for (int i = 0; i < MAXPOINTS; i++) {
		if (idLookup[i] == dwID) {
			return i;
		}
	}

	for (int i = 0; i < MAXPOINTS; i++) {
		if (idLookup[i] == -1) {
			idLookup[i] = dwID;
			return i;
		}
	}
	// Out of contacts
	return -1;
}

// Mark the specified index as initialized for new use
BOOL RemoveContactIndex(int index) {
	if (index >= 0 && index < MAXPOINTS) {
		idLookup[index] = -1;
		return true;
	}

	return false;
}

void DXApp::CalculateFrameStats()
{
	static int frameCount = 0;
	static float timeElapsed = 0.0f;
	frameCount++;
	timeElapsed += m_timer.DeltaTime();
	if (timeElapsed >= 1.0f)
	{
		std::wstringstream ss;
		ss << L"FPS: " << frameCount << L" | Frame Time: " << (1000.0f / frameCount) << L" ms";
		SetWindowText(m_hWnd, ss.str().c_str());
		frameCount = 0;
		timeElapsed = 0.0f;
	}
}
/*
void DXApp::LoadTexture(const wchar_t* filename)
{
	HRESULT hr = DirectX::CreateWICTextureFromFile(
			m_d3dDevice.Get(), filename, nullptr,
		m_texture.ReleaseAndGetAddressOf());
	if (FAILED(hr))
		{
		std::wstring errorMsg = L"Failed to load texture: ";
		errorMsg += filename;
		throw std::runtime_error(std::string(errorMsg.begin(), errorMsg.end()));
	}
	m_d3dContext->PSSetShaderResources(0, 1, m_texture.GetAddressOf());
	m_d3dContext->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
}

void DXApp::BuildShaders()
{
	HRESULT hr;

	// Compile vertex shader
	hr = D3DCompileFromFile(L"shader.hlsl", nullptr, nullptr, "VSMain", "vs_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS, 0, &m_vsBlob, &m_errorBlob);
	if (FAILED(hr))
	{
		if (m_errorBlob)
		{
			OutputDebugStringA((char*)m_errorBlob->GetBufferPointer());
		}
		throw std::runtime_error("Vertex shader compilation failed.");
	}
	hr = m_d3dDevice->CreateVertexShader(m_vsBlob->GetBufferPointer(), m_vsBlob->GetBufferSize(), nullptr, &m_vertexShader);
	if (FAILED(hr)) throw std::runtime_error("Vertex shader creation failed.");

	// Compile pixel shader
	hr = D3DCompileFromFile(L"shader.hlsl", nullptr, nullptr, "PSMain", "ps_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS, 0, &m_fsBlob, &m_errorBlob);
	if (FAILED(hr))
	{
		if (m_errorBlob)
		{
			OutputDebugStringA((char*)m_errorBlob->GetBufferPointer());
		}
		throw std::runtime_error("Pixel shader compilation failed.");
	}
	hr = m_d3dDevice->CreatePixelShader(m_fsBlob->GetBufferPointer(), m_fsBlob->GetBufferSize(), nullptr, &m_pixelShader);
	if (FAILED(hr)) throw std::runtime_error("Pixel shader creation failed.");
}


void DXApp::BuildVertexLayout()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,   0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,      0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	HRESULT hr = m_d3dDevice->CreateInputLayout(
		layout, 3,
		m_vsBlob->GetBufferPointer(),
		m_vsBlob->GetBufferSize(),
		&m_inputLayout);
	if (FAILED(hr)) throw std::runtime_error("Input layout creation failed.");
}

void DXApp::BuildGeometryBuffers()
{
	Vertex vertices[] =
	{
		{ DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT4(0, 0, 1, 1), DirectX::XMFLOAT2(0,1)},
		{ DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f), DirectX::XMFLOAT4(0, 1, 0, 1), DirectX::XMFLOAT2(1,1)},
		{ DirectX::XMFLOAT3(0.0f,  0.5f, 0.0f), DirectX::XMFLOAT4(1, 0, 0, 1), DirectX::XMFLOAT2(0.5,0)},
	};

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(vertices);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices;

	HRESULT hr = m_d3dDevice->CreateBuffer(&bd, &initData, &m_vertexBuffer);
	if (FAILED(hr)) throw std::runtime_error("Vertex buffer creation failed.");
}
*/
void DXApp::BuildConstantBuffers()
{
	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.ByteWidth = sizeof(MatrixBufferType);
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	HRESULT hr = m_d3dDevice->CreateBuffer(&cbDesc, nullptr, &m_constantBuffer);
	if (FAILED(hr)) throw std::runtime_error("Constant buffer creation failed.");

	// Set the constant buffer to the vertex shader
	m_World = DirectX::XMMatrixIdentity();
	m_View = DirectX::XMMatrixLookAtLH(
		DirectX::XMVectorSet(0.0f, 0.0f, -5.0f, 1.0f),
		DirectX::XMVectorZero(),
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	m_Proj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(45.0f),
		static_cast<float>(m_width) / static_cast<float>(m_height), 0.1f, 100.0f);

	MatrixBufferType matrixData{
		DirectX::XMMatrixTranspose(m_World),
		DirectX::XMMatrixTranspose(m_View),
		DirectX::XMMatrixTranspose(m_Proj),
	};

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	m_d3dContext->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &matrixData, sizeof(MatrixBufferType));
	m_d3dContext->Unmap(m_constantBuffer.Get(), 0);
	m_d3dContext->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
}


void DXApp::BuildCamera()
{
	DirectX::XMFLOAT3 cameraTarget{ 0.0f, 0.0f, 0.0f };
	m_Yaw = 0.0f;
	m_Pitch = 0.0f;
	m_Radius = 5.0f;
	DirectX::XMVECTOR target = DirectX::XMLoadFloat3(&cameraTarget);
	float x = m_Radius * cosf(m_Pitch) * sinf(m_Yaw);
	float y = m_Radius * sinf(m_Pitch);
	float z = m_Radius * cosf(m_Pitch) * cosf(m_Yaw);
	DirectX::XMVECTOR position = DirectX::XMVectorSet(x + cameraTarget.x, y + cameraTarget.y, z + cameraTarget.z, 1.0f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_View = DirectX::XMMatrixLookAtLH(position, target, up);

	m_World = DirectX::XMMatrixIdentity();
	m_Proj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(45.0f),
		static_cast<float>(m_width) / static_cast<float>(m_height), 0.1f, 100.0f);

	DirectX::XMMATRIX wvp = m_World * m_View * m_Proj;
	wvp = XMMatrixTranspose(wvp); // Transpose for HLSL if using row-major

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	m_d3dContext->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &wvp, sizeof(MatrixBufferType));
	m_d3dContext->Unmap(m_constantBuffer.Get(), 0);
	m_d3dContext->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
}

void DXApp::UpdateCamera()
{
	MatrixBufferType matrixData;
	matrixData.World = DirectX::XMMatrixTranspose(m_World);
	DirectX::XMFLOAT3 cameraTarget{ 0.0f, 0.0f, 0.0f };
	DirectX::XMVECTOR target = DirectX::XMLoadFloat3(&cameraTarget);
	float x = m_Radius * cosf(m_Pitch) * sinf(m_Yaw);
	float y = m_Radius * sinf(m_Pitch);
	float z = m_Radius * cosf(m_Pitch) * cosf(m_Yaw);
	DirectX::XMVECTOR position = DirectX::XMVectorSet(x + cameraTarget.x, y + cameraTarget.y, z + cameraTarget.z, 1.0f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_View = DirectX::XMMatrixLookAtLH(position, target, up);
	matrixData.View = DirectX::XMMatrixTranspose(m_View);
	matrixData.Projection = DirectX::XMMatrixTranspose(m_Proj);
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	m_d3dContext->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &matrixData, sizeof(MatrixBufferType));
	m_d3dContext->Unmap(m_constantBuffer.Get(), 0);
	m_d3dContext->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

}

