#pragma once

#include "SceneNode.h"
#include "Shaders.h"

struct Vertex {
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT2 uv;
};

class TriangleNode : public SceneNode
{
public:
	TriangleNode(ID3D11Device* device);
	~TriangleNode();
	void Draw(ID3D11DeviceContext* context) override;
	void Update(const DirectX::XMMATRIX& parentTransform) override;
private:
	void BuildGeometryBuffers(ID3D11Device* device);
	Shaders* m_shader;
	Vertex* m_vertices;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
};