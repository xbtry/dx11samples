#pragma once

#include "SceneNode.h"
#include "Shaders.h"
class CubeNode : public SceneNode
{
public:
	CubeNode(ID3D11Device* device);
	~CubeNode();
	void Draw(ID3D11DeviceContext* context) override;
	void Update(const DirectX::XMMATRIX& parentTransform) override;
private:
	struct Vertex {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 uv;
	};
	void BuildGeometryBuffers(ID3D11Device* device);
	Shaders* m_shader;
	Vertex* m_vertices;
	std::vector<UINT> m_indices;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
};