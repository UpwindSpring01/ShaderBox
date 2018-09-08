#include "stdafx.h"

#include "TransformComponent.h"
#include "SceneGraph/GameObject.h"

void SpringWindEngine::TransformComponent::AddChild(TransformComponent* pChild)
{
	assert(pChild->m_pParent == nullptr);

	m_ChildrenPtrArr.push_back(pChild);
	pChild->m_pParent = this;
}

void SpringWindEngine::TransformComponent::MoveChild(TransformComponent* pChild)
{
	assert(pChild->m_pParent != nullptr);
	assert(pChild->m_pParent != this);

	pChild->m_pParent->m_ChildrenPtrArr.erase(std::remove(m_ChildrenPtrArr.begin(), m_ChildrenPtrArr.end(), pChild));

	m_ChildrenPtrArr.push_back(pChild);
	pChild->m_pParent = this;
}

void SpringWindEngine::TransformComponent::Rotation(const float x, const float y, const float z)
{
	XMStoreFloat4(&m_Rotation, XMQuaternionRotationRollPitchYaw(x, y, z));
	SetDirty(SW_DIRTY_ROTATION | SW_DIRTY_WORLD);
}

void SpringWindEngine::TransformComponent::Rotation(const XMFLOAT4& rotation)
{
	m_Rotation = rotation;
	SetDirty(SW_DIRTY_ROTATION | SW_DIRTY_WORLD);
}

void SpringWindEngine::TransformComponent::Scale(const float x, const float y, const float z)
{
	m_Scale.x = x;
	m_Scale.y = y;
	m_Scale.z = z;
	SetDirty(SW_DIRTY_SCALE | SW_DIRTY_WORLD);
}

void SpringWindEngine::TransformComponent::Translation(const float x, const float y, const float z)
{
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;
	SetDirty(SW_DIRTY_TRANSLATE | SW_DIRTY_WORLD);
}

void SpringWindEngine::TransformComponent::Translation(const XMFLOAT3& translation)
{
	m_Position = translation;
	SetDirty(SW_DIRTY_TRANSLATE | SW_DIRTY_WORLD);
}

void SpringWindEngine::TransformComponent::SetDirty(byte dirtyFlags)
{
	if ((dirtyFlags & SW_DIRTY_WORLD) != 0)
	{
		GetGameObject()->SetTransformDirty();
	}
	m_DirtyFlags |= dirtyFlags;

	for (TransformComponent* pComp : m_ChildrenPtrArr)
	{
		pComp->SetDirty(dirtyFlags);
	}
}

const XMFLOAT4X4& SpringWindEngine::TransformComponent::GetWorldMatrix()
{
	if (m_DirtyFlags != 0)
	{
		UpdateTransforms(false);
	}
	return m_World;
}

const XMFLOAT3& SpringWindEngine::TransformComponent::GetWorldPosition()
{
	if ((m_DirtyFlags & SW_DIRTY_TRANSLATE) != 0)
	{
		UpdateTransforms(true);
	}
	return m_WorldPosition;
}

const XMFLOAT3& SpringWindEngine::TransformComponent::GetWordScale()
{
	if ((m_DirtyFlags & SW_DIRTY_SCALE) != 0)
	{
		UpdateTransforms(true);
	}
	return m_WorldScale;
}

const XMFLOAT4& SpringWindEngine::TransformComponent::GetWorldRotation()
{
	if ((m_DirtyFlags & SW_DIRTY_ROTATION) != 0)
	{
		UpdateTransforms(true);
	}
	return m_WorldRotation;
}

void SpringWindEngine::TransformComponent::UpdateTransforms(bool decompose)
{
	XMMATRIX world = GetWorld();
	if (!decompose)
	{
		return;
	}
	XMVECTOR scale, rot, pos;
	XMMatrixDecompose(&scale, &rot, &pos, world);
	XMStoreFloat3(&m_WorldPosition, pos);
	XMStoreFloat3(&m_WorldScale, scale);
	XMStoreFloat4(&m_WorldRotation, rot);
	m_DirtyFlags = SW_DIRTY_NONE;
}

const XMMATRIX SpringWindEngine::TransformComponent::GetWorld()
{
	//TODO Apply parent transform
	if ((m_DirtyFlags & SW_DIRTY_WORLD) != 0)
	{
		XMVECTOR rot = XMLoadFloat4(&m_Rotation);
		XMMATRIX world = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z) * XMMatrixRotationQuaternion(rot) * XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
		XMStoreFloat4x4(&m_World, world);
		m_DirtyFlags &= ~SW_DIRTY_WORLD;
		return world;
	}
	else
	{
		return XMLoadFloat4x4(&m_World);
	}
}