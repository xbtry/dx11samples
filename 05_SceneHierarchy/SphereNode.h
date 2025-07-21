#pragma once

#include "SceneNode.h"
#include "Shaders.h"
class SphereNode : public SceneNode
{
public:
	SphereNode(ID3D11Device* device);
	~SphereNode();
	void Draw(ID3D11DeviceContext* context) override;
	void Update(const DirectX::XMMATRIX& parentTransform) override;
private:
	struct Vertex {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 uv;
	};
	int m_indexCount;
	void BuildGeometryBuffers(ID3D11Device* device);
	Shaders* m_shader = nullptr;
	Vertex* m_vertices = nullptr;
	std::vector<UINT> m_indices;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
};