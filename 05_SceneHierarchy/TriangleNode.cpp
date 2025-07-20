#include "TriangleNode.h"
#include <stdexcept>

TriangleNode::TriangleNode(ID3D11Device* device)
{
	m_vertices = new Vertex[3]{
		{ { -0.5f, -0.5f, 0.0f }, { 0, 0, 1, 1 }, { 0, 1 } },
		{ { 0.5f, -0.5f, 0.0f }, { 0, 1, 0, 1 }, { 1, 1 } },
		{ { 0.0f, 0.5f, 0.0f }, { 1, 0, 0, 1 }, { 0.5f, 0 } }
	};
	m_shader = new Shaders();
	m_shader->CompileShaders(device, L"TriangleVertexShader.hlsl", L"TrianglePixelShader.hlsl");
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	if (!m_shader->Load(device, L"TriangleVertexShader.hlsl", L"TrianglePixelShader.hlsl")) {
		throw std::runtime_error("Failed to load shaders for TriangleNode.");
	}
	m_shader->BuildInputLayout(device, layout, ARRAYSIZE(layout));
	BuildGeometryBuffers(device);
}

TriangleNode::~TriangleNode(){
	if (m_vertexBuffer) {
		m_vertexBuffer.Reset();
	}
	if (m_shader) {
		delete m_shader;
		m_shader = nullptr;
	}
	if (m_vertices) {
		delete[] m_vertices;
		m_vertices = nullptr;
	}
}

void TriangleNode::Update(const DirectX::XMMATRIX& parentTransform)
{
	// Update the world transform based on parent transform
	m_worldTransform = parentTransform * DirectX::XMMatrixIdentity();
	// Update children if any (not applicable here, but can be overridden in derived classes)
	SceneNode::Update(m_worldTransform);
}

void TriangleNode::BuildGeometryBuffers(ID3D11Device* device)
{
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex) * 3; // 3 vertices for a triangle
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = m_vertices;
	HRESULT hr = device->CreateBuffer(&bd, &initData, &m_vertexBuffer);
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to create vertex buffer for TriangleNode.");
	}
}


void TriangleNode::Draw(ID3D11DeviceContext* context)
{
	if (!m_shader || !m_vertexBuffer) {
		return; // Ensure shader and vertex buffer are valid
	}
	// Bind the shader
	m_shader->Bind(context);
	// Set the vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// Draw the triangle
	context->Draw(3, 0); // 3 vertices for a triangle
}