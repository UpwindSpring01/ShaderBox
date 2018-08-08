#include "stdafx.h"

#include "TransformComponent.h"
#include "../SceneGraph/GameObject.h"

void TransformComponent::Rotate(float x, float y, float z)
{
	XMStoreFloat4(&m_Rotation, XMQuaternionRotationRollPitchYaw(x, y, z));
	SetDirty(SW_DIRTY_ROTATION);
}

void TransformComponent::Scale(float x, float y, float z)
{
	m_Scale.x = x;
	m_Scale.y = y;
	m_Scale.z = z;
	SetDirty(SW_DIRTY_SCALE);
}

void TransformComponent::Translate(float x, float y, float z)
{
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;
	SetDirty(SW_DIRTY_TRANSLATE);
}

void TransformComponent::SetDirty(byte dirtyFlags)
{
	m_DirtyFlags |= dirtyFlags;
}

XMFLOAT4X4 TransformComponent::GetWorld()
{
	if(m_DirtyFlags != 0)
	{
		UpdateTransforms();
	}
	return m_World;
}

XMFLOAT3 TransformComponent::GetWorldPosition()
{
	if((m_DirtyFlags & SW_DIRTY_TRANSLATE) != 0)
	{
		UpdateTransforms();
	}
	return m_WorldPosition;
}

XMFLOAT3 TransformComponent::GetWordScale()
{
	if((m_DirtyFlags & SW_DIRTY_SCALE) != 0)
	{
		UpdateTransforms();
	}
	return m_WorldScale;
}

XMFLOAT4 TransformComponent::GetWorldRotation()
{
	if((m_DirtyFlags & SW_DIRTY_ROTATION) != 0)
	{
		UpdateTransforms();
	}
	return m_WorldRotation;
}

void TransformComponent::UpdateTransforms()
{
	XMVECTOR rot = XMLoadFloat4(&m_Rotation);
	XMMATRIX world = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z) *
		XMMatrixRotationQuaternion(rot) * XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	
	XMStoreFloat4x4(&m_World, world);

	XMVECTOR pos, scale;
	XMMatrixDecompose(&scale, &rot, &pos, world);
	XMStoreFloat3(&m_WorldPosition, pos);
	XMStoreFloat3(&m_WorldScale, scale);
	XMStoreFloat4(&m_WorldRotation, rot);

	m_DirtyFlags = SW_DIRTY_NONE;
}