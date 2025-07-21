#pragma once

#include "SceneNode.h"
#include "Shaders.h"
class TriangleNode : public SceneNode
{
public:
	TriangleNode(ID3D11Device* device);
	~TriangleNode();
	void Draw(ID3D11DeviceContext* context) override;
	void Update(const DirectX::XMMATRIX& parentTransform) override;
private:
	struct Vertex {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 uv;
	};
	void BuildGeometryBuffers(ID3D11Device* device);
	Shaders* m_shader = nullptr;
	Vertex* m_vertices = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
};