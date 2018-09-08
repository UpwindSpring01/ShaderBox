#include "stdafx.h"

#include "MeshDrawComponent.h"

void SpringWindEngine::MeshDrawComponent::SetMesh(Unsafe_Shared_Ptr<Mesh> pMesh)
{
	m_pMesh = pMesh;
	RefreshBuffers();
}

void SpringWindEngine::MeshDrawComponent::SetMaterial(Material* pMaterial)
{
	m_pMaterial = pMaterial;
	RefreshBuffers();
}

void SpringWindEngine::MeshDrawComponent::SetMeshAndMaterial(Unsafe_Shared_Ptr<Mesh> pMesh, Material* pMaterial)
{
	m_pMesh = pMesh;
	m_pMaterial = pMaterial;
	RefreshBuffers();
}

void SpringWindEngine::MeshDrawComponent::RefreshBuffers()
{
	if (m_pMesh && m_pMaterial != nullptr)
	{
		m_pVertexBuffer = m_pMesh->GetVertexBuffer(m_pMaterial->GetEffect()->pInputLayoutDesc);
		m_pIndexBuffer = m_pMesh->GetIndexBuffer(m_pMaterial->GetEffect()->Topology);
	}
}

void SpringWindEngine::MeshDrawComponent::Draw(const EngineContext& engineContext)
{
	BaseConstantBufferObject* buffer = engineContext.pBaseCBufferObject;
	
	GameObject* pObject = GetGameObject();
	CameraComponent* pCamera = pObject->GetScene()->GetActiveCamera();

	XMMATRIX world = pObject->GetTransform()->GetWorld();
	XMMATRIX wvp = world * pCamera->GetViewProjection();

	XMMATRIX worldTransposed = XMMatrixTranspose(world);
	XMStoreFloat4x4(&buffer->world, worldTransposed);
	XMStoreFloat4x4(&buffer->worldInverse, XMMatrixInverse(nullptr, worldTransposed));
	XMStoreFloat4x4(&buffer->worldViewProjection, XMMatrixTranspose(wvp));

	SpringWind::GetInstance()->UpdateObjectBuffer();

	m_pMaterial->SetShadersAndBuffers(engineContext, GetGameObject()->GetTransform());
	m_pMaterial->BeginDraw();

	ID3D11DeviceContext* pDeviceContext = engineContext.pDeviceContext;
	const Effect* pEffect = m_pMaterial->GetEffect();
	
	UINT offset = 0;
	ID3D11Buffer* pBuffer = m_pVertexBuffer.Get();
	pDeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &pEffect->pInputLayoutDesc->InputLayoutSize, &offset);
	pDeviceContext->IASetPrimitiveTopology(pEffect->Topology);

	pDeviceContext->IASetInputLayout(pEffect->pInputLayoutDesc->pInputLayout);
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer->first, DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->DrawIndexed(m_pIndexBuffer->second, 0, 0);

	m_pMaterial->EndDraw();
}
