#include "SphereNode.h"

#include <stdexcept>
#include <random>

SphereNode::SphereNode(ID3D11Device* device)
{
	m_shader = new Shaders();
	m_shader->CompileShaders(device, L"TriangleVertexShader.hlsl", L"TrianglePixelShader.hlsl");
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	if (!m_shader->Load(device, L"TriangleVertexShader.hlsl", L"TrianglePixelShader.hlsl")) {
		throw std::runtime_error("Failed to load shaders for SphereNode.");
	}
	m_shader->BuildInputLayout(device, layout, ARRAYSIZE(layout));
	BuildGeometryBuffers(device);
}

SphereNode::~SphereNode() {
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

void SphereNode::Update(const DirectX::XMMATRIX& parentTransform)
{
	// Update the world transform based on parent transform
	m_worldTransform = parentTransform * DirectX::XMMatrixIdentity();
	// Update children if any (not applicable here, but can be overridden in derived classes)
	SceneNode::Update(m_worldTransform);
}

void SphereNode::BuildGeometryBuffers(ID3D11Device* device)
{
	float radius = 1.0;
	unsigned int sliceCount = 20;
	unsigned int stackCount = 20;
	std::vector<Vertex> vertices;
	std::vector<UINT> indices;
	
	for (UINT i = 0; i <= stackCount; ++i)
	{
		float phi = DirectX::XM_PI * i / stackCount;
		
		for (UINT j = 0; j <= sliceCount; ++j)
		{
			
			float theta = DirectX::XM_2PI * j / sliceCount;
			std::random_device rd;  // Seed
			std::mt19937 gen(rd()); // Mersenne Twister generator
			std::uniform_real_distribution<float> dist(0.0f, 1.0f);

			// Use dist(gen) to get random float between 0.0 and 1.0
			float red = dist(gen);
			Vertex v;
			v.position = {
				radius * sinf(phi) * cosf(theta),
				radius * cosf(phi),
				radius * sinf(phi) * sinf(theta)
			};

			v.uv = { (float)j / sliceCount, (float)i / stackCount };
			v.color = DirectX::XMFLOAT4(red, 0.0, 0.5, 1.0);

			vertices.push_back(v);
		}
	}

	for (UINT i = 0; i < stackCount; ++i)
	{
		for (UINT j = 0; j < sliceCount; ++j)
		{
			UINT a = i * (sliceCount + 1) + j;
			UINT b = (i + 1) * (sliceCount + 1) + j;

			indices.push_back(a);
			indices.push_back(b);
			indices.push_back(a + 1);

			indices.push_back(a + 1);
			indices.push_back(b);
			indices.push_back(b + 1);
		}
	}

	m_indexCount = static_cast<UINT>(indices.size());

	HRESULT hr;

	D3D11_BUFFER_DESC vbd = {};
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = UINT(sizeof(Vertex) * vertices.size());
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vinit = {};
	vinit.pSysMem = vertices.data();
	hr = device->CreateBuffer(&vbd, &vinit, m_vertexBuffer.GetAddressOf());
	if (FAILED(hr))
		throw std::runtime_error("Failed to create vertex buffer for SphereNode.");
	D3D11_BUFFER_DESC ibd = {};
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = UINT(sizeof(UINT) * indices.size());
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA iinit = {};
	iinit.pSysMem = indices.data();
	hr = device->CreateBuffer(&ibd, &iinit, m_indexBuffer.GetAddressOf());

	if (FAILED(hr))
		throw std::runtime_error("Failed to create index buffer for SphereNode.");
}


void SphereNode::Draw(ID3D11DeviceContext* context)
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
	context->DrawIndexed(static_cast<UINT>(m_indexCount), 0, 0);
}