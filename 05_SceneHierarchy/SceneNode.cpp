#include "SceneNode.h"

SceneNode::SceneNode()
	: m_position(0.0f, 0.0f, 0.0f), m_rotation(0.0f, 0.0f, 0.0f), m_scale(1.0f, 1.0f, 1.0f)
{
	m_worldTransform = DirectX::XMMatrixIdentity();
}

SceneNode::~SceneNode()
{
	// Cleanup children
	m_children.clear();
}

void SceneNode::SetPosition(float x, float y, float z)
{
	m_position = DirectX::XMFLOAT3(x, y, z);
}

void SceneNode::SetRotation(float pitch, float yaw, float roll)
{
	m_rotation = DirectX::XMFLOAT3(pitch, yaw, roll);
}

void SceneNode::SetScale(float scaleX, float scaleY, float scaleZ)
{
	m_scale = DirectX::XMFLOAT3(scaleX, scaleY, scaleZ);
}

void SceneNode::AddChild(std::shared_ptr<SceneNode> child)
{
	m_children.push_back(child);
}


void SceneNode::Update(const DirectX::XMMATRIX& parentTransform)
{
	// Calculate the world transform for this node
	DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);
	DirectX::XMMATRIX scaling = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	m_worldTransform = parentTransform * scaling * rotation * translation;
	// Update children
	for (const auto& child : m_children)
	{
		child->Update(m_worldTransform);
	}
}

void SceneNode::Draw(ID3D11DeviceContext* context)
{
	// This method should be overridden in derived classes to perform actual drawing
	// For now, we just update the world transform for children
	for (const auto& child : m_children)
	{
		child->Draw(context);
	}
}