#include "stdafx.h"
#include "GameScene.h"

#include "GameObject.h"
#include "Graphics/SpriteRenderer.h"
#include "Graphics/PPMaterial.h"
#include "Base/SpringWindWindow.h"

SpringWindEngine::GameScene::GameScene()
{
}

SpringWindEngine::GameScene::~GameScene()
{
	for (GameObject* pObj : m_ChildPtrArr)
	{
		delete pObj;
	}
	m_ChildPtrArr.clear();
}

void SpringWindEngine::GameScene::AddChild(GameObject* pObj)
{
	pObj->m_pScene = this;
	m_ChildPtrArr.emplace_back(pObj);
}

void SpringWindEngine::GameScene::SetActiveCamera(CameraComponent* pComponent)
{
	assert(pComponent->GetGameObject()->GetScene() == this);
	pComponent->SetProjectionDirty();
	pComponent->SetTransformDirty();
	m_pActiveCamera = pComponent;
}

void SpringWindEngine::GameScene::RootUpdate()
{
	for (GameObject* pObj : m_ChildPtrArr)
	{
		pObj->RootUpdate();
	}
}

void SpringWindEngine::GameScene::Draw(const EngineContext& engineContext, const WindowContext& windowContext, const RenderTarget& mainRT, const RenderTarget& ppRT)
{
	m_pActiveCamera->UpdateFrameData(engineContext);

	for (GameObject* pObj : m_ChildPtrArr)
	{
		pObj->RootDraw(engineContext);
	}

	if (m_pActiveCamera->GetPPMaterials().size() > 0)
	{
		engineContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		engineContext.pDeviceContext->GSSetShader(nullptr, 0, 0);
		engineContext.pDeviceContext->HSSetShader(nullptr, 0, 0);
		engineContext.pDeviceContext->DSSetShader(nullptr, 0, 0);

		engineContext.pDeviceContext->IASetInputLayout(nullptr);
		engineContext.pDeviceContext->VSSetShader(SpringWind::GetInstance()->GetPPVertexShader(), 0, 0);

		// Currently only fullscreen post process, no stencil support, and no depth buffer binding as shader resource
		bool switched = false;
		for (PPMaterial* pMaterial : m_pActiveCamera->GetPPMaterials())
		{
			static float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

			if (switched)
			{
				engineContext.pDeviceContext->ClearRenderTargetView(mainRT.pRenderTargetView, ClearColor);
				engineContext.pDeviceContext->OMSetRenderTargets(1, &mainRT.pRenderTargetView, nullptr);
				engineContext.pDeviceContext->PSSetShaderResources(0, 1, &ppRT.pShaderResourceView);
			}
			else
			{
				engineContext.pDeviceContext->ClearRenderTargetView(ppRT.pRenderTargetView, ClearColor);
				engineContext.pDeviceContext->OMSetRenderTargets(1, &ppRT.pRenderTargetView, nullptr);
				engineContext.pDeviceContext->PSSetShaderResources(0, 1, &mainRT.pShaderResourceView);
			}

			engineContext.pDeviceContext->PSSetShader(pMaterial->GetShader(), 0, 0);
			pMaterial->BeginDraw();
			engineContext.pDeviceContext->Draw(3, 0);
			pMaterial->EndDraw();
			switched = !switched;
		}

		ID3D11ShaderResourceView* pNullSRV[] = { nullptr };
		engineContext.pDeviceContext->PSSetShaderResources(0, 1, pNullSRV);
		if (switched)
		{
			engineContext.pDeviceContext->OMSetRenderTargets(1, &mainRT.pRenderTargetView, mainRT.pDepthStencilView);
			engineContext.pDeviceContext->ClearDepthStencilView(mainRT.pDepthStencilView, D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH | D3D11_CLEAR_FLAG::D3D11_CLEAR_STENCIL, 1.0f, 0);
			SpriteRenderer::GetInstance()->DrawImmediate(engineContext, windowContext, ppRT.pShaderResourceView, XMFLOAT2(0, 0), XMFLOAT4(1, 1, 1, 1), XMFLOAT2(0, 0), XMFLOAT2(1, 1), 0);
		}
		/*else
		{
			engineContext.pDeviceContext->OMSetRenderTargets(1, &mainRT.pRenderTargetView, mainRT.pDepthStencilView);
		}*/
	}
}