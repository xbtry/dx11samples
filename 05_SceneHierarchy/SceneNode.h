#pragma once
#include "pch.h"

class SceneNode
{
public:
	enum class NodeType
	{
		Triangle,
		Cube,
		Sphere,
		// Add more types as needed
	};
	SceneNode();
	virtual ~SceneNode();
	void SetPosition(float x, float y, float z);
	void SetRotation(float pitch, float yaw, float roll);
	void SetScale(float scaleX, float scaleY, float scaleZ);
	void AddChild(std::shared_ptr<SceneNode> child);
	virtual void Update(const DirectX::XMMATRIX& parentTransform);
	virtual void Draw(ID3D11DeviceContext* context);
protected:
	DirectX::XMFLOAT3 m_position, m_rotation, m_scale;
	DirectX::XMMATRIX m_worldTransform;

	std::vector<std::shared_ptr<SceneNode>> m_children;
};