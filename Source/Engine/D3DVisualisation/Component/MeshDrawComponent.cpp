#include "stdafx.h"

#include "MeshDrawComponent.h"

void MeshDrawComponent::Draw(const GameContext& gameContext)
{
	BaseConstantBufferObject* buffer = GetGameObject()->GetScene()->GetObjectDataCBuffer();
	XMMATRIX world = XMMatrixIdentity();
	XMMATRIX wvp = world * gameContext.pCameraData->m_View *  gameContext.pCameraData->m_Projection;

	XMStoreFloat4x4(&buffer->world, XMMatrixTranspose(world));
	XMStoreFloat4x4(&buffer->worldInverse, XMMatrixTranspose(XMMatrixInverse(nullptr, world)));
	XMStoreFloat4x4(&buffer->worldViewProjection, XMMatrixTranspose(wvp));

	GetGameObject()->GetScene()->UpdateObjectBuffer(gameContext);

	m_pMaterial->SetShadersAndBuffers(gameContext, GetGameObject()->GetTransform());
	m_pMaterial->BeginDraw();
	m_pMesh->Draw();
	m_pMaterial->EndDraw();
}
