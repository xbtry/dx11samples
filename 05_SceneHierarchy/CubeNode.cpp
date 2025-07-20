#include "CubeNode.h"
#include <stdexcept>

CubeNode::CubeNode(ID3D11Device* device)
{
	m_vertices = new Vertex[8]
	{
		{ {-0.5f, -0.5f, -0.5f}, {1,0,0,1}, {0.0f, 1.0f} },  //0
		{ {-0.5f, +0.5f, -0.5f}, {0,1,0,1}, {0.0f, 0.0f} },  //1
		{ {+0.5f, +0.5f, -0.5f}, {0,0,1,1}, {1.0f, 0.0f} },  //2
		{ {+0.5f, -0.5f, -0.5f}, {1,1,0,1}, {1.0f, 1.0f} },  //3
		{ {-0.5f, -0.5f, +0.5f}, {1,0,1,1}, {0.0f, 1.0f} },  //4
		{ {-0.5f, +0.5f, +0.5f}, {0,1,1,1}, {0.0f, 0.0f} },  //5
		{ {+0.5f, +0.5f, +0.5f}, {1,1,1,1}, {1.0f, 0.0f} },  //6
		{ {+0.5f, -0.5f, +0.5f}, {1,0,0,1}, {1.0f, 1.0f} }   //7
	};

	// Indices for 12 triangles (2 per face)
	m_indices = {
		0,1,2, 0,2,3,    // back face
		4,6,5, 4,7,6,    // front face
		4,5,1, 4,1,0,    // left face
		3,2,6, 3,6,7,    // right face
		1,5,6, 1,6,2,    // top face
		4,0,3, 4,3,7     // bottom face
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
		throw std::runtime_error("Failed to load shaders for CubeNode.");
	}
	m_shader->BuildInputLayout(device, layout, ARRAYSIZE(layout));
	BuildGeometryBuffers(device);
}

CubeNode::~CubeNode() {
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

void CubeNode::Update(const DirectX::XMMATRIX& parentTransform)
{
	// Update the world transform based on parent transform
	m_worldTransform = parentTransform * DirectX::XMMatrixIdentity();
	// Update children if any (not applicable here, but can be overridden in derived classes)
	SceneNode::Update(m_worldTransform);
}

void CubeNode::BuildGeometryBuffers(ID3D11Device* device)
{
	// Vertex Buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex) * 8;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = m_vertices;
	HRESULT hr = device->CreateBuffer(&bd, &initData, m_vertexBuffer.GetAddressOf());
	if (FAILED(hr))
		throw std::runtime_error("Failed to create vertex buffer for CubeNode.");

	// Index Buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(UINT) * static_cast<UINT>(m_indices.size());
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	initData.pSysMem = m_indices.data();
	hr = device->CreateBuffer(&bd, &initData, m_indexBuffer.GetAddressOf());
	if (FAILED(hr))
		throw std::runtime_error("Failed to create index buffer for CubeNode.");
}


void CubeNode::Draw(ID3D11DeviceContext* context)
{
	if (!m_shader || !m_vertexBuffer || !m_indexBuffer) {
		return;
	}
	// Bind the shader
	m_shader->Bind(context);
	// Set the vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->DrawIndexed(static_cast<UINT>(m_indices.size()), 0, 0);
}