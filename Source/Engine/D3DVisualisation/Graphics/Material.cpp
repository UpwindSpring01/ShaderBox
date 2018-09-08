#include "stdafx.h"

#include "Material.h"

SpringWindEngine::Material::Material(Effect* pEffect)
	: m_pEffect(pEffect)
{
	++m_pEffect->Counter;
}

SpringWindEngine::Material::~Material()
{
	--m_pEffect->Counter;
}

void SpringWindEngine::Material::SetShadersAndBuffers(const EngineContext& engineContext, const TransformComponent* /*pTransComp*/)
{
	ID3D11DeviceContext* pDeviceContext = engineContext.pDeviceContext;
	pDeviceContext->IASetInputLayout(m_pEffect->pInputLayoutDesc->pInputLayout);
	pDeviceContext->VSSetShader(m_pEffect->pVertexShader, 0, 0);
	pDeviceContext->PSSetShader(m_pEffect->pPixelShader, 0, 0);
	pDeviceContext->GSSetShader(m_pEffect->pGeometryShader, 0, 0);
	pDeviceContext->HSSetShader(m_pEffect->pHullShader, 0, 0);
	pDeviceContext->DSSetShader(m_pEffect->pDomainShader, 0, 0);
}